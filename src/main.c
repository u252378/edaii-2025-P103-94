#include "document.h"
#include "query.h"
#include "reverse_index.h"
#include "sort_search.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// function to show all details of a document
void show_full_document(Document *doc) {
    if (!doc) return;
    
    printf("\n=================================\n");
    printf("ID: %d\nTitle: %s\n", doc->doc_id, doc->title);
    printf("Relevance: %.2f\n", doc->relevance);
    printf("---------------------------------\n");
    printf("%s\n", doc->body);

    if (doc->links) {
        printf("Links:\n");
        Link *current = doc->links;
        while (current != NULL) {
            printf("- Document ID: %d\n", current->id);
            current = current->next;
        }
    }
    
    printf("=================================\n");
}

int main(int argc, char **argv) {
    // check if dataset folder path is provided
    if (argc < 2) {
        printf("Usage: %s <dataset_folder>\n", argv[0]);
        return 1;
    }

    // load all docs from the folder
    Document *docs = load_documents_from_folder(argv[1]);
    if (!docs) {
        printf("No documents found or failed to load.\n");
        return 1;
    }

    // build the reverse index for fast keyword lookups
    ReverseIndex *reverse_index = build_reverse_index(docs);
    if (!reverse_index) {
        printf("Failed to build reverse index.\n");
        free_documents(docs);
        return 1;
    }

    // interactive search (MAIN FEATURE OF OUR PROGRAM)
    char query_str[256];
    QueueQueries recent_queries;
    init_queue_query(&recent_queries);

    while (1) {
        printf("\n>>> Enter search query (use -word to exclude, or 'exit' to quit): ");
        fflush(stdout);

        // read input line
        if (fgets(query_str, sizeof(query_str), stdin) == NULL) {
            break;
        }

        // remove newline character
        query_str[strcspn(query_str, "\n")] = '\0';

        // check for exit command
        if (strcmp(query_str, "exit") == 0) {
            break;
        }

        // skip empty input
        if (strlen(query_str) == 0) {
            continue;
        }

        // parse query into keywords (handles INCLUDES/EXCLUDES)
        QueryList *query = parse_query(query_str);
        if (!query || !query->head) {
            printf("\nInvalid query format\n");
            continue;
        }

        // add to recent queries
        enqueue_query(&recent_queries, query_str);

        // show recent searches
        printf("\nRecent searches:\n");
        for (int i = 0; i < recent_queries.size; i++) {
            int index = (recent_queries.start + i) % 3;
            printf("* %s\n", recent_queries.queries[index]);
        }

        // search for documents containing the keywords
        DocumentsList *combined_results = NULL;
        QueryNode *keyword_node = query->head;
        int first_keyword = 1;
        
        while (keyword_node) {
            normalize_keyword(keyword_node->keyword);
            DocumentsList *results = reverseIndexGet(reverse_index, keyword_node->keyword);
            
            if (!results || !results->head) {
                if (keyword_node->type == INCLUDE) {
                    printf("\nNo documents contain all required search terms\n");
                    if (combined_results) free_documents_list(combined_results);
                    combined_results = NULL;
                    break;
                }
                keyword_node = keyword_node->next;
                continue;
            }
            
            remove_duplicate_results(results);
            
            if (first_keyword) {
                combined_results = results;
                first_keyword = 0;
            } 
            else if (keyword_node->type == EXCLUDE) {
                // Handle excluded terms by removing matching documents
                DocumentsList *filtered = malloc(sizeof(DocumentsList));
                filtered->head = NULL;
                filtered->tail = NULL;
                filtered->number_documents = 0;
                
                DocumentsListNode *curr = combined_results->head;
                while (curr) {
                    int should_include = 1;
                    DocumentsListNode *exclude_node = results->head;
                    while (exclude_node) {
                        if (curr->document->doc_id == exclude_node->document->doc_id) {
                            should_include = 0;
                            break;
                        }
                        exclude_node = exclude_node->next;
                    }
                    
                    if (should_include) {
                        DocumentsListNode *new_node = malloc(sizeof(DocumentsListNode));
                        new_node->document = curr->document;
                        new_node->next = NULL;
                        
                        if (filtered->tail) {
                            filtered->tail->next = new_node;
                        } else {
                            filtered->head = new_node;
                        }
                        filtered->tail = new_node;
                        filtered->number_documents++;
                    }
                    curr = curr->next;
                }
                
                free_documents_list(combined_results);
                free_documents_list(results);
                combined_results = filtered;
            }
            else {
                // Normal intersection for included terms
                DocumentsList *intersected = intersect_documents_lists(combined_results, results);
                free_documents_list(combined_results);
                free_documents_list(results);
                combined_results = intersected;
            }
            
            if (!combined_results || !combined_results->head) {
                printf("\nNo documents match all search criteria\n");
                break;
            }
            
            keyword_node = keyword_node->next;
        }

        if (!combined_results || !combined_results->head) {
            free_query_list(query);
            continue;
        }

        // convert results to Document* linked list
        Document *temp_head = NULL;
        DocumentsListNode *node = combined_results->head;
        while (node) {
            node->document->next = temp_head;
            temp_head = node->document;
            node = node->next;
        }

        // sort by relevance
        temp_head = sort_documents_by_relevance(temp_head);

        // display up to 5 results
        printf("\nTop 5 results for '%s':\n", query_str);
        Document *curr = temp_head;
        int displayed_results = 0;
        
        while (curr && displayed_results < 5) {
            printf("\n(%d) %s\n", displayed_results, curr->title);
            printf("---\n");

            // print first 200 characters of body
            if (curr->body) {
                int chars_printed = 0;
                const char *body_ptr = curr->body;
                while (*body_ptr && chars_printed < 200) {
                    putchar(*body_ptr);
                    body_ptr++;
                    chars_printed++;
                }
                if (*body_ptr) printf("...");
            }

            printf("\n---\n");
            printf("relevance score: %.2f\n", curr->relevance);

            curr = curr->next;
            displayed_results++;
        }

        printf("\n[%d total results]\n", combined_results->number_documents);

        // document selection
        if (displayed_results > 0) {
            int selection;
            printf("\nSelect document to view (0-%d): ", (displayed_results-1));
            fflush(stdout);

            if (scanf("%d", &selection) != 1) {
                while (getchar() != '\n');
                printf("Invalid input.\n");
                free_query_list(query);
                free_documents_list(combined_results);
                continue;
            }
            while (getchar() != '\n');

            // find selected document
            curr = temp_head;
            int count = 0;
            while (curr && count < selection) {
                curr = curr->next;
                count++;
            }

            if (curr && count == selection) {
                show_full_document(curr);
            } else {
                printf("Invalid selection.\n");
            }
        }
        
        free_query_list(query);
        free_documents_list(combined_results);
    }

    // free memory before exiting program
    free_reverse_index(reverse_index);
    free_documents(docs);
    free_queue_queries(&recent_queries);

    return 0;
}
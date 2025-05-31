#include "document.h"
#include "query.h"
#include "reverse_index.h"
#include "sort_search.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

//function to display all details of a single document (title, body, links) in the format required in the pdf:
void show_full_document(Document *doc) {
    if (!doc) return; //if the doc pointer is null do nothing
    
    printf("\n=================================\n");
    printf("ID: %d\nTitle: %s\n", doc->doc_id, doc->title);
    printf("Relevance: %.2f\n", doc->relevance);
    printf("---------------------------------\n");
    printf("%s\n", doc->body);

    if (doc->links) { //if doc has external links 
        printf("Links:\n");
        Link *current = doc->links;
        while (current != NULL) { //for each link node in the list loop
            printf("- Document ID: %d\n", current->id); //print id of the linked doc
            current = current->next;
        }
    }
    
    printf("=================================\n");
}

int main(int argc, char **argv) {
    if (argc < 2) { //argument check
        printf("Usage: %s <dataset_folder>\n", argv[0]);
        return 1;
    }

    Document *docs = load_documents_from_folder(argv[1]); //load all docs from the specified folder using the function created in document.c
    if (!docs) { //in case loading fails or no docs were found, exit and print a message
        printf("No documents found or failed to load.\n");
        return 1;
    }

    ReverseIndex *reverse_index = build_reverse_index(docs); //build the reverse index for keyword lookups
    if (!reverse_index) { //if it fails to create the index, exit, free docs and print message 
        printf("Failed to build reverse index.\n");
        free_documents(docs);
        return 1;
    }

    char query_str[256]; //initialize queue of recent searches (just 3)
    QueueQueries recent_queries;
    init_queue_query(&recent_queries);

    while (1) { //main loop for asking for a word to search for
        printf("\n>>> HELLO! Welcome to our program,\n");
        printf("    please enter a word/words that you want to look for\n");
        printf("    (or type 'exit' to finish): ");
        fflush(stdout); //flush stdout to ensure prompt appears (we had to look it up as it was causing errors)

        if (fgets(query_str, sizeof(query_str), stdin) == NULL) { //reads input 
            break;
        }

        query_str[strcspn(query_str, "\n")] = '\0'; //remove the trailing newline character, if present (we also had to look it up)

        if (strcmp(query_str, "exit") == 0) { //exit functionality
            break;
        }

        if (strlen(query_str) == 0) { //if user typed enter, skip and ask again
            continue;
        }

        QueryList *query = parse_query(query_str); //parse the query string into a linked list of keywords
        if (!query || !query->head) {
            printf("\nInvalid query format\n");
            if (query) free_query_list(query);
            continue;
        }

        enqueue_query(&recent_queries, query_str); //add the user’s query string to the recent-queries queue

        printf("\nRecent searches:\n"); //display the last 3 searches
        for (int i = 0; i < recent_queries.size; i++) {
            int index = (recent_queries.start + i) % 3;
            printf("* %s\n", recent_queries.queries[index]);
        }

        DocumentsList *combined_results = NULL; //to combine more than 1 word
        int has_results = 1;
        QueryNode *keyword_node = query->head;

        while (keyword_node && has_results) { //when it matches
            normalize_keyword(keyword_node->keyword); //normalize the word to avoid problems

            DocumentsList *results = reverseIndexGet(reverse_index, keyword_node->keyword); //gets a list of the docs containing the input word/s

            if (!results || !results->head) { //if there are NO docs containing the word
                if (keyword_node->type == INCLUDE) {
                    printf("\nNo documents contain the required term: %s\n", keyword_node->keyword);
                    has_results = 0;
                    if (combined_results) {
                        free_documents_list(combined_results);
                        combined_results = NULL;
                    }
                    if (results) free_documents_list(results); //free combined results to avoid segmentation fault 
                    break;
                }

                if (keyword_node->type == EXCLUDE) { //if keyword type is EXCLUDE and no results, there is nothing to exclude
                    if (results) free_documents_list(results);
                    keyword_node = keyword_node->next;
                    continue;
                }

                if (keyword_node->type == OR) { //if one of the words does not match keep going as it is an OR
                    if (results) free_documents_list(results);
                    keyword_node = keyword_node->next;
                    continue;
                }
            }

            remove_duplicate_results(results); //remove duplicate document IDs from the “results” list

            if (keyword_node->type == EXCLUDE) { //if this keyword is an EXCLUDE, filter combined_results
                if (combined_results) {
                    DocumentsList *filtered = malloc(sizeof(DocumentsList));
                    if (!filtered) {
                        perror("malloc");
                        free_documents_list(results);
                        free_documents_list(combined_results);
                        has_results = 0;
                        break;
                    }
                    filtered->head = NULL;
                    filtered->tail = NULL;
                    filtered->number_documents = 0;

                    DocumentsListNode *curr = combined_results->head;
                    while (curr) {
                        int should_include = 1;
                        DocumentsListNode *exclude_node = results->head;
                        while (exclude_node) {
                            if (curr->document->doc_id ==
                                exclude_node->document->doc_id) {
                                should_include = 0;
                                break;
                            }
                            exclude_node = exclude_node->next;
                        }
                        if (should_include) {
                            DocumentsListNode *new_node =
                                malloc(sizeof(DocumentsListNode));
                            if (!new_node) {
                                perror("malloc");
                                free_documents_list(filtered);
                                free_documents_list(results);
                                free_documents_list(combined_results);
                                has_results = 0;
                                break;
                            }
                            new_node->document = curr->document;
                            new_node->next = NULL;
                            if (!filtered->head) {
                                filtered->head = new_node;
                                filtered->tail = new_node;
                            } else {
                                filtered->tail->next = new_node;
                                filtered->tail = new_node;
                            }
                            filtered->number_documents++;
                        }
                        curr = curr->next;
                    }
                    free_documents_list(combined_results);
                    free_documents_list(results);
                    combined_results = filtered;

                    if (!combined_results->head) {
                        has_results = 0;
                        printf("\nAll results excluded by term: %s\n",
                               keyword_node->keyword);
                        break;
                    }
                } else {
                    free_documents_list(results);
                }
            }
            else {
                if (!combined_results) {
                    combined_results = results;
                } else {
                    DocumentsList *intersected = intersect_documents_lists(combined_results, results);
                    free_documents_list(combined_results);
                    free_documents_list(results);
                    combined_results = intersected;
                    if (!combined_results || !combined_results->head) {
                        has_results = 0;
                        printf("\nNo documents contain all required terms\n");
                        break;
                    }
                }
            }

            keyword_node = keyword_node->next;
        }

        /* 5.8) Si no hay resultados o la lista está vacía, liberar y continuar */
        if (!has_results || !combined_results || !combined_results->head) { //if there are no matching documents, free structures and continue
            if (combined_results) free_documents_list(combined_results);
            free_query_list(query);
            printf("[DEBUG] No results to display for \"%s\".\n", query_str); //we had errors so we used this to troubleshoot
            continue;
        }

        //ranking of matching docs using sort_search:
        Document *temp_head = NULL;
        {
            DocumentsListNode *node2 = combined_results->head;
            while (node2) {
                Document *d = node2->document;
                d->next = temp_head;
                temp_head = d;
                node2 = node2->next;
            }
        }

        temp_head = sort_documents_by_relevance(temp_head, query_str); //compute relevance scores and sort the temp_head list

        printf("\nTop results for '%s':\n", query_str); //display the top 5 results in descending relevance 
        Document *curr_doc = temp_head;
        int displayed = 0;
        while (curr_doc && displayed < 5) {
            printf("\n(%d) %s\n", displayed, curr_doc->title);
            printf("---\n");
            if (curr_doc->body) {
                int cnt = 0;
                const char *p = curr_doc->body;
                while (*p && cnt < 200) { //print a small snippet of the body 
                    putchar(*p);
                    p++;
                    cnt++;
                }
                if (*p) printf("..."); //add ... at the end of the body to indicate there is more text
            }
            printf("\n---\n");
            printf("relevance score: %.2f\n", curr_doc->relevance);
            curr_doc = curr_doc->next; //move to next code 
            displayed++; 
        }
        if (displayed == 0) {
            printf("No results to display.\n");
        }
        printf("\n[%d total results]\n", combined_results->number_documents);

        if (displayed > 0) { //to allow user to view a doc 
            int selection;
            printf("\nSelect document to view (0-%d): ",
                   (displayed - 1));
            fflush(stdout);

            if (scanf("%d", &selection) != 1) {
                while (getchar() != '\n') { }
                printf("Invalid input.\n");
                free_query_list(query);
                free_documents_list(combined_results);
                continue;
            }
            while (getchar() != '\n') { }

            if (selection >= 0 && selection < displayed) {
                curr_doc = temp_head;
                for (int k = 0; k < selection; k++) {
                    curr_doc = curr_doc->next; //advance to the chosen doc
                }
                show_full_document(curr_doc); //display details 
            } else {
                printf("Invalid selection.\n");
            }
        }
        
        //free temporary data structures :
        free_query_list(query);
        free_documents_list(combined_results);
        temp_head = NULL; 
    }
    //free reverse index and the docs:
    reverseIndexFree(reverse_index, true);
    free_documents(docs);
    free_queue_queries(&recent_queries);

    return 0;
}
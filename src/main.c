#include "document.h"
#include "query.h"
#include "reverse_index.h"
#include "sort_search.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> // for tolower()
#include <string.h>

// function to remove duplicate results in case there is any:
void remove_duplicate_results(DocumentsList *results) {
    if (!results || !results->head) return; // if the list is empty do nothing 
    
    DocumentsListNode *current = results->head;
    while (current) {
        DocumentsListNode *runner = current;
        while (runner->next) {
            if (runner->next->document->doc_id == current->document->doc_id) { // if next node is a duplicate (same document ID)
                DocumentsListNode *temp = runner->next;
                runner->next = runner->next->next; // skip the duplicate 
                free(temp); // free the removed node
            } 
            else {
                runner = runner->next; // move to the next node
            }
        }
        current = current->next; // move to the next doc to compare
    }
}

// function to show all details of a document: 
void show_full_document(Document *doc) {
    if (!doc) return;
    
    printf("\n=================================\n");
    printf("ID: %d\nTitle: %s\n", doc->doc_id, doc->title);
    printf("Relevance: %.2f\n", doc->relevance);
    printf("---------------------------------\n");
    printf("%s\n", doc->body);

    if (doc->links) { // if the document contains links to other documents
        printf("Links:\n");
        Link *current = doc->links;
        while (current != NULL) {
            printf("- Document ID: %d\n", current->id); // print each linked document's ID
            current = current->next;
        }
    }
    
    printf("=================================\n");
}

int main(int argc, char **argv) {
    // check if dataset folder path is provided:
    if (argc < 2) {
        printf("Usage: %s <dataset_folder>\n", argv[0]);
        return 1;
    }

    // load all docs from the folder
    Document *docs = load_documents_from_folder(argv[1]);
    if (!docs) {
        printf("No documents found or failed to load.\n");
        return 1; // EXIT with error if loading fails
    }

    // build the reverse index for fast keyword lookups
    ReverseIndex *reverse_index = build_reverse_index(docs);
    if (!reverse_index) {
        printf("Failed to build reverse index.\n");
        free_documents(docs);
        return 1;
    }

    // interactive search (MAIN FEATURE OF OUR PROGRAM)
    char keyword[256];
    QueueQueries recent_queries;
    init_queue_query(&recent_queries);

    while (1) {
        printf("\n>>> HELLO! Welcome to our program, please enter a word/words that you want to look for (or type 'exit' to finish): ");
        fflush(stdout); // force immediate display

        // read input line
        if (fgets(keyword, sizeof(keyword), stdin) == NULL) {
            break; // exit on EOF
        }

        // remove newline character
        keyword[strcspn(keyword, "\n")] = '\0';

        // check for exit command
        if (strcmp(keyword, "exit") == 0) {
            break;
        }

        // skip empty input
        if (strlen(keyword) == 0) {
            continue;
        }

        normalize_keyword(keyword); // normalize the search term

        // add to recent queries
        enqueue_query(&recent_queries, keyword);

        // show recent searches
        printf("\nRecent searches:\n");
        for (int i = 0; i < recent_queries.size; i++) {
            int index = (recent_queries.start + i) % 3;
            printf("* %s\n", recent_queries.queries[index]);
        }

        // search for documents containing the keyword
        DocumentsList *results = reverseIndexGet(reverse_index, keyword);
        if (!results || !results->head) {
            printf("\nNo documents contain '%s'\n", keyword);
            continue;
        }

        remove_duplicate_results(results);

        // convert results to Document* linked list
        Document *temp_head = NULL;
        DocumentsListNode *node = results->head;
        while (node) {
            node->document->next = temp_head;
            temp_head = node->document;
            node = node->next;
        }

        // sort by relevance (using the new implementation)
        temp_head = sort_documents_by_relevance(temp_head);

        // print top results
        printf("\nTop 5 results for '%s':\n", keyword);  // Fixed to say "Top 5" instead of "Top 10"
        Document *curr = temp_head;
        int index = 0, total_results = 0;

        // count total results
        Document *counter = temp_head;
        while (counter) {
            total_results++;
            counter = counter->next;
        }

        // display up to 5 results with preview
        while (curr && index < 5) {
            printf("\n(%d) %s\n", index, curr->title);
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
                if (*body_ptr) printf("..."); // show ellipsis if there's more text
            }

            printf("\n---\n");
            printf("relevance score: %.2f\n", curr->relevance);

            curr = curr->next;
            index++;
        }

        printf("\n[%d total results]\n", total_results);

        // document selection
        if (index > 0) {
            int selection;
            printf("\nSelect document to view (0-%d): ", index-1);
            fflush(stdout);

            if (scanf("%d", &selection) != 1) {
                while (getchar() != '\n'); // clear input buffer
                printf("Invalid input.\n");
                continue;
            }
            while (getchar() != '\n'); // clear remaining input

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
    }

    // free memory before exiting program
    free_reverse_index(reverse_index);
    free_documents(docs);
    free_queue_queries(&recent_queries);

    return 0;
}
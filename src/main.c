#include "document.h"
#include "query.h"
#include "reverse_index.h"
#include "sort_search.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> //for tolower()
#include <string.h> //for strlen()

//function to remove duplicate results in case there is any:
void remove_duplicate_results(DocumentsList *results) {
    if (!results || !results->head) return; //if the list is empty do nothing 
    
    DocumentsListNode *current = results->head;
    while (current) {
        DocumentsListNode *runner = current;
        while (runner->next) {
            if (runner->next->document->doc_id == current->document->doc_id) { //if next node is a duplicate (same document ID)
                DocumentsListNode *temp = runner->next;
                runner->next = runner->next->next; //skip the duplicate 
                free(temp); //free the removed node
            } 
            else {
                runner = runner->next; //move to the next node
            }
        }
        current = current->next; //move to the next doc to compare
    }
}

//function to show all details of a document: 
void show_full_document(Document *doc) {
    if (!doc) return;
    
    printf("\n=================================\n");
    printf("ID: %d\nTitle: %s\n", doc->doc_id, doc->title);
    printf("Relevance: %.2f\n", doc->relevance);
    printf("---------------------------------\n");
    printf("%s\n", doc->body);

    if (doc->links) { //if the document contains links to other documents
        printf("Links:\n");
        Link *current = doc->links;
        while (current != NULL) {
            printf("- Document ID: %d\n", current->id); //print each linked document's ID
            current = current->next;
        }
    }
    
    printf("=================================\n");
}

int main(int argc, char **argv) {
    //check if dataset folder path is provided:
    if (argc < 2) {
        printf("Usage: %s <dataset_folder>\n", argv[0]);
        return 1;
    }
    //load all docs from the folder
    Document *docs = load_documents_from_folder(argv[1]);
    if (!docs) {
        printf("No documents found or failed to load.\n");
        return 1; //EXIT with error if loading fails
    }
    Document *doc_current = docs; //now, we compute the relevance score for each doc
    while (doc_current) {
        doc_current->relevance = calculate_relevance(doc_current,  "");
        doc_current = doc_current->next;
    }
    //build the reverse index for fast keyword lookups
    ReverseIndex *reverse_index = build_reverse_index(docs);
    if (!reverse_index) {
        printf("Failed to build reverse index.\n");
        free_documents(docs);
        return 1;
    }

    //interactive search (MAIN FEATURE OF OUR PROGRAM)
    char keyword[100];
    QueueQueries recent_queries;
    init_queue_query(&recent_queries);
    do {
        printf("\nHELLO! Welcome to our program, please enter a word/words that you want to look for (or type 'exit' to finish): "); //ASK THE USER FOR A SEARCH TERM
        scanf("%99s", keyword); //reads user input
        normalize_keyword(keyword); //normalize the word entered
        if (strcmp(keyword, "exit") == 0) {
            //free memory and exit
            free_reverse_index(reverse_index);
            free_documents(docs);
            free_queue_queries(&recent_queries);
            return 0;
        }

        // Add search term to recent queries
        enqueue_query(&recent_queries, keyword);

        // Show recent searches in the box format
        printf("******* recent searches ********\n");
        for (int i = 0; i < 3; ++i) {
            int index = (recent_queries.start + i) % 3;
            if (recent_queries.size > i)
                printf("* %s *\n", recent_queries.queries[index]);
        }
        printf("********************************\n");

        DocumentsList *results = reverseIndexGet(reverse_index, keyword);
        if (!results || !results->head) {
            printf("No documents contain the word '%s'.\n", keyword);
            continue;
        }

        remove_duplicate_results(results);

        // Convert results to Document* linked list
        Document *temp_head = NULL;
        DocumentsListNode *node = results->head;
        while (node) {
            node->document->next = temp_head;
            temp_head = node->document;
            node = node->next;
        }

        // Sort by relevance
        temp_head = sort_documents_by_relevance(temp_head, keyword);

        // Print top 5 formatted results
        printf("\n");
        Document *curr = temp_head;
        int index = 0, total_results = 0;

        // Count total results first
        Document *counter = temp_head;
        while (counter) {
            total_results++;
            counter = counter->next;
        }

        // Display up to 5 most relevant results
        while (curr && index < 5) {
            printf("(%d) %s\n", index, curr->title);
            printf("---\n");

            // Print first ~200 characters of body
            if (curr->body) {
                int max_chars = 200;
                int chars_printed = 0;
                const char *body_ptr = curr->body;
                
                while (*body_ptr && chars_printed < max_chars) {
                    putchar(*body_ptr);
                    body_ptr++;
                    chars_printed++;
                }
                
                if (*body_ptr) {
                    printf("..."); // Add ellipsis if text continues
                }
            }
            printf("\n---\n");
            printf("relevance score: %.0f\n", curr->relevance);
            
            curr = curr->next;
            index++;
        }

        printf("[%d results]\n", total_results);

        // Ask user to select a document to view
        int selection;
        printf("\nSelect document: ");
        if (scanf("%d", &selection) != 1) {
            while (getchar() != '\n'); // clean input
            continue;
        }
        while (getchar() != '\n');

        // Find and display the selected document
        curr = temp_head;
        int count = 0;
        while (curr && count < selection) {
            curr = curr->next;
            count++;
        }

        if (curr && count == selection) {
            print_document_compact(curr); // Use the new standardized print function
        }

    } while (1);

    printf("\n=== All documents ===\n");
    Document *current = docs;
    while (current) {
        print_document_details(current); //prints ID, title, body and links
        current = current->next;
    }

    printf("\n=== Documents sorted by relevance ===\n");
    docs = sort_documents_by_relevance(docs,  "");
    print_sorted_documents(docs); //prints title and relevance score   

    //free memory before exiting program
    free_reverse_index(reverse_index);
    free_documents(docs);
    free_queue_queries(&recent_queries);
    return 0;
}
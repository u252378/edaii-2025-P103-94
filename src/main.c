#include "document.h"
#include "query.h"
#include "reverse_index.h"
#include "sort_search.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> //for tolower()

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

//function to let user select a document from a list and view its full content:
void select_document(DocumentsList *results) {
    if (!results || !results->head) { //if the result list is NULL or empty, print a message and return
        printf("No documents available.\n");
        return;
    }

    //show numbered list
    printf("\nSearch results:\n");
    //traverse the results list and print numbered titles with their document IDs
    DocumentsListNode *node = results->head;
    int count = 1;
    while (node) {
        printf("%d. %s (ID: %d)\n", count++, node->document->title, node->document->doc_id); //print each document's number, title, and ID
        node = node->next; //move to the next doc
    }

    int choice;
    printf("\nEnter document number to view (0 to cancel): ");
    //read the user's choice and validate input type
    if (scanf("%d", &choice) != 1) {
        while (getchar() != '\n'); //clear input buffer
        return;
    }
    while (getchar() != '\n'); //clear the newline

    //if the input is a valid choice (within range)
    if (choice > 0 && choice < count) {
        node = results->head;
        for (int i = 1; i < choice; i++) node = node->next; //traverse the list to the selected document
        show_full_document(node->document); //show the full document details
    }
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
        doc_current->relevance = calculate_relevance(doc_current);
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
    do {
        printf("\nHELLO! Welcome to our program, please enter a word/words that you want to look for (or type 'exit' to finish): "); //ASK THE USER FOR A WORD/WORDS
        scanf("%99s", keyword); //reads user input
        normalize_keyword(keyword); //call the function to normalize the word entered
        if (strcmp(keyword, "exit") == 0) {
            //free memory and exit
            free_reverse_index(reverse_index);
            free_documents(docs);
            return 0;
        }
        DocumentsList *results = reverseIndexGet(reverse_index, keyword); //search for documents that contain our desired keywords
        if (!results || !results->head) { //if the word is not in any doc then print the following:
            printf("No documents contain the word '%s'.\n", keyword);
        } 
        else {
            remove_duplicate_results(results); //this will remove any duplicates in case there is any
      select_document(results); 
    remove_duplicate_results(results);
    
        }
    } while (1); //we did this in order for our code to repeat unless user types "exit"

    printf("\n=== All documents ===\n");
    Document *current = docs;
    while (current) {
        print_document_details(current); //prints ID, title, body and links
        current = current->next;
    }

    printf("\n=== Documents sorted by relevance ===\n");
    docs = sort_documents_by_relevance(docs);
    print_sorted_documents(docs); //prints title and relevance score   

    //free memory before exiting program
    free_reverse_index(reverse_index);
    free_documents(docs);
    return 0;
}
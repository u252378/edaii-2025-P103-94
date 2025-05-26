#include <stdio.h>
#include <stdlib.h>
#include "document.h"
#include "query.h"
#include "reverse_index.h"
#include "sort_search.h"

int main(int argc, char** argv) {
    //we check if the user provided the dataset folder path as a command-line argument:
    if (argc < 2) {
        //if that is not the case, we will print the usage instructions and exit.
        printf("Usage: %s <dataset_folder>\n", argv[0]);
        return 1; //EXIT wih error.
    }

    //now, this loads all documents from the specified folder path:
    Document* docs = load_documents_from_folder(argv[1]);
    ReverseIndex *index = reverseIndexInit(1000); // or another appropriate slot count
    build_reverse_index(index, docs); // assuming this is your function to populate it

    //check if loading failed or no documents were found, we will notify the user and exit.
    if (!docs) {
        printf("No documents found or failed to load.\n");
        return 1; //EXIT wih error.
    }
    print_reverse_index(index);  
    free_reverse_index(index);  // frre memory
    //start traversing the linked list of loaded documents:
    Document* current = docs;
    while (current) {
        print_document_details(current); //this will print the details of the current document (ID, title, body, and links)
        current = current->next; //to move to the next document in the linked list,
    }
   docs = sort_documents_by_relevance(docs); // sorts the documents
   print_sorted_documents(docs);             // prints sorted documents
   free_documents(docs); //when finished, we have to free all the memory allocated for the document list.
   return 0;
}
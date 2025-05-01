#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "document.h"


int contains_substring(const char* text, const char* keyword) { //function to search if the body contains a substring
    return strstr(text, keyword) != NULL; //found
}

int main(int argc, char** argv) {
    if (argc < 3) { //we check if the user has introduced at least two arguments
        printf("Not enough arguments");
        return 1; //error
    }

  
    Document* docs = load_documents_from_folder(argv[1]);  // we load the documents
    if (!docs) {
        printf("No documents found");
        return 1; //if no documents found 
    }

    // QUERY has a list of keywords
    int num_keywords = argc - 2;
    char** keywords = &argv[2];//pointer to where the keywords start

    // we search trough all the documents
    printf("Search results:\n\n");
    Document* current = docs;
    while (current) { //for the document in the documents
        int matches_all_keywords = 1; //if it matches=true

        for (int i = 0; i < num_keywords; ++i) { //for the keyword in QUERY
            if (!contains_substring(current->body, keywords[i])) { //if not matches all the keywords
                matches_all_keywords = 0; //false
                break;
            }
        }

        if (matches_all_keywords) { //if it matches
            print_document_details(current); //we print
        }

        current = current->next; //pass on to the next
    }

    free_documents(docs); //we free the memory
    return 0;

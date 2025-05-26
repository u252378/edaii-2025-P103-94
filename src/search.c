#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "document.h"
#include "query.h"

int contains_substring(const char* text, const char* keyword) { //function to search if the body contains a substring
    return strstr(text, keyword) != NULL; //found
}

int main2(int argc, char** argv) {
    if (argc < 3) { //we check if the user has introduced at least two arguments
        printf("Not enough arguments");
        return 1; //error
    }

    Document* docs = load_documents_from_folder(argv[1]);  // we load the documents
    if (!docs) {
        printf("No documents found");
        return 1; //if no documents found 
    }

    //parse the query into a QueryList (handles INCLUDES/EXCLUDES)
    QueryList* query = parse_query(argv[2]);
    if (!query) {
        printf("Failed to parse query.\n");
        free_documents(docs);
        return 1;
    }

    // we search trough all the documents
    printf("Search results:\n\n");
    Document* current = docs;
    int found_count = 0; //track matches for better output

    while (current) {
        int should_include = 1; //to determine if doc matches all conditions
        QueryNode* keyword_node = query->head; //iterate through query keywords

        while (keyword_node) {
            int keyword_found = contains_substring(current->body, keyword_node->keyword);

            //handle INCLUDED keywords (must match)
            if (keyword_node->type == INCLUDE && !keyword_found) {
                should_include = 0;
                break;
            }

            //handle EXCLUDED keywords (must NOT match)
            if (keyword_node->type == EXCLUDE && keyword_found) {
                should_include = 0;
                break;
            }

            keyword_node = keyword_node->next;
        }

        if (should_include) { //only print if all conditions passed
            print_document_details(current);
            found_count++;
        }

        current = current->next; //pass on to the next
    }

    printf("\n[%d results]\n", found_count); //show total matches 
 
    free_query_list(query);
    free_documents(docs); //we free the memory
    return 0;
}
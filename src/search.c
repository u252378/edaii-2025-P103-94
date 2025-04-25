//in this file we will implement the document linear search functionality 
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "document.h"
#include "query.h"

//helper function: checks if keyword appears in the given text.
bool contains_keyword(const char* text, const char* keyword) {
    return strstr(text, keyword) != NULL; //IMPORTANT: strstr returns a pointer if keyword is found.
}

//determines whether a document matches all query criteria:
bool document_matches_query(const Document* doc, const Query* query) {
    bool or_matched = false; //keeps track if any OR keyword matched.
    bool has_or = false; //keeps track if the query contains any OR keyword.

    for (const Query* q = query; q != NULL; q = q->next) {
        bool in_title = contains_keyword(doc->title, q->word); //check in title
        bool in_body = contains_keyword(doc->body, q->word); //check in body 
        bool found = in_title || in_body; //this will be true if keyword is found ANYWHERE.

        if (q->type == INCLUDE && !found)
            return false; //if required keyword not found, reject document.

        if (q->type == EXCLUDE && found)
            return false; //if excluded keyword is found, reject document

        if (q->type == OR) {
            has_or = true; //this marks that we have an OR condition.
            if (found) or_matched = true; //if one OR keyword matched, mark as matched.
        }
    }

    return has_or ? or_matched : true; //if ORs exist, return true only if one matched
}

//searches through all documents and returns those matching the query:
Document* search_documents(const Document* all_docs, const Query* query, int max_results) {
    Document* result_head = NULL;
    Document* result_tail = NULL;
    int count = 0;

    while (all_docs && count < max_results) {
        if (document_matches_query(all_docs, query)) {
            // Copy basic info to a new node (shallow copy of strings & links for now)
            Document* copy = malloc(sizeof(Document));
            *copy = *all_docs;
            copy->next = NULL;

            if (!result_head) result_head = result_tail = copy;
            else {
                result_tail->next = copy;
                result_tail = copy;
            }

            count++;
        }
        all_docs = all_docs->next;
    }

    return result_head;
}

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "document.h"
#include "query.h"

bool contains_keyword(const char* text, const char* keyword) {
    return strstr(text, keyword) != NULL;
}

bool document_matches_query(const Document* doc, const Query* query) {
    bool or_matched = false;
    bool has_or = false;

    for (const Query* q = query; q != NULL; q = q->next) {
        bool in_title = contains_keyword(doc->title, q->word);
        bool in_body = contains_keyword(doc->body, q->word);
        bool found = in_title || in_body;

        if (q->type == INCLUDE && !found)
            return false;

        if (q->type == EXCLUDE && found)
            return false;

        if (q->type == OR) {
            has_or = true;
            if (found) or_matched = true;
        }
    }

    return has_or ? or_matched : true;
}

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

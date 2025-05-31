#include "sort_search.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

float calculate_relevance(Document *doc, const char *keyword) {
    if (!doc || !keyword) return 0.0f;
    
    float relevance = 0.0f;
    
    // Keyword frequency in body (60% weight)
    if (doc->body) {
        const char *body_ptr = doc->body;
        int keyword_count = 0;
        int word_count = 0;
        char word[256];
        int word_idx = 0;
        
        while (*body_ptr) {
            if (isalpha((unsigned char)*body_ptr)) {
                word[word_idx++] = tolower((unsigned char)*body_ptr);
            } else if (word_idx > 0) {
                word[word_idx] = '\0';
                word_count++;
                if (strcmp(word, keyword) == 0) {
                    keyword_count++;
                }
                word_idx = 0;
            }
            body_ptr++;
        }
        
        if (word_count > 0) {
            relevance += 0.6f * ((float)keyword_count / word_count);
        }
    }
    
    // Keyword in title (30% weight)
    if (doc->title && strstr(doc->title, keyword) != NULL) {
        relevance += 0.3f;
    }
    
    // Outgoing links (10% weight)
    Link *link = doc->links;
    int unique_links = 0;
    while (link) {
        if (link->id != doc->doc_id) {
            unique_links++;
        }
        link = link->next;
    }
    relevance += 0.1f * (unique_links > 5 ? 1.0f : (float)unique_links / 5.0f);
    
    return relevance;
}

void split_list(Document *source, Document **front, Document **back) {
    Document *slow = source;
    Document *fast = source->next;
    
    while (fast) {
        fast = fast->next;
        if (fast) {
            slow = slow->next;
            fast = fast->next;
        }
    }
    
    *front = source;
    *back = slow->next;
    slow->next = NULL;
}

Document *sorted_merge(Document *a, Document *b) {
    Document *result = NULL;
    
    if (!a) return b;
    if (!b) return a;
    
    if (a->relevance >= b->relevance) {
        result = a;
        result->next = sorted_merge(a->next, b);
    } else {
        result = b;
        result->next = sorted_merge(a, b->next);
    }
    
    return result;
}

void sort_by_relevance(Document **headRef) {
    Document *head = *headRef;
    if (!head || !head->next) return;
    
    Document *a, *b;
    split_list(head, &a, &b);
    sort_by_relevance(&a);
    sort_by_relevance(&b);
    *headRef = sorted_merge(a, b);
}

Document *sort_documents_by_relevance(Document *docs, const char *keyword) {
    Document *current = docs;
    while (current) {
        current->relevance = calculate_relevance(current, keyword);
        current = current->next;
    }
    
    sort_by_relevance(&docs);
    return docs;
}
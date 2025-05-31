#include "sort_search.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// helper to count word occurrences (case-insensitive)
static int count_word_occurrences(const char *text, const char *word) {
    if (!text || !word) return 0;
    
    int count = 0;
    char *text_copy = strdup(text);
    char *word_copy = strdup(word);
    
    // convert to lowercase
    for (int i = 0; text_copy[i]; i++) text_copy[i] = tolower(text_copy[i]);
    for (int i = 0; word_copy[i]; i++) word_copy[i] = tolower(word_copy[i]);
    
    // count occurrences
    char *pos = text_copy;
    while ((pos = strstr(pos, word_copy))) {
        count++;
        pos += strlen(word_copy);
    }
    
    free(text_copy);
    free(word_copy);
    return count;
}

// helper to count links
static int count_links(Link *links) {
    int count = 0;
    while (links) {
        if (links->id != 0) { // ignore empty links
            count++;
        }
        links = links->next;
    }
    return count;
}

// function to calculate relevance score of a document
float calculate_relevance(Document *doc, const char *search_term) {
    if (!doc) return 0.0f;
    
    float relevance = 0.0f;
    
    // 1. weight for title matches (40%)
    if (doc->title && search_term) {
        relevance += count_word_occurrences(doc->title, search_term) * 0.4f;
    }
    
    // 2. weight for body matches (30%)
    if (doc->body && search_term) {
        relevance += count_word_occurrences(doc->body, search_term) * 0.3f;
    }
    
    // 3. weight for title length (20%)
    if (doc->title) {
        relevance += (strlen(doc->title) / 100.0f) * 0.2f;
    }
    
    // 4. weight for outgoing links (10%)
    relevance += count_links(doc->links) * 0.1f;
    
    return relevance;
}

// this function splits a linked list into two halves
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

// this function merges two sorted lists based on document relevance
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

// this function sorts the linked list using merge sort algorithm
void sort_by_relevance(Document **headRef) {
    Document *head = *headRef;
    if (!head || !head->next) return;
    
    Document *a, *b;
    split_list(head, &a, &b);
    sort_by_relevance(&a);
    sort_by_relevance(&b);
    *headRef = sorted_merge(a, b);
}

// function to sort documents by relevance
Document *sort_documents_by_relevance(Document *docs, const char *search_term) {
    // calculate relevance for all docs first
    Document *current = docs;
    while (current) {
        current->relevance = calculate_relevance(current, search_term);
        current = current->next;
    }
    
    // then perform the sort
    sort_by_relevance(&docs);
    return docs;
}

// function to print sorted documents
void print_sorted_documents(const Document *docs) {
    while (docs) {
        printf("Title: %s\n", docs->title);
        printf("Relevance: %.2f\n", docs->relevance);
        docs = docs->next;
    }
}
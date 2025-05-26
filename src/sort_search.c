#include <stdlib.h>
#include <stdio.h>
#include "sort_search.h"

void sort_by_relevance(Document** headRef);
// this function splits a linked list into two halves, use this for merge sort (divide step).
void split_list(Document* source, Document** front, Document** back) {
    Document* slow = source;       // slow pointer (moves one step at a time)
    Document* fast = source->next; // fast pointer (moves two steps at a time)

    while (fast) { // move fast pointer until it reaches the end of the list
        fast = fast->next;
        if (fast) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // now slow is in the middle of the list
    *front = source;           // front part starts at the beginning
    *back = slow->next;        // back part starts after the middle
    slow->next = NULL;         // break the list into two parts
}

// this function merges two sorted lists based on document relevance.// the most relevant (higher score) document comes first
Document* sorted_merge(Document* a, Document* b) {
    Document* result = NULL;
    // base cases: if one list is empty
    if (!a) return b;
    if (!b) return a;

    if (a->relevance >= b->relevance) {  // compare relevance values to decide the order
        result = a;
        result->next = sorted_merge(a->next, b); // merge the rest
    } else {
        result = b;
        result->next = sorted_merge(a, b->next); // merge the rest
    }

    return result;
}
Document* sort_documents_by_relevance(Document *docs) {//sorting
      sort_by_relevance(&docs);  // sorts using merge sort
    return docs;   
}

void print_sorted_documents(const Document *docs) {//print sorted docuemnts
      while (docs) {
        printf("Title: %s\n", docs->title);
        printf("Relevance: %.2f\n" ,docs->relevance);
        docs = docs->next;
    }
}
// this function sorts the linked list using merge sort algorithm (because It has O(n log n) time complexity in all cases), it sorts documents in descending order by relevance score
void sort_by_relevance(Document** headRef) {
    Document* head = *headRef;
    
    // base case: if list is empty or has only one element
    if (!head || !head->next) return;

    Document* a; // front half
    Document* b; // back half

    // split the list into 'a' and 'b' halves
    split_list(head, &a, &b);

    // recursively sort both halves
    sort_by_relevance(&a);
    sort_by_relevance(&b);

    // merge the two sorted halves
    *headRef = sorted_merge(a, b);
}

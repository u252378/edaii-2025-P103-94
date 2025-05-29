#include "sort_search.h"
#include <stdio.h>
#include <stdlib.h>
float calculate_relevance(Document *doc) {
    if (!doc) return 0.0f;
    
    float relevance = 0.0f;
    
    // 1. Peso por longitud del cuerpo (50%)
    if (doc->body) {
        relevance += strlen(doc->body) / 2000.0f; // +1.0 por cada 2000 caracteres
    }
    
    // 2. Peso por links salientes (30%)
    Link *link = doc->links;
    int unique_links = 0;
    while (link) {
        if (link->id != doc->doc_id) { // Ignorar autoreferencias
            unique_links++;
        }
        link = link->next;
    }
    relevance += unique_links * 0.3f;
    
    // 3. Peso por título (20%)
    if (doc->title) {
        relevance += strlen(doc->title) * 0.05f;
    }
    
    return relevance;
}
void sort_by_relevance(Document **headRef);
// this function splits a linked list into two halves, use this for merge sort
// (divide step).
void split_list(Document *source, Document **front, Document **back) {
  Document *slow = source;       // slow pointer (moves one step at a time)
  Document *fast = source->next; // fast pointer (moves two steps at a time)

  while (fast) { // move fast pointer until it reaches the end of the list
    fast = fast->next;
    if (fast) {
      slow = slow->next;
      fast = fast->next;
    }
  }

  // now slow is in the middle of the list
  *front = source;    // front part starts at the beginning
  *back = slow->next; // back part starts after the middle
  slow->next = NULL;  // break the list into two parts
}

// this function merges two sorted lists based on document relevance.// the most
// relevant (higher score) document comes first
Document *sorted_merge(Document *a, Document *b) {
  Document *result = NULL;
  // base cases: if one list is empty
  if (!a)
    return b;
  if (!b)
    return a;

  if (a->relevance >=
      b->relevance) { // compare relevance values to decide the order
    result = a;
    result->next = sorted_merge(a->next, b); // merge the rest
  } else {
    result = b;
    result->next = sorted_merge(a, b->next); // merge the rest
  }

  return result;
}
Document *sort_documents_by_relevance(Document *docs) { // sorting
  sort_by_relevance(&docs); // sorts using merge sort
  return docs;
}

void print_sorted_documents(const Document *docs) { // print sorted docuemnts
  while (docs) {
    printf("Title: %s\n", docs->title);
    printf("Relevance: %.2f\n", docs->relevance);
    docs = docs->next;
  }
}
// this function sorts the linked list using merge sort algorithm (because It
// has O(n log n) time complexity in all cases), it sorts documents in
// descending order by relevance score
void sort_by_relevance(Document **headRef) {
    Document *current = *headRef;
    // Primero calcular relevancia
    while (current) {
        current->relevance = calculate_relevance(current);
        current = current->next;
    }
    
    // Luego ordenar (tu implementación existente)
    Document *head = *headRef;
    if (!head || !head->next) return;
    
    Document *a, *b;
    split_list(head, &a, &b);
    sort_by_relevance(&a);
    sort_by_relevance(&b);
    *headRef = sorted_merge(a, b);
}
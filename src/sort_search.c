#include "sort_search.h"
#include <stdio.h>
#include <stdlib.h>

//function to calculate the relevance score of a document based on body length, number of links, and title length:
float calculate_relevance(Document *doc) {
  if (!doc) return 0.0f;
  
  float relevance = 0.0f; //initialize relevance score 
  
  //1. Weight for body length (50%)
  if (doc->body) {
    relevance += strlen(doc->body) / 2000.0f; //add 1.0 for every 2000 characters in the body
  }
  
  //2. Weight for outgoing links (30%)
  Link *link = doc->links; //start from first liink
  int unique_links = 0;
  while (link) {
    if (link->id != doc->doc_id) { //ignore self-referencing links 
      unique_links++;
    }
    link = link->next;
  }
  relevance += unique_links * 0.3f; //each unique link adds 0.3 to relevance
  
  //3. Weight for title length (20%)
  if (doc->title) {
    relevance += strlen(doc->title) * 0.05f; //add 0.05 per character in the title
  }
  return relevance;
}

void sort_by_relevance(Document **headRef);

// this function splits a linked list into two halves, use this for merge sort (divide step).
void split_list(Document *source, Document **front, Document **back) {
  Document *slow = source; // slow pointer (moves one step at a time)
  Document *fast = source->next; // fast pointer (moves two steps at a time)

  while (fast) { // move fast pointer until it reaches the end of the list
    fast = fast->next;
    if (fast) {
      slow = slow->next;
      fast = fast->next;
    }
  }

  // now slow is in the middle of the list
  *front = source; // front part starts at the beginning
  *back = slow->next; // back part starts after the middle
  slow->next = NULL; // break the list into two parts
}

// this function merges two sorted lists based on document relevance.
// the most relevant (higher score) document comes first
Document *sorted_merge(Document *a, Document *b) {
  Document *result = NULL;
  // base cases: if one list is empty
  if (!a)
    return b;
  if (!b)
    return a;

  if (a->relevance >=b->relevance) { // compare relevance values to decide the order
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

// this function sorts the linked list using merge sort algorithm (because It has O(n log n) time complexity in all cases), it sorts documents in descending order by relevance score
void sort_by_relevance(Document **headRef) {
  Document *current = *headRef;
  //fisrt calculate relevance:
  while (current) {
    current->relevance = calculate_relevance(current);
    current = current->next;
  }
  
  //the sort
  Document *head = *headRef;
  if (!head || !head->next) return;
  
  Document *a, *b;
  split_list(head, &a, &b);
  sort_by_relevance(&a);
  sort_by_relevance(&b);
  *headRef = sorted_merge(a, b);
}
#include "../src/document.h"
#include <stdio.h>
#include <stdlib.h>

// function to count number document in docuemnts linked list
int number_documents_list(Document *head) {
  int count = 0;            // to know the current documents in list
  Document *current = head; // pointer to know current document of list

  // iterates through all docuemnt linked list
  while (current != NULL) {
    count++;                 // increments counter of documents
    current = current->next; // moves current pointer to point to the next
                             // document in list
  }
  return count;
}

// Test 1 (empty list)
int test1() {
  Document *head = NULL;

  // check documents of list, and print appropiate message
  if (number_documents_list(head) != 0) {
    printf("Test 1 failed\n");
    return 0;
  }
  printf("Test 1 passed\n");
  return 1;
}

// Test 2 (one docuemnt in document linked list)
int test2() {
  // allocate memory, and initialise all fields to NULL for the first element of
  // linked list
  Document *doc = calloc(1, sizeof(Document));
  if (doc == NULL) {
    printf("Memory allocation failed\n");
    return 0;
  }

  // set only the fields of first element in linked list required to do the test
  doc->doc_id = 4;
  doc->next = NULL;

  if (number_documents_list(doc) != 1) {
    printf("Test 2 failed\n");
    return 0;
  }
  if (doc->doc_id != 4) {
    printf("Test 2 failed\n");
    return 0;
  }
  if (doc->next != NULL) {
    printf("Test 2 failed\n");
    return 0;
  }

  // freeing the allocated memory for first element of the linked list
  free_documents(doc);

  printf("Test 2 passed\n");
  return 1;
}

// Test 3 (two documents in document linked list)
int test3() {
  // allocate memory, and initialise fields to NULL for the two element of
  // linked list
  Document *doc1 = calloc(1, sizeof(Document));
  Document *doc2 = calloc(1, sizeof(Document));
  if (doc1 == NULL || doc2 == NULL) {
    printf("Memory allocation failed\n");
    if (doc1 != NULL) {
      free_documents(doc1);
    }
    if (doc2 != NULL) {
      free_documents(doc2);
    }
    return 0;
  }

  // set only the required fields to do the test of the two elements of linked
  // list allocated above
  doc1->doc_id = 1;
  doc2->doc_id = 2;
  doc1->next = doc2;
  doc2->next = NULL;

  if (number_documents_list(doc1) != 2) {
    printf("Test 3 failed\n");
    free_documents(doc1);
    return 0;
  }
  if (doc1->doc_id != 1) {
    printf("Test 3 failed\n");
    free_documents(doc1);
    return 0;
  }
  if (doc2->doc_id != 2) {
    printf("Test 3 failed\n");
    free_documents(doc1);
    return 0;
  }
  if (doc1->next != doc2) {
    printf("Test 3 failed\n");
    free_documents(doc1);
    return 0;
  }
  if (doc2->next != NULL) {
    printf("Test 3 failed\n");
    free_documents(doc1);
    return 0;
  }

  // freeing the memory allocated for the two elements of linked list
  free_documents(doc1);

  printf("Test 3 passed\n");
  return 1;
}

int main() {
  int passed = 0;

  int result1 = test1();
  int result2 = test2();
  int result3 = test3();

  passed = result1 + result2 + result3;

  printf("The number of passed test are %d", passed);
  return 0;
}

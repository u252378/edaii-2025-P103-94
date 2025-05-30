#include "../src/document.h"
#include "../src/reverse_index.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// function to do 1 unit test for the search functionality using hashmap
int search_using_hashmap() {

  // to have two new documents in memory, and initialised
  Document* doc1 =  malloc(sizeof(Document));
  if (doc1 == NULL) {
    printf("Memory allocation failed\n");
    return 1;
  }
  doc1 -> doc_id = 1;
  doc1 -> title = malloc(sizeof(char) * 6);
  strcpy(doc1-> title, "Doc 1");

  doc1 -> body = malloc(sizeof(char) * 25);
  strcpy(doc1 -> body, "apple banana orange");

  doc1 -> links = NULL;
  doc1 -> relevance = 0;
  doc1 -> next = NULL;

  Document* doc2 =  malloc(sizeof(Document));
  if (doc2 == NULL) {
    printf("Memory allocation failed\n");
    return 1;
  }
  doc2 -> doc_id = 2;
  doc2 -> title = malloc(sizeof(char) * 6);
  strcpy(doc2-> title, "Doc 2");

  doc2 -> body = malloc(sizeof(char) * 30);
  strcpy(doc2 -> body, "banana orange strawberry");

  doc2 -> links = NULL;
  doc2 -> relevance = 0;
  doc2 -> next = NULL;

  // doc1 next pointer now points to doc2
  doc1 -> next = doc2;

  // calling function to build reverse index
  ReverseIndex *index = build_reverse_index(doc1);
  if (index == NULL) {
    printf("function build_reverse_index failed\n");
    free_documents(doc1);
    return 1;
  }

  // calling function to check the documents that contain the word 'banana'
  DocumentsList * result = reverseIndexGet(index, "banana");
  if (result == NULL) {
    printf("No documents found for 'banana'\n");
    reverseIndexFree(index, true, false);
    free_documents(doc1);
    return 1;
  }

  // check both documents containing the word 'banana' were found
  if (result -> number_documents == 2) {
    printf("Found 2 documents containing 'banana'\n");
  } else {
    printf("There were 2 documents expected containing 'banana' and this was not the case\n");
    reverseIndexFree(index, true, false);
    free_documents(doc1);
    return 1;
  }

  // free reverse index and documents
  reverseIndexFree(index, true, false);
  free_documents(doc1);
  return 0;
}


int main() {
  int result = search_using_hashmap();
  if (result == 0) {
    printf("test passed\n");
  } else {
    printf("Test failed\n");
  }

  return 0;
}
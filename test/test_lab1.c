// in this document we will create all unit tests for lab 1:
/*
*** Write at least 1 unit test for the document parsing functionality
*** Write at least 3 unit tests for the documents linked list
*** Write at least 3 unit tests for the links linked list
*/

#include "document.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// we need a helper function to create a temporary test document file to test
// all unit tests:
void create_test_document(const char *path, const char *content) {
  FILE *file = fopen(path, "w");
  if (!file) {
    perror("Failed to create test file");
    exit(1);
  }
  fprintf(file, "%s", content);
  fclose(file);
}

// unit test for doc parsing:
/*We want to ensure that our function document_desserialize() works correclty
We also want to catch bugs in ID/title/body/link extraction*/
void test_document_parsing() {
  printf("Running test for document parsing...\n");

  const char
      *test_content = // simulates a real document file to test parsing logic
      "123\n"
      "Test Title\n"
      "This is a test body with a [link](456).\n";
  create_test_document(
      "test_doc.txt",
      test_content); // create a temporary file with the data we just entered

  Document *doc = document_desserialize(
      "test_doc.txt"); // Now onto the proper testing, we call our document
                       // parsing function to convert the file into a Document
                       // struct
  if (doc == NULL) {
    printf(
        "FAILED: document_desserialize returned NULL\n"); // First, check if
                                                          // parsing succeeded
                                                          // (Document pointer
                                                          // not NULL), that way
                                                          // we ensure function
                                                          // doesn’t crash on
                                                          // valid input
    return;
  }

  if (doc->doc_id != 123) { // now, verify that the doc ID was parsed correctly
                            // meaning it has read it as an integer
    printf("FAILED: Expected doc_id=123, got %d\n", doc->doc_id);
    free_document(doc);
    return;
  }

  if (strcmp(doc->title, "Test Title") !=
      0) { // it compares the parsed title to the expected title to check that
           // the second line of the file was correctly stored in doc->title
    printf("FAILED: Expected title='Test Title', got '%s'\n", doc->title);
    free_document(doc);
    return;
  }

  if (strstr(doc->body, "test body") ==
      NULL) { // validates that the body text was parsed correctly
    printf("FAILED: Body text is missing or incorrect\n");
    free_document(doc);
    return;
  }

  if (doc->links ==
      NULL) { // check if the links list was initialized, by seeing if the
              // parser detected the link [link](456) in the body
    printf("FAILED: No links found in document\n");
    free_document(doc);
    return;
  }

  if (doc->links->id != 456) { // verifies the extracted link ID matches 456
    printf("FAILED: Expected link ID = 456, got %d\n", doc->links->id);
    free_document(doc);
    return;
  }

  free_document(
      doc); // IMPORTANT: frees the Document struct to avoid memory leaks
  printf("PASSED\n"); // if all tests were passed it will print this successful
                      // message
}
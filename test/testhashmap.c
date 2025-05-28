#include "document.h" // our document type
#include "hashmap.h"  // our hashmap
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_getFromHashMap() { // tests if get in the hashmap works correctly
  printf("Running test_getFromHashMap...");
  HashMap *map = createHashMap(10);              // make hashmap with 10 slots
  Document *doc = malloc(sizeof(Document));      // allocate memory for document
  doc->title = strdup("doc1");                   // set title
  doc->content = strdup("test content");         // set content
  insertToHashMap(map, doc->title, doc);         // key = "doc1", value = doc
  Document *found = getFromHashMap(map, "doc1"); // search for key "doc1"
  assert(found != NULL);                         // fail if document not found
  assert(strcmp(found->title, "doc1") == 0);     // title should be "doc1"
  assert(strcmp(found->content, "test content") == 0); // content should match
  Document *not_found = getFromHashMap(
      map, "does_not_exist"); // try and search for somethin that doesn´t exist
  assert(not_found == NULL);  // should return NULL
  printf(" test_getFromHashMap passed."); // if passed
  free(doc->title);                       // free the copied title string
  free(doc->content);                     // free the copied content string
  free(doc);                              // free the document itself
  freeHashMap(map); // free the entire hashmap (like deleting the phone book)

  int main() {
    test_getFromHashMap(); // run the test
    return 0;              // tell the computer everything worked
  }

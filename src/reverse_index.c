#include "reverse_index.h"
#include "document.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
ReverseIndex *reverseIndexInit(int slots) { // function to create the reverse index (the hash table)
  ReverseIndex *index =
      malloc(sizeof(ReverseIndex)); // allocate memory for the index
  index->slotsCount = slots; // save number of slots
  index->unique_keywords = 0; // start with 0 keys
  index->slots = calloc(
      slots, sizeof(ReverseIndexSlot *)); // allocate and zero out the slots
  return index; // return the new index
}

static int hash(char *word,int slotsCount) { // function to hash a word into a number
  unsigned long hash = 5381; // initial value
  int c;
  while ((c = *word++)) { // loop through each character in the word
    hash = ((hash << 5) + hash) + c; // hash * 33 + c
  }
  return hash % slotsCount; // limit hash to fit in slots array
}

void reverseIndexPut(ReverseIndex *index, char *word, DocumentsListNode *node) {
  int slot = hash(word, index->slotsCount);
  ReverseIndexSlot *slotPtr = index->slots[slot]; // get slot pointer

  if (!slotPtr) { // in case slot is empty
    slotPtr = malloc(sizeof(ReverseIndexSlot));
    slotPtr->keys = NULL; 
    slotPtr->keysCount = 0;
    index->slots[slot] = slotPtr;
  }

  ReverseIndexKey *key = slotPtr->keys;
  while (key) {
    if (strcmp(key->word, word) == 0) {
      DocumentsListNode *current = key->values->head;
      while (current) {
        if (current->document->doc_id == node->document->doc_id) {
          return; // document already indexed for this word
        }
        current = current->next;
      }

      // Create a copy of the node
      DocumentsListNode *copyNode = malloc(sizeof(DocumentsListNode));
      if (!copyNode) return;
      copyNode->document = node->document;
      copyNode->next = NULL;

      if (!key->values->head) {
        key->values->head = copyNode;
        key->values->tail = copyNode;
      } else {
        key->values->tail->next = copyNode;
        key->values->tail = copyNode;
      }
      key->values->number_documents++;
      return;
    }
    key = key->next;
  }

  // Word not found: create a new key
  ReverseIndexKey *newKey = malloc(sizeof(ReverseIndexKey));
  newKey->word = strdup(word);

  DocumentsList *newList = malloc(sizeof(DocumentsList));
  newList->head = node;
  newList->tail = node;
  newList->number_documents = 1;
  node->next = NULL;

  newKey->values = newList;
  newKey->next = slotPtr->keys;
  slotPtr->keys = newKey;
  slotPtr->keysCount++;
  index->unique_keywords++;
}


DocumentsList *reverseIndexGet(ReverseIndex *index,char *word) { // function to get the list of documents for a word
  int slot = hash(word, index->slotsCount); // get index using hash
  ReverseIndexSlot *slotPtr = index->slots[slot]; // get the slot

  if (!slotPtr)
    return NULL; // if no slot

  ReverseIndexKey *key = slotPtr->keys; // start with first key in slot
  while (key) {
    if (strcmp(key->word, word) == 0) { // if found the word
      return key->values; // return the document list
    }
    key = key->next; // try next key
  }
  return NULL; // word not found
}

void reverseIndexFree(ReverseIndex *index, bool freeLists) {
    if (!index) return;

    for (int i = 0; i < index->slotsCount; i++) {
        ReverseIndexSlot *slot = index->slots[i];
        if (!slot) continue;

        ReverseIndexKey *key = slot->keys;
        while (key) {
            ReverseIndexKey *nextKey = key->next;
            
            if (freeLists && key->values) {
                // Only free the DocumentsList structure and nodes
                DocumentsListNode *current = key->values->head;
                while (current) {
                    DocumentsListNode *next = current->next;
                    // Explicitly don't free current->document
                    free(current);
                    current = next;
                }
                free(key->values);
                key->values = NULL; // Prevent dangling pointer
            }
            
            free(key->word);
            free(key);
            key = nextKey;
        }
        free(slot);
        index->slots[i] = NULL; // Prevent dangling pointer
    }
    free(index->slots);
    free(index);
}
//**normalize words in the parser (uppercase, punctuation, etc.) [LAB 3]:
//fuction to convert all words to lowercase,and also to only accept letters, and store the final word in the same place
void normalize_keyword(char *word) {
  int i = 0, j = 0;
  while (word[i]) {
    if (isalpha((unsigned char)word[i])) {
      word[j++] = tolower((unsigned char)word[i]); //if it is in uppercase change it to lowercase
    }
    i++; //move to the next char
  }
  word[j] = '\0'; //null terminate the clean word
}

void reverseIndexDocument(ReverseIndex *reverse_index, Document *document) {
  if (!reverse_index || !document || !document->body) return;

  char *text = strdup(document->body);
  if (!text) {
    return;  // fail en strdup
  }

  char *token = strtok(text, " \t\n\r.,;:!?()[]{}<>\""); //split by whitespace and punctuation
  while (token != NULL) {
    normalize_keyword(token); //clean token 

    if (token[0] != '\0') { //skip empty results
      DocumentsListNode *list = malloc(sizeof(DocumentsListNode));
      if (list) { 
        list->document = document;
        list->next = NULL;
        reverseIndexPut(reverse_index, token, list); //insert token into index 
       


      }
    }
    token = strtok(NULL, " \t\n\r.,;:!?()[]{}<>\"");
  }
  free(text);
}

void reverseIndexSaveToFile(ReverseIndex *index, const char *filename) { // save the word index to a text file
  FILE *file = fopen(filename, "w"); // open the file for writing 
  if (!file) {
    printf("Couldn't open %s for writing\n", filename);
    return;
  }

  for (int i = 0; i < index->slotsCount; i++) { // loop through each slot in the index
    ReverseIndexSlot *slot = index->slots[i];
    if (!slot)
      continue; // skip empty slots
    ReverseIndexKey *key = slot->keys; // for each word in this slot
    while (key) { // write the word followed by colon
      fprintf(file, "%s:", key->word);
      DocumentsListNode *doc =
        key->values->head; // write all documents that contain this word
      while (doc) {
        if (doc->document && doc->document->title) {
          fprintf(file, "%s,", doc->document->title); // write document title followed by comma
        }
        doc = doc->next;
      }
      fprintf(file, "\n"); // end this word's line
      key = key->next; // go to the next word
    }
  }
  fclose(file); // close the file
}

// load the word index from a text file:
void reverseIndexLoadFromFile(ReverseIndex *index, const char *filename, Document *(*getDocByTitle)(const char *)) { 
  FILE *file = fopen(filename, "r"); // open the file in read mode
  if (!file) {
    printf("Couldn't open %s for reading\n", filename);
    return;
  }
  // read file line by line
  char line[1000]; // buffer to hold each line
  while (fgets(line, sizeof(line), file)) {
    line[strcspn(line, "\n")] = '\0'; // remove the newline at the end
    char *colon = strchr(line, ':'); // split line into word and documents parts
    if (!colon)
      continue; // skip bad lines
    *colon = '\0'; // split the string in two
    char *word = line; // part before colon
    char *documents = colon + 1; // part after colon
    char *docTitle = strtok(documents, ","); // split documents by commas
    while (docTitle) { // find the document with this title
      Document *doc = getDocByTitle(docTitle);
      if (doc) {
        DocumentsListNode *node = malloc(sizeof(DocumentsListNode));
        node->next = NULL; // initializes the next of the node
        node->document = doc;
        reverseIndexPut(index, word, node); // add to index
      }
      docTitle = strtok(NULL, ","); // get next document title
    }
  }

  fclose(file); // close the file
}

ReverseIndex *build_reverse_index(Document *docs) {
  ReverseIndex *index = reverseIndexInit(101); //initialize index with 101 slots
  while (docs) {
    reverseIndexDocument(index, docs); //index each doc
    docs = docs->next;
  }
  return index;
}

void print_reverse_index(const ReverseIndex *index) {
  reverseIndexSaveToFile((ReverseIndex *)index, "reverse_index.txt"); //save index to file
}

void free_reverse_index(ReverseIndex *index) {
    reverseIndexFree(index, true);  // Only free index structures
}

#ifndef REVERSE_INDEX_H
#define REVERSE_INDEX_H
#include "document.h"

typedef struct reverseIndexKey { // this struct represents a keyword
  char *word;                    // the actual keyword
  DocumentsList *values; // linked list of documents that contain this word
  struct reverseIndexKey *next; // next keyword
} ReverseIndexKey;

typedef struct reverseIndexSlot { // this struct is one slot in the hash table
  ReverseIndexKey *keys;          // pointer to the first keyword in this slot
  int keysCount;                  // how many keywords are in this slot
} ReverseIndexSlot;

typedef struct reverseIndex {
  int slotsCount; // total number of slots in the hash table
  int unique_keywords;
  ReverseIndexSlot **slots; // pointer to the array of slots
} ReverseIndex;

ReverseIndex *
reverseIndexInit(int slots); // create and initialize the reverse index
void reverseIndexPut(ReverseIndex *index, char *word,
                     DocumentsListNode *node); // add a word and its associated
                                               // document list to the index
void free_reverse_index(ReverseIndex *index);

DocumentsList *reverseIndexGet(
    ReverseIndex *index,
    char *word); // get the list of documents that contain the given word

void reverseIndexFree(ReverseIndex *index, bool freeLists, bool freeDocs);
void normalise_word(char *word);
void reverseIndexDocument(ReverseIndex *index,
                          Document *document); // tokenize a document and add
                                               // all its words to the index
void reverseIndexSaveToFile(
    ReverseIndex *index,
    const char *filename); // saves the reverse index to a file
void reverseIndexLoadFromFile(
    ReverseIndex *index, const char *filename,
    Document *(*getDocByTitle)(const char *)); // loads a reverse index from a
                                               // file into an existing index
ReverseIndex *build_reverse_index(Document *docs);
void print_reverse_index(const ReverseIndex *index);

#endif

#ifndef HASHMAP_H // prevents multiple inclusions of this header file
#define HASHMAP_H // marks the beginning of the header definition
#include "document.h"

// first, we will define a structure of the node in the hash map's linked list (which we will use for separate chaining)
typedef struct HashNode {
  char *key; // example: document ID
  Document *value; // pointer to the associated Document
  struct HashNode *next; // pointer to the next node in the chain (for avoiding collisions)
} HashNode;

// now, we define the HashMap structure itself:
typedef struct {
  int size; // number of buckets in the hash table
  HashNode **buckets; // array of pointers to HashNode chains (the table)
} HashMap;

// Hashmap functions:
HashMap *createHashMap(int size); // function to create a new hash map with the given number of buckets:
void insertToHashMap(HashMap *map, const char *key, Document *value); // inserts a key-value pair (document ID and Document*) into the map
Document *getFromHashMap(HashMap *map, const char *key); // retrieves the Document* associated with the given key
void freeHashMap(HashMap *map); // frees all memory allocated by the hash map (but not the Document* unless managed)

#endif
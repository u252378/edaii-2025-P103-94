#include "hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// hash function for strings:
unsigned int hash(const char *key, int size) {
  unsigned int hashValue = 0;
  for (int i = 0; key[i] != '\0';
       i++) { // loops through each character of the string
    hashValue = hashValue * 31 +
                key[i]; // common practice: multiply by a prime and add the char
  }
  return hashValue % size; // return index within the table size
}

// creates a new hash map with a specified number of buckets:
HashMap *createHashMap(int size) {
  HashMap *map =
      malloc(sizeof(HashMap)); // we need to allocate memory for the HashMap
  map->size = size;            // now, set the number of buckets
  map->buckets = calloc(
      size,
      sizeof(HashNode *)); // allocate memory and initialize all buckets to NULL
  return map;              // finally, return the new hash map
}

// inserts a key-value pair into the hash map:
void insertToHashMap(HashMap *map, const char *key, Document *value) {
  unsigned int index = hash(key, map->size); // compute hash index
  HashNode *node = map->buckets[index];      // get the head of the bucket chain

  // this will check if key already exists in the chain:
  while (node != NULL) {
    if (strcmp(node->key, key) == 0) { // if key matches, update the value
      node->value = value; // overwrite the value (doesn't duplicate Document)
      return;
    }
    node = node->next; // move to the next node in the chain
  }

  // in case key is not found, create a new node and insert at the beginning of
  // the list:
  HashNode *newNode = malloc(sizeof(HashNode)); // allocate memory for new node
  newNode->key = strdup(key); // duplicate the key string (allocates new memory)
  newNode->value = value;     // assign the Document* as value
  newNode->next = map->buckets[index]; // point new node to current head
  map->buckets[index] = newNode;       // make new node the head of the chain
}

// retrieves the value (Document*) associated with a given key:
Document *getFromHashMap(HashMap *map, const char *key) {
  unsigned int index = hash(key, map->size); // compute hash index
  HashNode *node = map->buckets[index];      // get head of the chain

  while (node != NULL) {
    if (strcmp(node->key, key) == 0) { // if key matches, return its value
      return node->value;
    }
    node = node->next; // move to next node
  }

  return NULL; // return NULL if key is not found
}

// free memory:
void freeHashMap(HashMap *map) {
  for (int i = 0; i < map->size; i++) { // loop through all buckets
    HashNode *node = map->buckets[i];   // get the head of the chain
    while (node != NULL) {
      HashNode *temp = node; // store current node
      node = node->next;     // move to next before freeing

      free(temp->key); // free the duplicated key string
      free(temp);      // free the node (does not free Document*)
    }
  }
  free(map->buckets); // free the array of buckets
  free(map);          // free the map structure itself
}
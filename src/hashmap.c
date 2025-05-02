#include <stdlib.h>       // For malloc, free
#include <string.h>       // For strcmp, strdup
#include "hashmap.h"      // Include the corresponding header file

// Hash function to compute an index for a given key
unsigned int hash(char* key) {
    unsigned int hash = 0;              // Start with a hash value of 0
    while (*key) {                      // Loop through each character in the key string
        hash = (hash * 31) + *key++;    // Multiply hash by 31 and add the ASCII value of the character
    }
    return hash % TABLE_SIZE;           // Ensure the hash fits within the table bounds
}

// Create and initialize a new hashmap
HashMap* create_hashmap() {
    HashMap* map = malloc(sizeof(HashMap));  // Allocate memory for the hashmap
    for (int i = 0; i < TABLE_SIZE; i++) {
        map->table[i] = NULL;                // Initialize each bucket to NULL
    }
    return map;                              // Return the initialized map
}

// Insert or update a key-value pair into the hashmap
void put(HashMap* map, char* key, int value) {
    unsigned int index = hash(key);          // Compute hash index for the key
    Entry* current = map->table[index];      // Get the head of the linked list at that index

    while (current != NULL) {                // Traverse the list to see if key already exists
        if (strcmp(current->key, key) == 0) {  // If key found, update value
            current->value = value;
            return;
        }
        current = current->next;             // Move to next node
    }

    // If key was not found, insert a new entry at the head of the list
    Entry* new_entry = malloc(sizeof(Entry));     // Allocate memory for new entry
    new_entry->key = strdup(key);                // Duplicate the key string
    new_entry->value = value;                    // Set the value
    new_entry->next = map->table[index];         // Point to previous head of the list
    map->table[index] = new_entry;               // Update head to new entry
}

// Retrieve the value associated with a key from the hashmap
int get(HashMap* map, char* key, int* found) {
    unsigned int index = hash(key);           // Compute index for the key
    Entry* current = map->table[index];       // Get head of the list at index

    while (current != NULL) {                 // Traverse the linked list
        if (strcmp(current->key, key) == 0) { // If key matches
            *found = 1;                       // Mark as found
            return current->value;            // Return the value
        }
        current = current->next;              // Move to next node
    }

    *found = 0;                               // Key not found
    return 0;                                 // Return default value
}

// Free all memory used by the hashmap
void free_hashmap(HashMap* map) {
    for (int i = 0; i < TABLE_SIZE; i++) {        // Iterate over all buckets
        Entry* current = map->table[i];           // Get head of the list at current index
        while (current != NULL) {                 // Traverse the list
            Entry* temp = current;                // Save current node
            current = current->next;              // Move to next node
            free(temp->key);                      // Free the duplicated key string
            free(temp);                           // Free the node itself
        }
    }
    free(map);                                    // Finally, free the map structure
}
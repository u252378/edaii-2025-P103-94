#ifndef HASHMAP_H //prevent multiple inclusions of this header file
#define HASHMAP_H

#define TABLE_SIZE 100 //define the size of the hash table (number of buckets)

// Structure representing a single key-value pair in the hashmap
typedef struct entry {
    char* key;           // Pointer to a dynamically allocated key string
    int value;           // Value associated with the key
    struct entry* next;  // Pointer to the next entry (for handling collisions using chaining)
} Entry;

// Structure representing the hashmap itself
typedef struct {
    Entry* table[TABLE_SIZE];  // Array of pointers to linked lists (buckets)
} HashMap;

// Function to create and initialize a new HashMap
HashMap* create_hashmap();

// Function to insert or update a key-value pair in the hashmap
void put(HashMap* map, char* key, int value);

// Function to retrieve the value associated with a key
// Sets *found = 1 if key is found, *found = 0 otherwise
int get(HashMap* map, char* key, int* found);

// Function to free all memory used by the hashmap
void free_hashmap(HashMap* map);

#endif  // End of include guard
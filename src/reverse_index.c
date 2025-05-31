#include "reverse_index.h"
#include "document.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Function to create the reverse index (the hash table)
ReverseIndex *reverseIndexInit(int slots) {
    ReverseIndex *index = malloc(sizeof(ReverseIndex)); // Allocate memory for the index
    if (!index) {
        fprintf(stderr, "Memory allocation failed for ReverseIndex.\n");
        return NULL;
    }

    index->slotsCount = slots;          // Save the number of slots
    index->unique_keywords = 0;        // Start with 0 unique keys
    index->slots = calloc(slots, sizeof(ReverseIndexSlot *)); // Allocate and zero out the slots
    if (!index->slots) {
        fprintf(stderr, "Memory allocation failed for ReverseIndex slots.\n");
        free(index);
        return NULL;
    }
    return index; // Return the newly created index
}

// Function to hash a word into a number
static int hash(char *word, int slotsCount) {
    unsigned long hash = 5381; // Initial hash value
    int c;
    while ((c = *word++)) { // Loop through each character in the word
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash % slotsCount; // Limit hash to fit within the slots array
}

// Add a word and its associated document node to the index
void reverseIndexPut(ReverseIndex *index, char *word, DocumentsListNode *node) {
    if (!index || !word || !node) return;

    int slot = hash(word, index->slotsCount);
    ReverseIndexSlot *slotPtr = index->slots[slot]; // Get slot pointer

    // If the slot is empty, create a new slot
    if (!slotPtr) {
        slotPtr = malloc(sizeof(ReverseIndexSlot));
        if (!slotPtr) {
            fprintf(stderr, "Memory allocation failed for ReverseIndexSlot.\n");
            return;
        }
        slotPtr->keys = NULL;
        slotPtr->keysCount = 0;
        index->slots[slot] = slotPtr;
    }

    // Search for the word in the slot's keys
    ReverseIndexKey *key = slotPtr->keys;
    while (key) {
        if (strcmp(key->word, word) == 0) {
            // Word found, check if the document is already indexed
            DocumentsListNode *current = key->values->head;
            while (current) {
                if (current->document->doc_id == node->document->doc_id) {
                    free(node); // Document already indexed for this word, free the node
                    return;
                }
                current = current->next;
            }

            // Add the document to the list
            node->next = NULL;
            if (!key->values->head) {
                key->values->head = node;
                key->values->tail = node;
            } else {
                key->values->tail->next = node;
                key->values->tail = node;
            }
            key->values->number_documents++;
            return;
        }
        key = key->next;
    }

    // Word not found: create a new key
    ReverseIndexKey *newKey = malloc(sizeof(ReverseIndexKey));
    if (!newKey) {
        fprintf(stderr, "Memory allocation failed for ReverseIndexKey.\n");
        free(node);
        return;
    }
    newKey->word = strdup(word);
    if (!newKey->word) {
        fprintf(stderr, "Memory allocation failed for word.\n");
        free(newKey);
        free(node);
        return;
    }

    // Create a new list for the word
    DocumentsList *newList = malloc(sizeof(DocumentsList));
    if (!newList) {
        fprintf(stderr, "Memory allocation failed for DocumentsList.\n");
        free(newKey->word);
        free(newKey);
        free(node);
        return;
    }
    newList->head = node;
    newList->tail = node;
    newList->number_documents = 1;
    node->next = NULL;

    // Add the new key to the slot
    newKey->values = newList;
    newKey->next = slotPtr->keys;
    slotPtr->keys = newKey;
    slotPtr->keysCount++;
    index->unique_keywords++;
}

// Get the list of documents for a word
DocumentsList *reverseIndexGet(ReverseIndex *index, char *word) {
    if (!index || !word) return NULL;

    int slot = hash(word, index->slotsCount); // Get index using hash
    ReverseIndexSlot *slotPtr = index->slots[slot]; // Get the slot

    if (!slotPtr) return NULL; // If no slot, return NULL

    ReverseIndexKey *key = slotPtr->keys; // Start with the first key in the slot
    while (key) {
        if (strcmp(key->word, word) == 0) { // If the word matches
            return key->values; // Return the document list
        }
        key = key->next; // Try the next key
    }
    return NULL; // Word not found
}

// Free all memory allocated for the reverse index
void reverseIndexFree(ReverseIndex *index, bool freeLists) {
    if (!index) return;

    for (int i = 0; i < index->slotsCount; i++) {
        ReverseIndexSlot *slot = index->slots[i];
        if (!slot) continue;

        ReverseIndexKey *key = slot->keys;
        while (key) {
            ReverseIndexKey *nextKey = key->next;

            if (freeLists && key->values) {
                // Free the DocumentsList structure and its nodes
                DocumentsListNode *current = key->values->head;
                while (current) {
                    DocumentsListNode *next = current->next;
                    // Do not free current->document here; it is managed elsewhere
                    free(current);
                    current = next;
                }
                free(key->values);
            }

            free(key->word);
            free(key);
            key = nextKey;
        }
        free(slot);
    }
    free(index->slots);
    free(index);
}

// Normalize a word (convert to lowercase, remove non-alphabetic characters)
void normalize_keyword(char *word) {
    if (!word) return;

    int i = 0, j = 0;
    while (word[i]) {
        if (isalpha((unsigned char)word[i])) {
            word[j++] = tolower((unsigned char)word[i]); // Convert to lowercase
        }
        i++;
    }
    word[j] = '\0'; // Null-terminate the cleaned word
}

// Tokenize a document and add all its words to the reverse index
void reverseIndexDocument(ReverseIndex *reverse_index, Document *document) {
    if (!reverse_index || !document || !document->body) return;

    char *text = strdup(document->body);
    if (!text) {
        fprintf(stderr, "Memory allocation failed for document body duplicate.\n");
        return;
    }

    char *token = strtok(text, " \t\n\r.,;:!?()[]{}<>\""); // Split by whitespace and punctuation
    while (token != NULL) {
        normalize_keyword(token); // Clean the token

        if (token[0] != '\0') { // Skip empty results
            DocumentsListNode *list = malloc(sizeof(DocumentsListNode));
            if (!list) {
                fprintf(stderr, "Memory allocation failed for DocumentsListNode.\n");
                break;
            }
            list->document = document;
            list->next = NULL; // Ensure the next pointer is initialized
            reverseIndexPut(reverse_index, token, list); // Insert token into the index
        }
        token = strtok(NULL, " \t\n\r.,;:!?()[]{}<>\"");
    }
    free(text); // Free the duplicate of the document body
}

// Build a reverse index from a list of documents
ReverseIndex *build_reverse_index(Document *docs) {
    ReverseIndex *index = reverseIndexInit(101); // Initialize index with 101 slots
    if (!index) return NULL;

    while (docs) {
        reverseIndexDocument(index, docs); // Index each document
        docs = docs->next;
    }
    return index;
}
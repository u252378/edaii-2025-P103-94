#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "reverse_index.h" 


ReverseIndex *reverseIndexInit(int slots) { // function to create the reverse index (the hash table)
    ReverseIndex *index = malloc(sizeof(ReverseIndex)); // allocate memory for the index
    index->slotsCount = slots; // save number of slots
    index->keys = 0; // start with 0 keys
    index->slots = calloc(slots, sizeof(ReverseIndexSlot*)); // allocate and zero out the slots
    return index; // return the new index
}


static int hash(char *word, int slotsCount) { // function to hash a word into a number 
    unsigned long hash = 5381; // initial value 
    int c;
    while ((c = *word++)) { // loop through each character in the word
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash % slotsCount; // limit hash to fit in slots array
}


void reverseIndexPut(ReverseIndex *index, char *word, DocumentsList *list) {// function to add a word and the associated document list to the index
    int slot = hash(word, index->slotsCount); // get index from hash function
    ReverseIndexSlot *slotPtr = index->slots[slot]; // access the correct slot

    if (!slotPtr) { //if slot is empty
        slotPtr = malloc(sizeof(ReverseIndexSlot)); //allocate 
        slotPtr->keys = NULL; // start with empty key list
        slotPtr->keysCount = 0;
        index->slots[slot] = slotPtr; // save in the index
    }

    ReverseIndexKey *key = slotPtr->keys; // go through keys in slot
    while (key) {
        if (strcmp(key->word, word) == 0) { // if word already exists
            // add document list to existing word
            DocumentsList *curr = key->values;
            while (curr->next) curr = curr->next; // go to the end of the list
            curr->next = list; // add new documents to the end
            return;
        }
        key = key->next; // try next key 
    }

    // word not found, create new key entry
    ReverseIndexKey *newKey = malloc(sizeof(ReverseIndexKey)); // new key
    newKey->word = strdup(word); // copy the word
    newKey->values = list; // set its documents list
    newKey->next = slotPtr->keys; // insert at beginning of list
    slotPtr->keys = newKey; // update head of key list
    slotPtr->keysCount++; // increase number of keys in this slot
    index->keys++; // increase total keys in index
}


DocumentsList *reverseIndexGet(ReverseIndex *index, char *word) {// function to get the list of documents for a word
    int slot = hash(word, index->slotsCount); // get index using hash
    ReverseIndexSlot *slotPtr = index->slots[slot]; // get the slot

    if (!slotPtr) return NULL; // if no slot

    ReverseIndexKey *key = slotPtr->keys; // start with first key in slot
    while (key) {
        if (strcmp(key->word, word) == 0) { // if found the word
            return key->values; // return the document list
        }
        key = key->next; // try next key 
    }
    return NULL; // word not found
}


void reverseIndexFree(ReverseIndex *index, bool freeLists, bool freeDocs) {// free memory used by the index
    for (int i = 0; i < index->slotsCount; i++) { //go through all slots
        ReverseIndexSlot *slot = index->slots[i];
        if (slot) {
            ReverseIndexKey *key = slot->keys;
            while (key) {
                ReverseIndexKey *nextKey = key->next;
                free(key->word); // free the word

                if (freeLists) {
                    DocumentsList *docList = key->values;
                    while (docList) {
                        DocumentsList *nextDoc = docList->next;
                        if (freeDocs && docList->doc) {
                            // free document if requested
                            free(docList->doc->title);
                            free(docList->doc->body);
                            free(docList->doc);
                        }
                        free(docList); // free the list node
                        docList = nextDoc;
                    }
                }
                free(key); // free the key struct
                key = nextKey;
            }
            free(slot); // free the slot 
        }
    }
    free(index->slots); // free the array of slots
    free(index); // free the index
}

void reverseIndexDocument(ReverseIndex *index, Document *document) { // tokenizes document body and adds all words to the reverse index???
    char *text = strdup(document->body); // copy the body 
    char *token = strtok(text, " \t\n\r.,;:!?()[]{}<>\""); // first word

    while (token != NULL) {
        DocumentsList *list = malloc(sizeof(DocumentsList)); // create new list
        list->doc = document; // set current document
        list->next = NULL; // end of list
        reverseIndexPut(index, token, list); // add word to index
        token = strtok(NULL, " \t\n\r.,;:!?()[]{}<>\""); // next word
    }

    free(text); // free the copied text
}


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


// Normalise words in the parser (uppercase, puncation, etc.)fuction to convert all words to lowercase,and also to only accept letters,
//and store the final word in the same place 
void normalise_word(char *keyword) {
    int i = 0; // index for reading original word
    int j = 0; // index for writing the changed word

    // iterates until the last characteris not null terminator
    while (keyword[i] != '\0') {
        char c = keyword[i];

        // checking if character is uppercase, and changing it to lowercase by adding 32
        if (c >= 'A' && c >= 'Z') {
            c = c + 32;
            keyword[j] = c;
            j++;
        } else if (c >= 'a' && c <= 'z') { // character is already in lowercase
            keyword[j] = c;
            j++;
        }


        i++;
    }

    // then after converting to lowercase, it adds a null terminator at the end
    keyword[j] = '\0';
}


void reverseIndexDocument(ReverseIndex *index, Document *document) { // tokenizes document body and adds all words to the reverse index???
    char *text = strdup(document->body); // copy the body 
    char *token = strtok(text, " \t\n\r.,;:!?()[]{}<>\""); // first word

    while (token != NULL) {
        normalise_word(token);

        if (token[0] != '\0') {
            DocumentsList *list = malloc(sizeof(DocumentsList)); // create new list
            list->doc = document; // set current document
            list->next = NULL; // end of list
            reverseIndexPut(index, token, list); // add word to index

        }
        token = strtok(NULL, " \t\n\r.,;:!?()[]{}<>\""); // next word
    }

    free(text); // free the copied text

void reverseIndexSaveToFile(ReverseIndex *index, const char *filename) {// save the word index to a text file
    FILE *file = fopen(filename, "w");// open the file
    if (!file) {
        printf("Couldn't open %s for writing\n", filename);
        return;
    }

    for (int i = 0; i < index->slotsCount; i++) {// loop through each slot in the index
        ReverseIndexSlot *slot = index->slots[i];
        if (!slot) continue; // skip empty slots
        ReverseIndexKey *key = slot->keys;     //for each word in this slot
        while (key) { // write the word followed by colon 
            fprintf(file, "%s:", key->word);
            DocumentsList *doc = key->values;   // write all documents that contain this word
            while (doc) {
                if (doc->doc && doc->doc->title) {
                    fprintf(file, "%s,", doc->doc->title);  // write document title followed by comma 
                }
                doc = doc->next;
            }
            fprintf(file, "\n"); // end this word's line
            key = key->next; // go to the next word
        }
    }
    fclose(file); // close the file
}
void reverseIndexLoadFromFile(ReverseIndex *index, const char *filename, 
                            Document* (*getDocByTitle)(const char*)) { // load the word index from a text file
    FILE *file = fopen(filename, "r");  // open the file
    if (!file) {
        printf("Couldn't open %s for reading\n", filename);
        return;
    }
// read file line by line
    char line[1000]; // buffer to hold each line
    while (fgets(line, sizeof(line), file) {
        line[strcspn(line, "\n")] = '\0';    // remove the newline at the end
        char *colon = strchr(line, ':');     // split line into word and documents parts
        if (!colon) continue; // skip bad lines
        *colon = '\0'; // split the string in two
        char *word = line;       // part before colon 
        char *documents = colon + 1; // part after colon
        char *docTitle = strtok(documents, ",");  // split documents by commas
        while (docTitle) { // find the document with this title
            Document *doc = getDocByTitle(docTitle);
            if (doc) {    
                DocumentsList *list = malloc(sizeof(DocumentsList)); // create a new list entry
                list->doc = doc;
                list->next = NULL;
                reverseIndexPut(index, word, list);// add to index
            }
            docTitle = strtok(NULL, ",");  // get next document title
        }
    }

    fclose(file); //close the file
}
}
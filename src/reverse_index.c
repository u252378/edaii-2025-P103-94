#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "reverse_index.h" 
#include "document.h"

ReverseIndex *reverseIndexInit(int slots) { // function to create the reverse index (the hash table)
    ReverseIndex *index = malloc(sizeof(ReverseIndex)); // allocate memory for the index
    index->slotsCount = slots; // save number of slots
    index->unique_keywords = 0; // start with 0 keys
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


void reverseIndexPut(ReverseIndex *index, char *word, DocumentsListNode *node)
 {// function to add a word and the associated document list to the index
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
        DocumentsList *docList = key->values;
if (!docList->head) {
    docList->head = node;
    docList->tail = node;
} else {
    docList->tail->next = node;
    docList->tail = node;
}
docList->number_documents++;

            return;
        }
        key = key->next; // try next key 
    }

    // word not found, create new key entry
    ReverseIndexKey *newKey = malloc(sizeof(ReverseIndexKey)); // new key
    newKey->word = strdup(word); // copy the word
    DocumentsList *newList = malloc(sizeof(DocumentsList));
    newList->head = node;
    newList->tail = node;
     newList->number_documents = 1;
    node->next = NULL;
    newKey->values = newList;
    newKey->next = slotPtr->keys; // insert at beginning of list
    slotPtr->keys = newKey; // update head of key list
    slotPtr->keysCount++; // increase number of keys in this slot
    index->unique_keywords++; // increase total keys in index
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
                    DocumentsListNode *docList = key->values->head;
                    while (docList) {
                        DocumentsListNode *nextDoc = docList->next;
                        if (freeDocs && docList->document) {
                            // free document if requested
                            free(docList->document->title);
                            free(docList->document->body);
                            free(docList->document);
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
            DocumentsListNode *list = malloc(sizeof(DocumentsListNode)); // create new list
            list->document = document; // set current document
            list->next = NULL; // end of list
            reverseIndexPut(index, token, list); // add word to index

        }
        token = strtok(NULL, " \t\n\r.,;:!?()[]{}<>\""); // next word
    }

    free(text); // free the copied text
}

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
            DocumentsListNode *doc = key->values->head;// write all documents that contain this word
            while (doc) {
                if (doc->document && doc->document->title) {
                    fprintf(file, "%s,", doc->document->title);  // write document title followed by comma 
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
    while (fgets(line, sizeof(line), file)) {
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
                DocumentsListNode *node = malloc(sizeof(DocumentsListNode));
                node->next = NULL;  //initializes the next of the node
                node->document = doc;
                reverseIndexPut(index, word, node); // add to index
            
            }
            docTitle = strtok(NULL, ",");  // get next document title
        }
    }
   
    fclose(file); //close the file
}
    ReverseIndex* build_reverse_index(Document* docs) {
    ReverseIndex* index = reverseIndexInit(101); // elige slots adecuados
    while (docs) {
        reverseIndexDocument(index, docs); // indexa cada documento
        docs = docs->next;
    }
    return index;
}

void print_reverse_index(const ReverseIndex* index) {
    reverseIndexSaveToFile((ReverseIndex*)index, "reverse_index.txt");
}

void free_reverse_index(ReverseIndex* index) {
    reverseIndexFree(index, true, true); // o ajusta los flags según lo que necesites
}

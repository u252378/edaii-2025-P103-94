#ifndef DOCUMENT_H
#define DOCUMENT_H
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations
struct Link;
struct Document;

// LINK STRUCTURE
typedef struct Link {
    int id;
    struct Link *next;
} Link;

// DOCUMENT STRUCTURE
typedef struct Document {
    int doc_id;
    char *title;
    char *body;
    Link *links;
    float relevance;
    struct Document *next; // Linked list of documents
} Document;

// DOCUMENT LIST NODE STRUCTURE
typedef struct DocumentsListNode {
    Document *document;
    struct DocumentsListNode *next;
} DocumentsListNode;

// DOCUMENT LIST STRUCTURE
typedef struct DocumentsList {
    int number_documents;
    DocumentsListNode *head;
    DocumentsListNode *tail;
} DocumentsList;

// FUNCTION DECLARATIONS
Document *document_desserialize(FILE *file);
Document *load_documents_from_folder(char *folder_path);
void print_document_details(const Document *doc);
void print_document_compact(const Document *doc);
void free_documents(Document *head);

#endif
// in this file we will create the data structures of lab 1:
#ifndef DOCUMENT_H
#define DOCUMENT_H
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// forward declarations
struct Link;
struct Document;

// LINK STRUCT:
typedef struct Link {
  int id;
  struct Link *next;
} Link;

// DOCUMENT STRUCT:
typedef struct Document {
  int doc_id;
  char *title;
  char *body;
  Link *links;
  float relevance;
  struct Document *next; // for the document linked list
} Document;

typedef struct DocumentsListNode {
  Document *document;
  struct DocumentsListNode *next;
} DocumentsListNode;

typedef struct DocumentsList {
  int number_documents;
  DocumentsListNode *head;
  DocumentsListNode *tail;
} DocumentsList;

// FUNCTION DECLARATIONS:
Document *document_desserialize(FILE *file);
Document *load_documents_from_folder(const char *folder_path);
void print_document_details(const Document *doc);
void print_documents(const Document *head);
void free_document(Document *doc);
void free_documents(Document *head);

#endif
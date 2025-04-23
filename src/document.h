//in this file we will create the data structures of lab 1:
#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

//LINK STRUCT:
typedef struct {
    int id;
    struct Link* next;
} Link;
  
//DOCUMENT STRUCT:
typedef struct {
    int doc_id;
    char* title;
    char* body;
    Link* links;
    float relevance;
    struct Document* next; //for the document linked list
} Document;

//FUNCTION DECLARATIONS:
Document* document_desserialize(char* path);
Document* load_documents_from_folder(const char* folder_path);
void print_documents(const Document* head);
void free_document(Document* doc);
void free_documents(Document* head);

#endif
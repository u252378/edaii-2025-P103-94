// in this file we will create the functions: document_desserialize and
// load_documents_from_folder
#include "document.h"
#include <assert.h>
#include <dirent.h> //to handle directory operations like opendir, readdir, closedir.
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// helper to create a new Link node
Link *create_link(int id) {
  Link *newLink = malloc(sizeof(Link)); // allocates memory for a new Link node.
  if (!newLink)
    return NULL;    // return NULL if memory allocation fails.
  newLink->id = id; // set the link's ID
  newLink->next =
      NULL; // initializes the next pointer to NULL (end of the list).
  return newLink;
}

// FUNCTION PARSE:
// parse a single document file:
Document *document_desserialize(char *path) {
  FILE *file = fopen(path, "r");
  if (!file) return NULL;

  Document *doc = (Document *)malloc(sizeof(Document));
  if (!doc) {
    fclose(file);
    return NULL;
  }

  char buffer[262144];
  int bufferIdx = 0;
  char ch;

  // Leer ID
  if (fgets(buffer, sizeof(buffer), file) == NULL) {
    fclose(file);
    free(doc);
    return NULL;
  }
  doc->doc_id = atoi(buffer);

  // Leer título
  if (fgets(buffer, sizeof(buffer), file) == NULL) {
    fclose(file);
    free(doc);
    return NULL;
  }
  buffer[strcspn(buffer, "\n")] = '\0';
  doc->title = strdup(buffer);

  // Leer cuerpo y extraer links
  bufferIdx = 0;
  char linkBuffer[1000];
  int linkBufferIdx = 0;
  bool parsingLink = false;
  Link *head = NULL, *tail = NULL;

  while ((ch = fgetc(file)) != EOF) {
    if (bufferIdx < (int)(sizeof(buffer) - 1)) buffer[bufferIdx++] = ch;

    if (parsingLink) {
      if (ch == ')') {
        parsingLink = false;
        linkBuffer[linkBufferIdx] = '\0';
        int linkId = atoi(linkBuffer);
        Link *newLink = create_link(linkId);
        if (!newLink) break;
        if (!head) head = tail = newLink;
        else {
          tail->next = newLink;
          tail = newLink;
        }
        linkBufferIdx = 0;
      } else if (ch != '(' && linkBufferIdx < (int)(sizeof(linkBuffer) - 1)) {
        linkBuffer[linkBufferIdx++] = ch;
      }
    } else if (ch == ']') {
      parsingLink = true;
    }
  }

  buffer[bufferIdx] = '\0'; // null-terminate body string

  // Asignar cuerpo y enlaces al documento
  doc->body = strdup(buffer);  // duplica buffer para cuerpo
  doc->links = head;
  doc->relevance = 0.0; // valor por defecto

  fclose(file);
  return doc;
}


  
  
Document *load_documents_from_folder(const char *folder_path) {
  DIR *dir =
      opendir(folder_path); // open the directory specified by folder_path.
  if (!dir)
    return NULL; // check if the directory can be open, if not return NULL.

  struct dirent *entry; // declare a pointer to hold the directory entries
  Document *head = NULL;
  Document *tail = NULL;

  while ((entry = readdir(dir)) != NULL) { // loop through each entry
    char fullpath[1024]; // array to store ful path of the file.
    snprintf(fullpath, sizeof(fullpath), "%s/%s", folder_path,
             entry->d_name); // create full file path.
    struct stat path_stat;
    if (stat(fullpath, &path_stat) == 0 && S_ISREG(path_stat.st_mode)) {
      Document *doc = document_desserialize(fullpath); // desserialize the doc.
      if (doc) {
        if (!head)
          head = tail =
              doc; // if list is empty, set head and tail to the new document.
        else {     // if list is NOT empty, append the document to the list
          tail->next = doc; // set the current tail's next to the new doc.
          tail = doc;       // update the tail to point to the new document.
        }
      }
    }
  }
  closedir(dir);
  return head;
}

// print details of a document:
void print_document_details(const Document *doc) {
  printf("\n=== Document ID %d ===\n", doc->doc_id);
  printf("Title: %s\n", doc->title);
  printf("Body:\n%s\n", doc->body);
  printf("Links:\n");

  const Link *link = doc->links; // pointer to traverse the links.
  while (link) {
    printf("  -> Document ID %d\n", link->id); // print each link's ID.
    link = link->next; // move to the next link in the list.
  }
}

// free all allocated memory for a list of documents:
void free_documents(Document *head) {
  while (head) {
    Document *next = head->next; // save next doc in the list.

    free(head->title);
    free(head->body);

    Link *link = head->links;
    while (link) {
      Link *tmp = link->next; // save next link.
      free(link);
      link = tmp; // move to next link.
    }

    free(head);
    head = next; // move to next doc in the list.
  }
}
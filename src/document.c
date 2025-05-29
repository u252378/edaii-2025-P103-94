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
// En document_desserialize():
Document *document_desserialize(FILE *file) {
  if (!file) return NULL;

  char buffer[4096];

  // Leer línea 1: ID
  if (!fgets(buffer, sizeof(buffer), file)) return NULL;
  int id;
  if (sscanf(buffer, "%d", &id) != 1) return NULL;

  // Leer línea 2: Título
  if (!fgets(buffer, sizeof(buffer), file)) return NULL;
  char *title = strdup(buffer);
  title[strcspn(title, "\n")] = '\0';  // quitar newline

  // Leer el resto: Body
  size_t body_size = 8192;
  char *body = malloc(body_size);
  if (!body) return NULL;
  body[0] = '\0';
  size_t len = 0;
  while (fgets(buffer, sizeof(buffer), file)) {
    size_t line_len = strlen(buffer);
    if (len + line_len + 1 > body_size) {
      body_size *= 2;
      body = realloc(body, body_size);
      if (!body) return NULL;
    }
    strcat(body, buffer);
    len += line_len;
  }

  Document *doc = malloc(sizeof(Document));
  doc->doc_id = id;
  doc->title = title;
  doc->body = body;
  doc->next = NULL;
  doc->links = NULL; // probablemente ya lo inicializas en otra parte

  return doc;
}


  
  
Document *load_documents_from_folder(char *folder_path) {
    DIR *dir = opendir(folder_path);
    if (!dir) return NULL;

    struct dirent *entry;
    Document *head = NULL;

    while ((entry = readdir(dir))) {
        if (entry->d_type == DT_REG) { // Solo archivos normales
            char fullpath[1024];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", folder_path, entry->d_name);

            FILE *file = fopen(fullpath, "r");
            if (!file) {
                perror("Failed to open file");
                continue;
            }

            Document *doc = document_desserialize(file);  // Aquí pasamos FILE *
            fclose(file);  // Cerramos el archivo

            if (doc) {
                doc->next = head;
                head = doc;
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
        Document *next = head->next;
        
        if (head->title) free(head->title);
        if (head->body) free(head->body);
        
        Link *link = head->links;
        while (link) {
            Link *tmp = link->next;
            free(link);
            link = tmp;
        }
        
        free(head);
        head = next;
    }
}
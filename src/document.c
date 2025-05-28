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
Document *document_desserialize(char *path) {
    FILE *file = fopen(path, "r");
    if (!file) return NULL;

    Document *doc = (Document *)malloc(sizeof(Document));
    if (!doc) {
        fclose(file);
        return NULL;
    }

    // Inicializar todos los campos
    doc->doc_id = 0;
    doc->title = NULL;
    doc->body = NULL;
    doc->links = NULL;
    doc->relevance = 0.0;
    doc->next = NULL;

    char buffer[262144];
    
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
    if (!doc->title) {
        fclose(file);
        free(doc);
        return NULL;
    }

    // Leer cuerpo y extraer links
    size_t body_size = 0;
    size_t body_capacity = 1024;
    char *body = malloc(body_capacity);
    if (!body) {
        fclose(file);
        free(doc->title);
        free(doc);
        return NULL;
    }

    Link *head = NULL, *tail = NULL;
    int ch;
    bool in_link = false;
    char link_buffer[100];
    size_t link_pos = 0;

    while ((ch = fgetc(file)) != EOF) {
        // Manejar crecimiento del buffer del cuerpo
        if (body_size >= body_capacity - 1) {
            body_capacity *= 2;
            char *new_body = realloc(body, body_capacity);
            if (!new_body) {
                free(body);
                fclose(file);
                free(doc->title);
                free(doc);
                return NULL;
            }
            body = new_body;
        }

        body[body_size++] = (char)ch;

        // Procesar links
        if (ch == '[') {
            in_link = true;
            link_pos = 0;
        } else if (in_link && ch == '(' && link_pos == 0) {
            // Esperar el ID del link
        } else if (in_link && ch == ')') {
            in_link = false;
            link_buffer[link_pos] = '\0';
            int link_id = atoi(link_buffer);
            Link *new_link = create_link(link_id);
            if (!new_link) continue;
            
            if (!head) head = tail = new_link;
            else {
                tail->next = new_link;
                tail = new_link;
            }
        } else if (in_link) {
            if (link_pos < sizeof(link_buffer) - 1) {
                link_buffer[link_pos++] = (char)ch;
            }
        }
    }

    body[body_size] = '\0';
    doc->body = body;
    doc->links = head;

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
#include "document.h"
#include <assert.h> //provides macros for adding diagnostics (used during debugging).
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
    return NULL; // return NULL if memory allocation fails.
  newLink->id = id; // set the link's ID
  newLink->next =
      NULL; // initializes the next pointer to NULL (end of the list).
  return newLink;
}

// FUNCTION PARSE:
// parse a single document file:
Document *document_desserialize(FILE *file) {
  if (!file) return NULL; //return null if the pointer is invalid

  char buffer[4096];

  //read line 1: ID
  if (!fgets(buffer, sizeof(buffer), file)) return NULL; 
  int id;
  if (sscanf(buffer, "%d", &id) != 1) return NULL; //extract int ID from the line 

  //read line 2: doc title 
  if (!fgets(buffer, sizeof(buffer), file)) return NULL; 
  char *title = strdup(buffer); //duplicate the title string to allocate memory
  title[strcspn(title, "\n")] = '\0';  //remove newline char at the end

  //reads the rest: body 
  size_t body_size = 8192; //set initial buffer size for the body 
  char *body = malloc(body_size);
  if (!body) return NULL;
  body[0] = '\0';
  size_t len = 0; //initialize body with empty str

  while (fgets(buffer, sizeof(buffer), file)) { //read line by line until EOF 
    size_t line_len = strlen(buffer);
    if (len + line_len + 1 > body_size) {
      body_size *= 2;
      body = realloc(body, body_size);
      if (!body) return NULL;
    }
    strcat(body, buffer); //append line to body
    len += line_len; //update total length
  }

  //create and initialize the doc structure: 
  Document *doc = malloc(sizeof(Document));
  doc->doc_id = id;
  doc->title = title;
  doc->body = body;
  doc->next = NULL;
  doc->links = NULL;

  return doc; //return the new doc
}

Document *load_documents_from_folder(char *folder_path) { //function to load all docs from the folder
    DIR *dir = opendir(folder_path); //open the director specified
    if (!dir) return NULL; //opening failed

    struct dirent *entry;
    Document *head = NULL;

    while ((entry = readdir(dir))) { //read each file in the folder
        if (entry->d_type == DT_REG) { //only regular files (IMPORTANT)
            char fullpath[1024];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", folder_path, entry->d_name); //build full path

            FILE *file = fopen(fullpath, "r");
            if (!file) {
                perror("Failed to open file");
                continue; //skip to next file 
            }

            Document *doc = document_desserialize(file); //parse the doc file
            fclose(file); //close file after reading

            if (doc) {
                doc->next = head; //insert the document at the beginning of the list
                head = doc; //update head pointer  
            }
        }
    }

    closedir(dir); //close directory 
    return head; //return linked list of docs
}

//print details of a document:
void print_document_details(const Document *doc) {
  printf("\n=== Document ID %d ===\n", doc->doc_id);
  printf("Title: %s\n", doc->title);
  printf("Body:\n%s\n", doc->body);
  printf("Links:\n");

  const Link *link = doc->links; //pointer to traverse the links.
  while (link) {
    printf("  -> Document ID %d\n", link->id); //print each link's ID.
    link = link->next; //move to the next link in the list.
  }
}

// Print document in compact format with all metadata
void print_document_compact(const Document *doc) {
    if (!doc) return; //safety check
    
    // Print ID section with proper formatting
    printf("ID\n%d\n", doc->doc_id);
    
    // Print TITLE section with proper formatting
    printf("TITLE\n%s\n", doc->title);
    
    // Print RELEVANCE SCORE section with proper formatting
    printf("RELEVANCE SCORE\n%.0f\n", doc->relevance);
    
    // Print BODY section header and content
    printf("BODY\n");
    if (doc->body) {
        const char *body = doc->body;
        while (*body) {
            putchar(*body); //print each character preserving original formatting
            body++;
        }
    }
    
    // Print links if they exist in [link](id) format
    if (doc->links) {
        const Link *link = doc->links;
        while (link) {
            printf("[link](%d)\n", link->id);
            link = link->next;
        }
    }
    
    // Print the footer line
    printf("-----------------------------\n");
}

//free all allocated memory for a list of documents:
void free_documents(Document *head) {
    while (head) {
        Document *next = head->next;
        
        // Free all document components
        if (head->title) free(head->title);
        if (head->body) free(head->body);
        
        // Free links
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
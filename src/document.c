#include "document.h"
#include <assert.h>
#include <dirent.h> // For handling directories
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

//helper function to create a new Link node:
Link *create_link(int id) {
    Link *newLink = malloc(sizeof(Link));
    if (!newLink) return NULL; // Return NULL if memory allocation fails

    newLink->id = id;
    newLink->next = NULL; // Initialize the next pointer to NULL
    return newLink;
}

//deserialize a single document file
Document *document_desserialize(FILE *file) {
    if (!file) return NULL;

    char buffer[4096];

    // Read line 1: ID
    if (!fgets(buffer, sizeof(buffer), file)) return NULL;
    int id;
    if (sscanf(buffer, "%d", &id) != 1) return NULL; // Extract int ID from the line

    // Read line 2: Document title
    if (!fgets(buffer, sizeof(buffer), file)) return NULL;
    char *title = strdup(buffer); // Duplicate the title string to allocate memory
    if (!title) return NULL; // Check memory allocation
    title[strcspn(title, "\n")] = '\0'; // Remove newline character

    // Read the rest: Document body
    size_t body_size = 8192;
    char *body = malloc(body_size);
    if (!body) {
        free(title);
        return NULL;
    }
    body[0] = '\0';
    size_t len = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        size_t line_len = strlen(buffer);
        if (len + line_len + 1 > body_size) {
            body_size *= 2;
            char *new_body = realloc(body, body_size);
            if (!new_body) {
                free(body);
                free(title);
                return NULL;
            }
            body = new_body;
        }
        strcat(body, buffer);
        len += line_len;
    }

    // Create and initialize the Document structure
    Document *doc = malloc(sizeof(Document));
    if (!doc) {
        free(title);
        free(body);
        return NULL;
    }
    doc->doc_id = id;
    doc->title = title;
    doc->body = body;
    doc->next = NULL;
    doc->links = NULL;

    return doc;
}

//load all documents from the specified folder
Document *load_documents_from_folder(char *folder_path) {
    DIR *dir = opendir(folder_path); //open the directory
    if (!dir) return NULL;

    struct dirent *entry;
    Document *head = NULL;

    while ((entry = readdir(dir))) { //iterate over each entry 
        if (entry->d_type == DT_REG) { //only process regular files (we had to look it up)
            char fullpath[1024];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", folder_path, entry->d_name);

            FILE *file = fopen(fullpath, "r");
            if (!file) {
                perror("Failed to open file");
                continue;
            }

            Document *doc = document_desserialize(file);
            fclose(file);

            if (doc) {
                doc->next = head;
                head = doc;
            }
        }
    }

    closedir(dir);
    return head;
}

//print details of a document 
void print_document_details(const Document *doc) {
    if (!doc) return;

    printf("\n=== Document ID %d ===\n", doc->doc_id);
    printf("Title: %s\n", doc->title);
    printf("Body:\n%s\n", doc->body);
    printf("Links:\n");

    const Link *link = doc->links;
    while (link) {
        printf("  -> Document ID %d\n", link->id);
        link = link->next;
    }
}

//print document in compact format
void print_document_compact(const Document *doc) {
    if (!doc) return;

    printf("ID\n%d\n", doc->doc_id);
    printf("TITLE\n%s\n", doc->title);
    printf("RELEVANCE SCORE\n%.2f\n", doc->relevance);
    printf("BODY\n");

    if (doc->body) {
        const char *body = doc->body;
        while (*body) {
            putchar(*body);
            body++;
        }
    }

    if (doc->links) {
        const Link *link = doc->links;
        while (link) {
            printf("[link](%d)\n", link->id);
            link = link->next;
        }
    }

    printf("-----------------------------\n");
}

//free all allocated memory for a list of documents
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
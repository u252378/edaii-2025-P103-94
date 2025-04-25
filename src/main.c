#include "sample_lib.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

void createaleak() {
  char *foo = malloc(20 * sizeof(char));
  printf("Allocated leaking string: %s", foo);
}

//Create a new link
Link* create_link(int document_id) {
  Link* new_link = (Link*)malloc(sizeof(Link));
  if (new_link) {
      new_link->document_id = document_id;
      new_link->next = NULL;
  }
  return new_link;
}

//Add a link to a document's link list
void add_link_to_document(Document* doc, int document_id) {
  Link* new_link = create_link(document_id);
  if (doc->links == NULL) {
      doc->links = new_link;
  } else {
      Link* current = doc->links;
      while (current->next != NULL) {
          current = current->next;
      }
      current->next = new_link;
  }
}

//Free all links in a document
void free_links(Link* head) {
  while (head != NULL) {
      Link* temp = head;
      head = head->next;
      free(temp);
  }
}

// Parse a document from a file
Document* document_deserialize(char* path) {
  FILE* file = fopen(path, "r");
  if (!file) {
      perror("Failed to open file");
      return NULL;
  }

  Document* doc = (Document*)malloc(sizeof(Document));
  if (!doc) {
      fclose(file);
      return NULL;
  }

  // Initialize defaults
  doc->id = 0;
  doc->title = NULL;
  doc->body = NULL;
  doc->links = NULL;
  doc->relevance = 0.0;
  doc->next = NULL;

  // Read document ID (first line)
  if (fscanf(file, "%d\n", &doc->id) != 1) {
      free(doc);
      fclose(file);
      return NULL;
  }

  // Read title (second line)
  char* title = NULL;
  size_t title_len = 0;
  if (getline(&title, &title_len, file) == -1) {
      free(doc);
      fclose(file);
      return NULL;
  }
  // Remove newline if present
  title[strcspn(title, "\n")] = '\0';
  doc->title = title;

  // Read body (rest of file)
  char* body = NULL;
  size_t body_len = 0;
  ssize_t read;
  char* line = NULL;
  size_t line_len = 0;

  while ((read = getline(&line, &line_len, file)) != -1) {
      if (body == NULL) {
          body = strdup(line);
          body_len = strlen(line);
      } else {
          body = realloc(body, body_len + strlen(line) + 1);
          strcat(body, line);
          body_len += strlen(line);
      }
  }
  free(line);

  // Process body to extract links and store plain text
  if (body) {
      process_body_for_links(doc, body);
      // Store the processed body (without link markup)
      doc->body = process_body_text(body);
      free(body);
  }

  fclose(file);
  return doc;
}

// Helper function to process body and extract links
void process_body_for_links(Document* doc, char* body) {
  char* ptr = body;
  while ((ptr = strstr(ptr, "[")) != NULL) {
      char* link_end = strstr(ptr, "]");
      if (!link_end) break;
      
      char* dest_start = strstr(link_end, "(");
      char* dest_end = strstr(dest_start, ")");
      if (!dest_start || !dest_end) break;
      
      // Extract destination ID
      char id_str[32];
      int len = dest_end - (dest_start + 1);
      strncpy(id_str, dest_start + 1, len);
      id_str[len] = '\0';
      int dest_id = atoi(id_str);
      
      // Add link to document
      add_link_to_document(doc, dest_id);
      
      ptr = dest_end + 1;
  }
}

// Helper function to process body text (remove link markup)
char* process_body_text(const char* body) {
  // Implementation would remove [link text](123) markup
  // and replace with just "link text" or similar
  // This is a simplified version
  return strdup(body);
}

// Add a document to the global list
Document* documents_head = NULL;

void add_document_to_list(Document* doc) {
    if (documents_head == NULL) {
        documents_head = doc;
    } else {
        Document* current = documents_head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = doc;
    }
}

// Load all documents from a dataset directory
void load_documents_from_directory(const char* dir_path) {
    DIR* dir = opendir(dir_path);
    if (!dir) {
        perror("Failed to open directory");
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {  // Regular file
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);
            
            Document* doc = document_deserialize(path);
            if (doc) {
                add_document_to_list(doc);
            }
        }
    }
    closedir(dir);
}

// Free all documents in the list
void free_document_list() {
    Document* current = documents_head;
    while (current != NULL) {
        Document* next = current->next;
        free_links(current->links);
        free(current->title);
        free(current->body);
        free(current);
        current = next;
    }
    documents_head = NULL;
}

// Print document list with indices
void print_document_list() {
  printf("\nAvailable Documents:\n");
  printf("====================\n");
  
  Document* current = documents_head;
  int index = 0;
  while (current != NULL) {
      printf("%d. [ID: %d] %s\n", index, current->id, current->title);
      current = current->next;
      index++;
  }
  printf("\n");
}

// Print a specific document
void print_document(int index) {
  Document* doc = documents_head;
  int current_index = 0;
  
  while (doc != NULL && current_index < index) {
      doc = doc->next;
      current_index++;
  }
  
  if (doc == NULL) {
      printf("Invalid document index!\n");
      return;
  }
  
  printf("\nDocument ID: %d\n", doc->id);
  printf("Title: %s\n", doc->title);
  printf("\nContent:\n%s\n", doc->body);
}

// Main CLI loop
void run_cli_interface() {
  int choice = -1;
  
  while (1) {
      print_document_list();
      printf("Enter document number to view, or -1 to quit: ");
      scanf("%d", &choice);
      
      if (choice == -1) {
          break;
      }
      
      print_document(choice);
      printf("\nPress enter to continue...");
      getchar();  // Consume newline
      getchar();  // Wait for enter
  }
}

int main(int argc, char** argv) {
  printf("*****************\nWelcome to EDA 2!\n*****************\n");

  // how to import and call a function
  printf("Factorial of 4 is %d\n", fact(4));

  // uncomment and run "make v" to see how valgrind detects memory leaks
  // createaleak();
  if (argc < 2) {
      printf("Usage: %s <dataset_directory>\n", argv[0]);
      return 1;
  }

  // Load documents from the specified directory
  load_documents_from_directory(argv[1]);

  // Run the CLI interface
  run_cli_interface();

  // Clean up
  free_document_list();

  return 0;
}
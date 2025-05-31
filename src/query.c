#include "query.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Creates a new empty query list */
QueryList *create_query_list(void) {
  QueryList *list = malloc(sizeof(QueryList));
  if (!list) return NULL;
  list->head = NULL;
  return list;
}

/* Frees all memory associated with a query list */
void free_query_list(QueryList *list) {
  QueryNode *current = list->head;
  while (current) {
    QueryNode *next = current->next;
    free(current->keyword);
    free(current);
    current = next;
  }
  free(list);
}

/* Adds a new keyword to the query list */
void add_keyword(QueryList *query_list, const char *keyword, QueryType type) {
  QueryNode *new_node = malloc(sizeof(QueryNode));
  if (!new_node) return;

  new_node->keyword = strdup(keyword);
  new_node->type = type;
  new_node->next = NULL;

  if (!query_list->head) {
    query_list->head = new_node;
  } else {
    QueryNode *current = query_list->head;
    while (current->next) current = current->next;
    current->next = new_node;
  }
}

/* Parses a query string into a QueryList structure */
QueryList *parse_query(const char *input) {
  QueryList *list = create_query_list();
  if (!list) return NULL;

  char buffer[256];
  int idx = 0;

  for (int i = 0;; i++) {
    char ch = input[i];
    if (ch == ' ' || ch == '\0') {
      if (idx > 0) {
        buffer[idx] = '\0';
        QueryType type = INCLUDE;
        
        if (buffer[0] == '!') {
          type = OR;
          memmove(buffer, buffer + 1, strlen(buffer));
        } else if (buffer[0] == '-') {
          type = EXCLUDE;
          memmove(buffer, buffer + 1, strlen(buffer));
        }
        
        add_keyword(list, buffer, type);
        idx = 0;
      }
      if (ch == '\0') break;
    } else {
      buffer[idx++] = ch;
    }
  }
  return list;
}

/* Prints the query list for debugging purposes */
void print_query_list(const QueryList *list) {
  QueryNode *current = list->head;
  while (current) {
    printf("[%s | %d] -> ", current->keyword, current->type);
    current = current->next;
  }
  printf("NULL\n");
}

/* Initializes a new query queue */
void init_queue_query(QueueQueries *queue) {
  queue->start = 0;
  queue->size = 0;
  for (int i = 0; i < 3; i++) {
    queue->queries[i] = NULL;
  }
}

/* Adds a query to the recent queries queue */
void enqueue_query(QueueQueries *queue, const char *keyword) {
  if (queue->size == 3) {
    free(queue->queries[queue->start]);
    queue->start = (queue->start + 1) % 3;
  } else {
    queue->size++;
  }
  
  int pos = (queue->start + queue->size - 1) % 3;
  queue->queries[pos] = strdup(keyword);
}

/* Frees all memory used by the query queue */
void free_queue_queries(QueueQueries *queue) {
  for (int i = 0; i < queue->size; i++) {
    free(queue->queries[(queue->start + i) % 3]);
  }
  queue->size = 0;
}

/* Frees a documents list and all its nodes */
void free_documents_list(DocumentsList *list) {
    if (!list) return;
    
    DocumentsListNode *current = list->head;
    while (current) {
        DocumentsListNode *next = current->next;
        free(current);
        current = next;
    }
    free(list);
}

/* Removes duplicate documents from a results list */
void remove_duplicate_results(DocumentsList *results) {
    if (!results || !results->head) return;
    
    DocumentsListNode *current = results->head;
    while (current) {
        DocumentsListNode *runner = current;
        while (runner->next) {
            if (runner->next->document->doc_id == current->document->doc_id) {
                DocumentsListNode *temp = runner->next;
                runner->next = runner->next->next;
                if (temp == results->tail) {
                    results->tail = runner;
                }
                free(temp);
            } else {
                runner = runner->next;
            }
        }
        current = current->next;
    }
}

/* Returns the intersection of two document lists */
DocumentsList *intersect_documents_lists(DocumentsList *list1, DocumentsList *list2) {
    if (!list1 || !list2) return NULL;
    
    DocumentsList *result = malloc(sizeof(DocumentsList));
    if (!result) return NULL;
    
    result->head = NULL;
    result->tail = NULL;
    result->number_documents = 0;

    DocumentsListNode *node1 = list1->head;
    while (node1) {
        DocumentsListNode *node2 = list2->head;
        while (node2) {
            if (node1->document->doc_id == node2->document->doc_id) {
                DocumentsListNode *new_node = malloc(sizeof(DocumentsListNode));
                if (!new_node) {
                    free_documents_list(result);
                    return NULL;
                }
                new_node->document = node1->document;
                new_node->next = NULL;
                
                if (result->tail) {
                    result->tail->next = new_node;
                } else {
                    result->head = new_node;
                }
                result->tail = new_node;
                result->number_documents++;
                break;
            }
            node2 = node2->next;
        }
        node1 = node1->next;
    }
    return result;
}
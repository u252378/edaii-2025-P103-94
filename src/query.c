#include "query.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Existing Query List Functions (unchanged) */

QueryList *create_query_list(void) {
  QueryList *list = malloc(sizeof(QueryList));
  if (!list) return NULL;
  list->head = NULL;
  return list;
}

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

void print_query_list(const QueryList *list) {
  QueryNode *current = list->head;
  while (current) {
    printf("[%s | %d] -> ", current->keyword, current->type);
    current = current->next;
  }
  printf("NULL\n");
}

/* New Queue Functions (only these were added) */

void init_queue_query(QueueQueries *queue) {
  queue->start = 0;
  queue->size = 0;
  for (int i = 0; i < 3; i++) {
    queue->queries[i] = NULL;
  }
}

void enqueue_query(QueueQueries *queue, const char *keyword) {
  // Remove oldest if queue is full
  if (queue->size == 3) {
    free(queue->queries[queue->start]);
    queue->start = (queue->start + 1) % 3;
  } else {
    queue->size++;
  }
  
  // Add new query at next available position
  int pos = (queue->start + queue->size - 1) % 3;
  queue->queries[pos] = strdup(keyword);
}

void free_queue_queries(QueueQueries *queue) {
  for (int i = 0; i < queue->size; i++) {
    free(queue->queries[(queue->start + i) % 3]);
  }
  queue->size = 0;
}
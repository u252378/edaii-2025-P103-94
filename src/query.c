#include "query.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

QueryList *create_query_list() {
  QueryList *query_list = malloc(sizeof(QueryList));
  if (!query_list) {
    perror("Memory allocation failed");
    return NULL;
  }
  query_list->head = NULL;
  return query_list;
}

void free_query_list(QueryList *query_list) {
  QueryNode *current = query_list->head;
  QueryNode *next;

  while (current != NULL) {
    next = current->next;
    free(current->keyword);
    free(current);
    current = next;
  }

  free(query_list);
}

void add_keyword(QueryList *query_list, const char *keyword, QueryType type) {
  QueryNode *new_node = malloc(sizeof(QueryNode));
  if (!new_node) {
    perror("Memory allocation failed");
    return;
  }

  new_node->keyword = malloc(strlen(keyword) + 1);
  if (!new_node->keyword) {
    perror("Memory allocation failed");
    free(new_node);
    return;
  }

  strcpy(new_node->keyword, keyword);
  new_node->type = type;
  new_node->next = NULL;

  if (!query_list->head) {
    query_list->head = new_node;
  } else {
    QueryNode *current = query_list->head;
    while (current->next != NULL) {
      current = current->next;
    }
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
#ifndef QUERY_H
#define QUERY_H

typedef enum { INCLUDE, EXCLUDE, OR } QueryType;

// Node in the query linked list
typedef struct QueryNode {
  char *keyword;
  QueryType type;
  struct QueryNode *next;
} QueryNode;

// Query linked list structure
typedef struct {
  QueryNode *head;
} QueryList;

// Query list functions:
QueryList *create_query_list(void);
void free_query_list(QueryList *list);
void add_keyword(QueryList *query_list, const char *keyword, QueryType type);
QueryList *parse_query(const char *input);
void print_query_list(const QueryList *list);  // For debugging

#endif
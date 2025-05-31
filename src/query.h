#ifndef QUERY_H
#define QUERY_H

#include "document.h" // This now contains the DocumentsList definition

typedef enum { INCLUDE, EXCLUDE, OR } QueryType;

// Node in the query linked list
typedef struct QueryNode {
    char *keyword;
    QueryType type;
    struct QueryNode *next;
} QueryNode;

// Query list structure
typedef struct {
    QueryNode *head;
} QueryList;

// Recent queries queue (circular buffer)
typedef struct {
    char *queries[3];  // Stores up to 3 recent queries
    int start;         // Index of oldest query
    int size;          // Current number of queries (0-3)
} QueueQueries;

// Query list functions
QueryList *create_query_list(void);
void free_query_list(QueryList *list);
void add_keyword(QueryList *query_list, const char *keyword, QueryType type);
QueryList *parse_query(const char *input);
void print_query_list(const QueryList *list);

// Queue functions (for recent searches)
void init_queue_query(QueueQueries *queue);
void enqueue_query(QueueQueries *queue, const char *keyword);
void free_queue_queries(QueueQueries *queue);

// Document list operations
void free_documents_list(DocumentsList *list);
DocumentsList *intersect_documents_lists(DocumentsList *list1, DocumentsList *list2);
void remove_duplicate_results(DocumentsList *results);

#endif
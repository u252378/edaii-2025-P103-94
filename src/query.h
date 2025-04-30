#ifndef QUERY_H
#define QUERY_H


// This is a data structure for a node in the query linked list
typedef struct {
    char *keyword;
    struct QueryNode *next;
} QueryNode;

// This is a data structure for the query linked list, containing pointer to head
typedef struct {
    QueryNode *head;
} QueryList;



void add_keyword(QueryList *query_list, const char *keyword);







typedef enum { 
    INCLUDE, EXCLUDE, OR 
} QueryType;

typedef struct Query {
    char* word;
    QueryType type;
    struct Query* next;
} Query;

Query* parse_query(const char* input);
void free_query(Query* head);
void print_query(const Query* head); //used for debugging.
#endif
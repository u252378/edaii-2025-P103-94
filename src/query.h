#ifndef QUERY_H
#define QUERY_H

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
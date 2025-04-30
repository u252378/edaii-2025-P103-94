#ifndef QUERY_H
#define QUERY_H
#define QUEUE_SIZE 3


// This is a data structure for a node in the query linked list
typedef struct {
    char *keyword;
    struct QueryNode *next;
} QueryNode;

// This is a data structure for the query linked list, containing pointer to head
typedef struct {
    QueryNode *head;
} QueryList;

// This is a data structure for a circular queue that will store a maximum of 3 queries
typedef struct {
    char* elements[QUEUE_SIZE];
    int front;
    int rear;
    int amount_current_elements;
} QueueQueries;


void add_keyword(QueryList *query_list, const char *keyword);



void init_queue_query(QueueQueries *queue);
void enqueue_queries(QueueQueries *queue, const char *keyword);
void print_last_queries(QueueQueries *queue);
void free_queue_queries(QueueQueries *queue);







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
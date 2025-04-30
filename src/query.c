//initialize query from string
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "query.h"


// This function is to create an empty query linked list
QueryList* create_query_list() {

  // allocate size for the query list
  QueryList *query_list = malloc(sizeof(QueryList));

  // handle that memory allocation failed
  if (!query_list) {
    perror("Memory allocation failed");
    return NULL;
  }

  // initialise the pointers of the query linked list to NULL
  query_list -> head = NULL;

  return query_list;
}





// This function is to free the query list
void free_query_list(QueryList *query_list) {
  // pointer to head of query list
  QueryNode *current = query_list -> head;

  // pointer that points to data type of query node
  QueryNode *next;

  // loop that will iterate through all list
  while (current!= NULL) {

    // pointer next, then points to next element of the current element, therefore there is no link anymore
    next = current -> next;
    
    // freeing the current keyword
    free(current -> keyword);
    // freeing the current node
    free(current);

    // now current node is the node next to the current one
    current = next;
  }

  // when freeing all the nodes in the list, then free the list itself
  free(query_list);
}





// This function is to add another element inserted by the user to the linked list
void add_keyword(QueryList *query_list, const char *keyword) {

  // allocate memory for the new query node
  QueryNode *new_node = malloc(sizeof(QueryNode));

  // handle the case for memory allocation error
  if(!new_node) {
    perror("Memory allocation failed");
    return;
  }

  // allocate memory for keyword string
  new_node -> keyword = malloc(strlen(keyword) + 1);
  
  // handle case for memory allocation error
  if (!new_node -> keyword) {
    perror("Memory allocation failed");

    // freeing the allocate node to avoid memory that will not be used
    free(new_node);

    return;
  }

  // when allocated memory for keyword, then copy keyword string into allocated memory
  strcpy(new_node -> keyword, keyword);

  // pointer of new node set to null since it is last element in list
  new_node -> next = NULL;

  if(!query_list -> head) {
    query_list -> head = new_node;
  } else {
    // create a pointer to point to elements in the list
    QueryNode *current = query_list -> head;

    // This will go through the list until the last element
    while(current->next != NULL) {
      current = current->next;
    }
    // When at the ned of the list, the last element next pointer, will point to the element to append
    current->next = new_node;
  }
}










Query* create_query_node(const char* word, QueryType type) {
    Query* q = malloc(sizeof(Query));
    q->word = strdup(word);
    q->type = type;
    q->next = NULL;
    return q;
}

Query* parse_query(const char* input) {
    Query *head = NULL, *tail = NULL;
    char buffer[256];
    int idx = 0;

    for (int i = 0; ; i++) {
        char ch = input[i];

        if (ch == ' ' || ch == '\0') {
            if (idx > 0) {
                buffer[idx] = '\0';

                QueryType type = INCLUDE;
                if (buffer[0] == '!') {
                    type = EXCLUDE;
                    memmove(buffer, buffer + 1, strlen(buffer)); // Remove '!'
                } else if (strchr(buffer, '|')) {
                    type = OR;
                }

                Query* node = create_query_node(buffer, type);
                if (!head) head = tail = node;
                else {
                    tail->next = node;
                    tail = node;
                }
                idx = 0;
            }

            if (ch == '\0') break;
        } else {
            buffer[idx++] = ch;
        }
    }

    return head;
}

void free_query(Query* head) {
    while (head) {
        Query* next = head->next;
        free(head->word);
        free(head);
        head = next;
    }
}

void print_query(const Query* head) {
    while (head) {
        printf("[%s | %d] -> ", head->word, head->type);
        head = head->next;
    }
    printf("NULL\n");
}




// This is a function to initilaise the queue of queries
void init_queue_query(QueueQueries *queue) {
  queue -> front = 0;
  queue -> rear = -1;
  queue -> amount_current_elements = 0;

  for (int i = 0; i < QUEUE_SIZE; i++) {
    queue -> elements[i] = NULL;
  }
}

// This is a function to enqueue the queries, overwriting the oldest query
void enqueue_queries(QueueQueries *queue, const char *keyword) {
  
  // move the rear pointer in a ciruclar way, so that it does not exceeds the max size of queue
  queue -> rear = (queue -> rear + 1) % QUEUE_SIZE;

  // move front pointer if queue is full, because oldest query is at front position
  if (queue -> amount_current_elements == QUEUE_SIZE) {

    // freeing memory of keyword that is going to be overwritten, it is front because it is queue (FIFO)
    free(queue -> elements[queue -> front]);

    // seting freed space to NULL
    queue -> elements[queue -> front] = NULL;

    queue -> front = (queue -> front + 1) % QUEUE_SIZE;
  } else { // for the case query is not full, the number of elements in queue is increased
    queue -> amount_current_elements++;
  }

  // allocate space for keyword string
  char* copy = malloc(strlen(keyword) + 1);

  // handles memory allocation error
  if (!copy) {
    perror("Memory allocation failed");
    return;
  }

  // copy keyword into allocated memory
  strcpy(copy, keyword);

  // add keyword string to queue
  queue -> elements[queue -> rear] = copy;
}

// This is a function to print the queue that only contains the 3 last queries
void print_last_queries(QueueQueries *queue) {
  printf("The last 3 queries are:\n");

  for(int i = 0; i < queue -> amount_current_elements; i++) {

    // calculate index using front pointer and i, considering it is a ciruclar queue
    int index = (queue -> front + i) % QUEUE_SIZE;

    printf("%s\n", queue -> elements[index]);
  }
}

// This is a function to free the queue of queries
void free_queue_queries(QueueQueries *queue) {
  for (int i = 0; i < queue -> amount_current_elements; i++) {

    // calculate index using front pointer and i, considering it is a circular queue
    int index = (queue -> front + i) % QUEUE_SIZE;

    free(queue -> elements[index]);
  }
}
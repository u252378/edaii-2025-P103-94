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





    // free(new_node);



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
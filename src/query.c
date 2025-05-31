#include "query.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// function to create a new empty query list
QueryList *create_query_list(void) {
   
    // allocating space for list
    QueryList *list = malloc(sizeof(QueryList));
    if (!list) return NULL; // handling case where list is not correctly allocated
    
    // setting head pointer of list to NULL
    list->head = NULL;

    // then return the list
    return list;
}

// function to free all memory associated with the query list
void free_query_list(QueryList *list) {
    if (!list) return; // checking if there is a valid list passed as an argument to the function

    // pointer to go through the list
    QueryNode *current = list->head;

    /*iterate thourgh the list until the last node,
    and freeing all of its contents while going through the list*/
    while (current) {
        QueryNode *next = current->next;
        free(current->keyword);
        free(current);
        current = next;
    }
    // finally free the list itself
    free(list);
}

// function to add a new keyword to the query list
void add_keyword(QueryList *query_list, const char *keyword, QueryType type) {
    // checking if there are valids list and keyword passed to the function as argument
    if (!query_list || !keyword) return;

    // allocating memory for a new node
    QueryNode *new_node = malloc(sizeof(QueryNode));
    if (!new_node) return; // checking memory was correctly allocated

    // set the node keyword to the keyword passed as argument to the function
    new_node->keyword = strdup(keyword);
    if (!new_node->keyword) { // checking it was correctly set, and if not free the new node allocated and return
        free(new_node);
        return;
    }

    // setting type of new node to the type passed as argument and next pointer of node to NULL
    new_node->type = type;
    new_node->next = NULL;

    // checking if there is a head, and if it is not the case then setting the head to be the new node
    if (!query_list->head) {
        query_list->head = new_node;
    } else { // case where there is already a head in the list, so new node is put at the end of list 
        QueryNode *current = query_list->head;
        while (current->next) current = current->next;
        current->next = new_node;
    }
}

/* Parses a query string into a QueryList structure */
QueryList *parse_query(const char *input) {
    QueryList *list = create_query_list(); // creating empty query list
    if (!list) return NULL; // handling case query list was not correctly created

    char buffer[256] = {0}; // buffer to hold individual keywords temporarily
    size_t idx = 0;    // the index to track current position in buffer
    int keyword_found = 0;  // to track if at least one keyword is added

    // iterate through each character in the string
    for (size_t i = 0;; i++) {
        char ch = input[i];
        if (ch == ' ' || ch == '\0') { // when encountering a space or null terminator, then process keyword
            if (idx > 0) { // checking buffer has characters
                buffer[idx] = '\0'; // add null terminator to the buffer
                QueryType type = INCLUDE; // the default type is INCLUDED

                // To detect if keyword starts with special characters
                if (buffer[0] == '!') {
                    type = OR; // then set type to OR
                    memmove(buffer, buffer + 1, strlen(buffer)); // remove leading character
                } else if (buffer[0] == '-') {
                    type = EXCLUDE; // then set type to EXCLUDE
                    memmove(buffer, buffer + 1, strlen(buffer)); // remove leading character
                }

                // add the keyword to the query list
                add_keyword(list, buffer, type);
                keyword_found = 1; // to know that keyword was added to list
                idx = 0;  // reset buffer index to 0
            }

            if (ch == '\0') break; // to exit the loop at the end of the input string
        } else {
            if (idx < sizeof(buffer) - 1) { // if space then add characters to buffer
                buffer[idx++] = ch;
            } else {
                fprintf(stderr, "Query word too long, truncating.\n");
                break;
            }
        }
    }

    // if no keywords were added, free the list and return NULL
    if (!keyword_found) {
        free_query_list(list);
        return NULL;
    }
    // return the query linked list
    return list;
}

/* function to prints the query list for debugging purposes */
void print_query_list(const QueryList *list) {
    if (!list) return; // checkin if a valid list is passed as argument

    // set a pointer to go through the list
    QueryNode *current = list->head;
    while (current) { // each node's keyword and type is printed
        printf("[%s | %d] -> ", current->keyword, current->type);
        current = current->next; // pointer is moved to next node
    }
    printf("NULL\n");
}

/* function to initialise a new query queue */
void init_queue_query(QueueQueries *queue) {
    queue->start = 0; // setting start of queue initially to 0
    queue->size = 0; // setting size of queue initially to 0
    for (int i = 0; i < 3; i++) { // initialising each query to NULL
        queue->queries[i] = NULL;
    }
}

/* function to add a query to the recent queries queue */
void enqueue_query(QueueQueries *queue, const char *keyword) {
    if (queue->size == 3) { // checking size of queue is of 3, so need to free space for new query
        free(queue->queries[queue->start]); // free the current start query in the queue
        queue->start = (queue->start + 1) % 3; // move start index in a circular way
    } else {
        queue->size++; // increaing size of queue if it is not full
    }
    // calculate position to insert new query at end
    int pos = (queue->start + queue->size - 1) % 3;
    queue->queries[pos] = strdup(keyword);
}

/* function to free all memory used by the query queue */
void free_queue_queries(QueueQueries *queue) {
    for (int i = 0; i < queue->size; i++) { // iterate through all stored queries and free each
        free(queue->queries[(queue->start + i) % 3]);
    }
    queue->size = 0; // set queue size to 0 after freeing
}

/* function to free a documents list and all of its nodes */
void free_documents_list(DocumentsList *list) {
    if (!list) return; // checking if valid list is passed as argument

    // pointer to know current node in list
    DocumentsListNode *current = list->head;
    while (current) { // iterate thorugh list and free each node
        DocumentsListNode *next = current->next;
        free(current);
        current = next;
    }
    free(list); // then free the list itself
}

/* Removes duplicate documents from a results list */
void remove_duplicate_results(DocumentsList *results) {
    // checking if valid list is passed as argument
    if (!results || !results->head) return;

    // pointer to go through the list
    DocumentsListNode *current = results->head;
    while (current) { // iterate until the last node
        DocumentsListNode *runner = current; // pointer to check next nodes for looking for duplicates
        while (runner->next) {
            if (runner->next->document->doc_id == current->document->doc_id) { // if duplicate node then it is removed
                DocumentsListNode *temp = runner->next;
                runner->next = runner->next->next;
                free(temp);
            } else {
                runner = runner->next; // if no duplicate ndoe then move to next node
            }
        }
        current = current->next; // move to next node
    }
}

/* Returns the intersection of two document lists */
DocumentsList *intersect_documents_lists(DocumentsList *list1, DocumentsList *list2) {
    if (!list1 || !list2 || !list1->head || !list2->head) return NULL; // checking if valid lists are passed as argument

    // allocate memory for the resulting intersection list
    DocumentsList *result = malloc(sizeof(DocumentsList));
    if (!result) return NULL; // checking list was allocated correctly

    // initialise result list
    result->head = NULL;
    result->tail = NULL;
    result->number_documents = 0;

    // create a temporary array to store doc_ids from list2 for faster lookup
    int *doc_ids = malloc(list2->number_documents * sizeof(int));
    if (!doc_ids) {
        free(result);
        return NULL;
    }
    int doc_count = 0;

    // store all doc_ids from list2 in temporary array
    DocumentsListNode *node2 = list2->head;
    while (node2 && doc_count < list2->number_documents) {
        doc_ids[doc_count++] = node2->document->doc_id;
        node2 = node2->next;
    }

    // checking each document in list1 is in list2
    DocumentsListNode *node1 = list1->head;
    while (node1) {
        int found = 0;
        for (int i = 0; i < doc_count; i++) {
            if (node1->document->doc_id == doc_ids[i]) {
                found = 1;
                break;
            }
        }

        if (found) { // if found create a new node in the result list pointing to the same document
            DocumentsListNode *new_node = malloc(sizeof(DocumentsListNode));
            if (!new_node) { // handling allocatiion fails
                free(doc_ids);
                free_documents_list(result);
                return NULL;
            }
            new_node->document = node1->document;
            new_node->next = NULL;

            // append the new node to the result list
            if (result->tail) {
                result->tail->next = new_node;
            } else {
                result->head = new_node;
            }
            result->tail = new_node;
            result->number_documents++;
        }
        node1 = node1->next;
    }
    // free temporary array before result list is returned
    free(doc_ids);
    return result;
}
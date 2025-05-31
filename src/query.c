#include "query.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Creates a new empty query list */
QueryList *create_query_list(void) {
    QueryList *list = malloc(sizeof(QueryList));
    if (!list) return NULL;
    list->head = NULL;
    return list;
}

/* Frees all memory associated with a query list */
void free_query_list(QueryList *list) {
    if (!list) return;

    QueryNode *current = list->head;
    while (current) {
        QueryNode *next = current->next;
        free(current->keyword);
        free(current);
        current = next;
    }
    free(list);
}

/* Adds a new keyword to the query list */
void add_keyword(QueryList *query_list, const char *keyword, QueryType type) {
    QueryNode *new_node = malloc(sizeof(QueryNode));
    if (!new_node) return;

    new_node->keyword = strdup(keyword);
    if (!new_node->keyword) {
        free(new_node);
        return;
    }
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

/* Parses a query string into a QueryList structure */
QueryList *parse_query(const char *input) {
    QueryList *list = create_query_list();
    if (!list) return NULL;

    char buffer[256] = {0}; // Ensure buffer is initialized
    int idx = 0;

    for (int i = 0;; i++) {
        char ch = input[i];
        if (ch == ' ' || ch == '\0') {
            if (idx > 0) {
                buffer[idx] = '\0'; // Null-terminate the buffer
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
            size_t idx = 0;
            if (idx < sizeof(buffer) - 1) {
                buffer[idx++] = ch;
            } else {
                fprintf(stderr, "Query word too long, truncating.\n");
                break;
            }
        }
    }
    return list;
}

/* Prints the query list for debugging purposes */
void print_query_list(const QueryList *list) {
    if (!list) return;

    QueryNode *current = list->head;
    while (current) {
        printf("[%s | %d] -> ", current->keyword, current->type);
        current = current->next;
    }
    printf("NULL\n");
}

/* Initializes a new query queue */
void init_queue_query(QueueQueries *queue) {
    queue->start = 0;
    queue->size = 0;
    for (int i = 0; i < 3; i++) {
        queue->queries[i] = NULL;
    }
}

/* Adds a query to the recent queries queue */
void enqueue_query(QueueQueries *queue, const char *keyword) {
    if (queue->size == 3) {
        free(queue->queries[queue->start]);
        queue->start = (queue->start + 1) % 3;
    } else {
        queue->size++;
    }

    int pos = (queue->start + queue->size - 1) % 3;
    queue->queries[pos] = strdup(keyword);
}

/* Frees all memory used by the query queue */
void free_queue_queries(QueueQueries *queue) {
    for (int i = 0; i < queue->size; i++) {
        free(queue->queries[(queue->start + i) % 3]);
    }
    queue->size = 0;
}

/* Frees a documents list and all its nodes */
void free_documents_list(DocumentsList *list) {
    if (!list) return;

    DocumentsListNode *current = list->head;
    while (current) {
        DocumentsListNode *next = current->next;
        free(current);
        current = next;
    }
    free(list);
}

/* Removes duplicate documents from a results list */
void remove_duplicate_results(DocumentsList *results) {
    if (!results || !results->head) return;

    DocumentsListNode *current = results->head;
    while (current) {
        DocumentsListNode *runner = current;
        while (runner->next) {
            if (runner->next->document->doc_id == current->document->doc_id) {
                DocumentsListNode *temp = runner->next;
                runner->next = runner->next->next;
                free(temp);
            } else {
                runner = runner->next;
            }
        }
        current = current->next;
    }
}

/* Returns the intersection of two document lists */
DocumentsList *intersect_documents_lists(DocumentsList *list1, DocumentsList *list2) {
    if (!list1 || !list2 || !list1->head || !list2->head) return NULL;

    DocumentsList *result = malloc(sizeof(DocumentsList));
    if (!result) return NULL;

    result->head = NULL;
    result->tail = NULL;
    result->number_documents = 0;

    // Create a temporary array to store doc_ids from list2 for faster lookup
    int *doc_ids = malloc(list2->number_documents * sizeof(int));
    if (!doc_ids) {
        free(result);
        return NULL;
    }
    int doc_count = 0;

    // Store all doc_ids from list2
    DocumentsListNode *node2 = list2->head;
    while (node2 && doc_count < list2->number_documents) {
        doc_ids[doc_count++] = node2->document->doc_id;
        node2 = node2->next;
    }

    // Check each document in list1 against list2
    DocumentsListNode *node1 = list1->head;
    while (node1) {
        int found = 0;
        for (int i = 0; i < doc_count; i++) {
            if (node1->document->doc_id == doc_ids[i]) {
                found = 1;
                break;
            }
        }

        if (found) {
            DocumentsListNode *new_node = malloc(sizeof(DocumentsListNode));
            if (!new_node) {
                free(doc_ids);
                free_documents_list(result);
                return NULL;
            }
            new_node->document = node1->document;
            new_node->next = NULL;

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

    free(doc_ids);
    return result;
}
//initialize query from string
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "query.h"

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
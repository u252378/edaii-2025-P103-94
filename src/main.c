#include "document.h"
#include "query.h"
#include "reverse_index.h"
#include "sort_search.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

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

void show_full_document(Document *doc) {
    if (!doc) return;

    printf("\n=================================\n");
    printf("ID: %d\nTitle: %s\n", doc->doc_id, doc->title);
    printf("Relevance: %.2f\n", doc->relevance);
    printf("---------------------------------\n");
    printf("%s\n", doc->body);

    if (doc->links) {
        printf("Links:\n");
        Link *current = doc->links;
        while (current != NULL) {
            printf("- Document ID: %d\n", current->id);
            current = current->next;
        }
    }

    printf("=================================\n");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <dataset_folder>\n", argv[0]);
        return 1;
    }

    Document *docs = load_documents_from_folder(argv[1]);
    if (!docs) {
        printf("No documents found or failed to load.\n");
        return 1;
    }

    for (Document *doc_current = docs; doc_current; doc_current = doc_current->next) {
        doc_current->relevance = calculate_relevance(doc_current, "");
    }

    ReverseIndex *reverse_index = build_reverse_index(docs);
    if (!reverse_index) {
        printf("Failed to build reverse index.\n");
        free_documents(docs);
        return 1;
    }

    char keyword[100];
    QueueQueries recent_queries;
    init_queue_query(&recent_queries);

    while (1) {
        printf("\nHELLO! Welcome to our program, please enter a word/words that you want to look for (or type 'exit' to finish): ");
        fflush(stdout);

        if (scanf("%99s", keyword) != 1) {
            printf("Failed to read input.\n");
            while (getchar() != '\n'); // limpia buffer
            continue;
        }
        while (getchar() != '\n'); // limpia buffer extra

        normalize_keyword(keyword);

        if (strcmp(keyword, "exit") == 0) {
            break;
        }

        enqueue_query(&recent_queries, keyword);

        printf("******* recent searches ********\n");
        for (int i = 0; i < 3; ++i) {
            int index = (recent_queries.start + i) % 3;
            if (recent_queries.size > i)
                printf("* %s *\n", recent_queries.queries[index]);
        }
        printf("********************************\n");

        DocumentsList *results = reverseIndexGet(reverse_index, keyword);
        if (!results || !results->head) {
            printf("No documents contain the word '%s'.\n", keyword);
            continue;
        }

        remove_duplicate_results(results);

        Document *temp_head = NULL;
        for (DocumentsListNode *node = results->head; node; node = node->next) {
            node->document->next = temp_head;
            temp_head = node->document;
        }

        temp_head = sort_documents_by_relevance(temp_head, keyword);

        printf("\n");
        Document *curr = temp_head;
        int index = 0, total_results = 0;

        for (Document *counter = temp_head; counter; counter = counter->next) {
            total_results++;
        }

        while (curr && index < 5) {
            printf("(%d) %s\n", index, curr->title);
            printf("---\n");

            if (curr->body) {
                for (int i = 0; i < 200 && curr->body[i]; i++) {
                    putchar(curr->body[i]);
                }
                if (strlen(curr->body) > 200) {
                    printf("...");
                }
            }

            printf("\n---\n");
            printf("relevance score: %.0f\n", curr->relevance);
            curr = curr->next;
            index++;
        }

        printf("[%d results]\n", total_results);

        int selection;
        printf("\nSelect document: ");
        fflush(stdout);

        if (scanf("%d", &selection) != 1) {
            while (getchar() != '\n');
            printf("Invalid input.\n");
            continue;
        }
        while (getchar() != '\n');

        curr = temp_head;
        int count = 0;
        while (curr && count < selection) {
            curr = curr->next;
            count++;
        }

        if (curr && count == selection) {
            print_document_compact(curr);
        } else {
            printf("Invalid selection.\n");
        }
    }

    free_reverse_index(reverse_index);
    free_documents(docs);
    free_queue_queries(&recent_queries);

    return 0;
}

#include "document.h"
#include "query.h"
#include "reverse_index.h"
#include "sort_search.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> // Para tolower()

// Normaliza una palabra (minúsculas, sin puntuación)
void normalize_keyword(char *word) {
    int i = 0, j = 0;
    while (word[i]) {
        if (isalpha(word[i])) {
            word[j++] = tolower(word[i]);
        }
        i++;
    }
    word[j] = '\0';
}

int main(int argc, char **argv) {
    // Verificación de argumentos
    if (argc < 2) {
        printf("Usage: %s <dataset_folder>\n", argv[0]);
        return 1;
    }

    // Carga de documentos
    Document *docs = load_documents_from_folder(argv[1]);
    if (!docs) {
        printf("No documents found or failed to load.\n");
        return 1;
    }

    // Construcción del índice inverso
    ReverseIndex *reverse_index = build_reverse_index(docs);
    if (!reverse_index) {
        printf("Failed to build reverse index.\n");
        free_documents(docs);
        return 1;
    }

    // --- PARTE NUEVA: Búsqueda interactiva ---
    char keyword[100];
    printf("Enter a word to search (or type 'exit' to finish): ");
    scanf("%99s", keyword);
    normalize_keyword(keyword);

    if (strcmp(keyword, "exit") != 0) {
        DocumentsList *results = reverseIndexGet(reverse_index, keyword);
        if (!results || !results->head) {
            printf("No documents contain the word '%s'.\n", keyword);
        } else {
            printf("\nDocuments containing '%s':\n", keyword);
            DocumentsListNode *node = results->head;
            while (node) {
                if (node->document) {
                    printf("- %s (ID: %d)\n", node->document->title, node->document->doc_id);
                }
                node = node->next;
            }
        }
    }

    // --- PARTE ORIGINAL (todo lo que ya tenías) ---
    printf("\n=== All documents ===\n");
    Document *current = docs;
    while (current) {
        print_document_details(current);  // Imprime ID, título, cuerpo y enlaces
        current = current->next;
    }

    printf("\n=== Documents sorted by relevance ===\n");
    docs = sort_documents_by_relevance(docs);
    print_sorted_documents(docs);  // Imprime títulos y relevancia

    // Liberar memoria
    free_reverse_index(reverse_index);
    free_documents(docs);
    return 0;
}
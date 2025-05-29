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

   // Búsqueda interactiva mejorada
    char keyword[100];
    do {
        printf("\nEnter a word to search (or type 'exit' to finish): ");
        scanf("%99s", keyword);
        normalize_keyword(keyword);

        if (strcmp(keyword, "exit") == 0) break;

        DocumentsList *results = reverseIndexGet(reverse_index, keyword);
        if (!results || !results->head) {
            printf("No documents contain the word '%s'.\n", keyword);
        } else {
            remove_duplicate_results(results); // Eliminar duplicados
            
            printf("\nDocuments containing '%s' (sorted by relevance):\n", keyword);
            DocumentsListNode *node = results->head;
            while (node) {
                if (node->document) {
                    printf("- %s (ID: %d, Relevance: %.2f)\n", 
                           node->document->title, 
                           node->document->doc_id,
                           node->document->relevance);
                }
                node = node->next;
            }
        }
    } while (1);

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
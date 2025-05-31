// main.c
#include "document.h"
#include "query.h"
#include "reverse_index.h"
#include "sort_search.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Shows all details of a document */
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

/* Comparador para qsort: orden descendente por relevancia */
static int cmp_relevance(const void *a, const void *b) {
    const Document *d1 = *(Document * const *)a;
    const Document *d2 = *(Document * const *)b;
    if (d1->relevance < d2->relevance) return 1;
    if (d1->relevance > d2->relevance) return -1;
    return 0;
}

/* Main program entry point */
int main(int argc, char **argv) {
    /* Check command line arguments */
    if (argc < 2) {
        printf("Usage: %s <dataset_folder>\n", argv[0]);
        return 1;
    }

    /* Load documents from specified folder */
    Document *docs = load_documents_from_folder(argv[1]);
    if (!docs) {
        printf("No documents found or failed to load.\n");
        return 1;
    }

    /* Build reverse index for efficient searching */
    ReverseIndex *reverse_index = build_reverse_index(docs);
    if (!reverse_index) {
        printf("Failed to build reverse index.\n");
        free_documents(docs);
        return 1;
    }

    /* Initialize recent queries queue */
    char query_str[256];
    QueueQueries recent_queries;
    init_queue_query(&recent_queries);

    /* Main interactive search loop */
    while (1) {
        printf("\n>>> Enter search query (use -word to exclude, or 'exit' to quit): ");
        fflush(stdout);

        /* Read user input */
        if (fgets(query_str, sizeof(query_str), stdin) == NULL) {
            break;
        }

        /* Remove trailing newline */
        query_str[strcspn(query_str, "\n")] = '\0';

        /* Check for exit command */
        if (strcmp(query_str, "exit") == 0) {
            break;
        }

        /* Skip empty queries */
        if (strlen(query_str) == 0) {
            continue;
        }

        /* Parse query into structured format */
        QueryList *query = parse_query(query_str);
        if (!query || !query->head) {
            printf("\nInvalid query format\n");
            if (query) free_query_list(query);
            continue;
        }

        /* Store query in recent queries */
        enqueue_query(&recent_queries, query_str);

        /* Display recent searches */
        printf("\nRecent searches:\n");
        for (int i = 0; i < recent_queries.size; i++) {
            int index = (recent_queries.start + i) % 3;
            printf("* %s\n", recent_queries.queries[index]);
        }

        /* Process each keyword in the query */
        DocumentsList *combined_results = NULL;
        int has_results = 1;
        QueryNode *keyword_node = query->head;
        
        while (keyword_node && has_results) {
            /* Normalize keyword: pasar a minúsculas y quitar no alfabéticos */
            normalize_keyword(keyword_node->keyword);

            /* Obtener lista de documentos que contienen la palabra */
            DocumentsList *results = reverseIndexGet(reverse_index, keyword_node->keyword);

            /* Si no hay resultados y es término INCLUDE, abortar */
            if (!results || !results->head) {
                if (keyword_node->type == INCLUDE) {
                    printf("\nNo documents contain the required term: %s\n", keyword_node->keyword);
                    has_results = 0;
                    if (combined_results) {
                        free_documents_list(combined_results);
                        combined_results = NULL;
                    }
                    break;
                } else {
                    /* Si es EXCLUDE u OR sin resultados, simplemente ignoramos para EXCLUDE
                       (no hay nada que excluir); para OR, podríamos juntar pero por simplicidad
                       lo tratamos como INCLUDE inexistente y devolvemos lista vacía. */
                    if (keyword_node->type == EXCLUDE) {
                        keyword_node = keyword_node->next;
                        continue;
                    } else {
                        /* OR: si no hay resultados, no influye en la intersección */
                        keyword_node = keyword_node->next;
                        continue;
                    }
                }
            }

            /* Eliminar duplicados dentro de esta lista de resultados */
            remove_duplicate_results(results);

            /* Procesar según el tipo de término */
            if (keyword_node->type == EXCLUDE) {
                /* Excluir documentos: filtrar combined_results excluyendo los de "results" */
                if (combined_results) {
                    DocumentsList *filtered = malloc(sizeof(DocumentsList));
                    if (!filtered) {
                        perror("malloc");
                        free_documents_list(results);
                        has_results = 0;
                        break;
                    }
                    filtered->head = NULL;
                    filtered->tail = NULL;
                    filtered->number_documents = 0;

                    DocumentsListNode *curr = combined_results->head;
                    while (curr) {
                        int should_include = 1;
                        DocumentsListNode *exclude_node = results->head;
                        while (exclude_node) {
                            if (curr->document->doc_id == exclude_node->document->doc_id) {
                                should_include = 0;
                                break;
                            }
                            exclude_node = exclude_node->next;
                        }
                        if (should_include) {
                            DocumentsListNode *new_node = malloc(sizeof(DocumentsListNode));
                            if (!new_node) {
                                perror("malloc");
                                free_documents_list(filtered);
                                free_documents_list(results);
                                free_documents_list(combined_results);
                                has_results = 0;
                                break;
                            }
                            new_node->document = curr->document;
                            new_node->next = NULL;
                            if (!filtered->head) {
                                filtered->head = new_node;
                                filtered->tail = new_node;
                            } else {
                                filtered->tail->next = new_node;
                                filtered->tail = new_node;
                            }
                            filtered->number_documents++;
                        }
                        curr = curr->next;
                    }
                    free_documents_list(combined_results);
                    free_documents_list(results);
                    combined_results = filtered;

                    if (!combined_results->head) {
                        has_results = 0;
                        printf("\nAll results excluded by term: %s\n", keyword_node->keyword);
                        break;
                    }
                } else {
                    /* Si no había resultados previos, nada que excluir */
                    free_documents_list(results);
                }
            }
            else {
                /* Termino INCLUDE u OR: intersectar o inicializar combined_results */
                if (!combined_results) {
                    /* Primera lista de INCLUDE/OR: simplemente asignar */
                    combined_results = results;
                } else {
                    /* Intersección entre combined_results y results */
                    DocumentsList *intersected = intersect_documents_lists(combined_results, results);
                    free_documents_list(combined_results);
                    free_documents_list(results);
                    combined_results = intersected;
                    if (!combined_results || !combined_results->head) {
                        has_results = 0;
                        printf("\nNo documents contain all required terms\n");
                        break;
                    }
                }
            }

            keyword_node = keyword_node->next;
        }

        /* Si no hay resultados validos o vacíos, limpiar y continuar */
        if (!has_results || !combined_results || !combined_results->head) {
            if (combined_results) free_documents_list(combined_results);
            free_query_list(query);
            continue;
        }

        /**********************************************************************
         * A PARTIR DE AQUÍ, USAMOS UN ARRAY DE Document* PARA ORDENAR POR RELEVANCIA
         **********************************************************************/

        /* 1) Calcular cuántos documentos hay en combined_results */
        int total = combined_results->number_documents;
        if (total <= 0) {
            /* No hay nada que mostrar */
            free_query_list(query);
            free_documents_list(combined_results);
            continue;
        }

        /* 2) Reservar un arreglo dinámico para puntadores a Document */
        Document **arr = malloc(total * sizeof(Document *));
        if (!arr) {
            perror("malloc");
            free_query_list(query);
            free_documents_list(combined_results);
            continue;
        }

        /* 3) Volcar cada Document* de la lista enlazada a arr[] */
        {
            DocumentsListNode *node = combined_results->head;
            for (int i = 0; i < total; i++) {
                arr[i] = node->document;
                node = node->next;
            }
        }

        /* 4) Ordenar arr[] por relevancia descendente */
        qsort(arr, total, sizeof(Document *), cmp_relevance);

        /* 5) Mostrar los top 5 resultados (o menos si total < 5) */
        printf("\nTop results for '%s':\n", query_str);
        int to_display = (total < 5) ? total : 5;
        for (int i = 0; i < to_display; i++) {
            Document *doc = arr[i];
            printf("\n(%d) %s\n", i, doc->title);
            printf("---\n");
            if (doc->body) {
                int chars_printed = 0;
                const char *body_ptr = doc->body;
                while (*body_ptr && chars_printed < 200) {
                    putchar(*body_ptr);
                    body_ptr++;
                    chars_printed++;
                }
                if (*body_ptr) printf("...");
            }
            printf("\n---\n");
            printf("relevance score: %.2f\n", doc->relevance);
        }
        printf("\n[%d total results]\n", total);

        /* 6) Pedir selección al usuario */
        int selection;
        printf("\nSelect document to view (0-%d): ", to_display - 1);
        fflush(stdout);
        if (scanf("%d", &selection) != 1) {
            while (getchar() != '\n'); // limpiar stdin
            printf("Invalid input.\n");
            free(arr);
            free_query_list(query);
            free_documents_list(combined_results);
            continue;
        }
        while (getchar() != '\n'); // descartar '\n' sobrante

        /* 7) Mostrar el documento completo si la selección es válida */
        if (selection >= 0 && selection < to_display) {
            show_full_document(arr[selection]);
        } else {
            printf("Invalid selection.\n");
        }

        /* 8) Liberar estructuras temporales */
        free(arr);
        free_query_list(query);
        free_documents_list(combined_results);
    }

    /* Programa finaliza: liberar índice e información de documentos */
    reverseIndexFree(reverse_index, true);
    free_documents(docs);
    free_queue_queries(&recent_queries);

    return 0;
}
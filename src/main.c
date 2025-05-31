// main.c
#include "document.h"
#include "query.h"
#include "reverse_index.h"
#include "sort_search.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* -----------------------------------------------------------------------
 * Función para mostrar un documento completo (título, cuerpo, enlaces)
 * ----------------------------------------------------------------------- */
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

/* -----------------------------------------------------------------------
 * Main program
 * ----------------------------------------------------------------------- */
int main(int argc, char **argv) {
    /* 1) Comprobación de argumentos */
    if (argc < 2) {
        printf("Usage: %s <dataset_folder>\n", argv[0]);
        return 1;
    }

    /* 2) Cargar todos los documentos desde la carpeta indicada */
    Document *docs = load_documents_from_folder(argv[1]);
    if (!docs) {
        printf("No documents found or failed to load.\n");
        return 1;
    }

    /* 3) Construir índice invertido */
    ReverseIndex *reverse_index = build_reverse_index(docs);
    if (!reverse_index) {
        printf("Failed to build reverse index.\n");
        free_documents(docs);
        return 1;
    }

    /* 4) Inicializar cola de consultas recientes */
    char query_str[256];
    QueueQueries recent_queries;
    init_queue_query(&recent_queries);

    /* 5) Bucle principal de interacción */
    while (1) {
        printf("\n>>> HELLO! Welcome to our program,\n");
        printf("    please enter a word/words that you want to look for\n");
        printf("    (or type 'exit' to finish): ");
        fflush(stdout);

        /* 5.1) Leer línea de usuario */
        if (fgets(query_str, sizeof(query_str), stdin) == NULL) {
            break;
        }
        /* Quitar el salto de línea final */
        query_str[strcspn(query_str, "\n")] = '\0';

        /* 5.2) Si el usuario escribe "exit", terminar */
        if (strcmp(query_str, "exit") == 0) {
            break;
        }
        /* 5.3) Evitar procesar cadena vacía */
        if (strlen(query_str) == 0) {
            continue;
        }

        /* 5.4) Parsear la consulta en QueryList */
        QueryList *query = parse_query(query_str);
        if (!query || !query->head) {
            printf("\nInvalid query format\n");
            if (query) free_query_list(query);
            continue;
        }

        /* 5.5) Añadir la consulta a la cola de recientes */
        enqueue_query(&recent_queries, query_str);

        /* 5.6) Mostrar últimas 3 búsquedas */
        printf("\nRecent searches:\n");
        for (int i = 0; i < recent_queries.size; i++) {
            int index = (recent_queries.start + i) % 3;
            printf("* %s\n", recent_queries.queries[index]);
        }

        /* 5.7) Iterar sobre cada término de la query */
        DocumentsList *combined_results = NULL;
        int has_results = 1;
        QueryNode *keyword_node = query->head;

        while (keyword_node && has_results) {
            /* 5.7.1) Normalizar la palabra (todo a minúsculas, quitar no alfabéticos) */
            normalize_keyword(keyword_node->keyword);

            /* 5.7.2) Obtener lista de documentos que contienen este término */
            DocumentsList *results = reverseIndexGet(reverse_index, keyword_node->keyword);

            /* 5.7.3) Si no hay resultados y el término era INCLUDE, abortar búsqueda */
            if (!results || !results->head) {
                if (keyword_node->type == INCLUDE) {
                    printf("\nNo documents contain the required term: %s\n",
                           keyword_node->keyword);
                    has_results = 0;
                    if (combined_results) {
                        free_documents_list(combined_results);
                        combined_results = NULL;
                    }
                    if (results) free_documents_list(results);
                    break;
                }
                /* 5.7.4) Si es EXCLUDE y results == NULL, no hay nada que excluir */
                if (keyword_node->type == EXCLUDE) {
                    if (results) free_documents_list(results);
                    keyword_node = keyword_node->next;
                    continue;
                }
                /* 5.7.5) Si es OR y results == NULL, simplemente ignorar este término */
                if (keyword_node->type == OR) {
                    if (results) free_documents_list(results);
                    keyword_node = keyword_node->next;
                    continue;
                }
            }

            /* 5.7.6) Eliminar posibles duplicados dentro de 'results' */
            remove_duplicate_results(results);

            /* 5.7.7) Procesar término EXCLUDE */
            if (keyword_node->type == EXCLUDE) {
                if (combined_results) {
                    /* Filtrar combined_results excluyendo los doc_id en results */
                    DocumentsList *filtered = malloc(sizeof(DocumentsList));
                    if (!filtered) {
                        perror("malloc");
                        free_documents_list(results);
                        free_documents_list(combined_results);
                        has_results = 0;
                        break;
                    }
                    filtered->head = NULL;
                    filtered->tail = NULL;
                    filtered->number_documents = 0;

                    /* Recorrer combined_results y copiar solo lo que no esté en results */
                    DocumentsListNode *curr = combined_results->head;
                    while (curr) {
                        int should_include = 1;
                        DocumentsListNode *exclude_node = results->head;
                        while (exclude_node) {
                            if (curr->document->doc_id ==
                                exclude_node->document->doc_id) {
                                should_include = 0;
                                break;
                            }
                            exclude_node = exclude_node->next;
                        }
                        if (should_include) {
                            DocumentsListNode *new_node =
                                malloc(sizeof(DocumentsListNode));
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
                        printf("\nAll results excluded by term: %s\n",
                               keyword_node->keyword);
                        break;
                    }
                } else {
                    /* Si no había combinados aún, no hay nada que excluir */
                    free_documents_list(results);
                }
            }
            /* 5.7.8) Procesar término INCLUDE u OR */
            else {
                if (!combined_results) {
                    /* Primer término, simplemente asignar results */
                    combined_results = results;
                } else {
                    /* Intersección entre combined_results y results */
                    DocumentsList *intersected =
                        intersect_documents_lists(combined_results, results);
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

        /* 5.8) Si no hay resultados o la lista está vacía, liberar y continuar */
        if (!has_results || !combined_results || !combined_results->head) {
            if (combined_results) free_documents_list(combined_results);
            free_query_list(query);
            continue;
        }

        /* -----------------------------------------------------------------
         * 6) ORDENACIÓN Y MUESTRA DE RESULTADOS MEDIANTE sort_search.c
         * ----------------------------------------------------------------- */

        /* 6.1) Convertir DocumentsList en lista enlazada temporal de Document*
         *      (temp_head) para que funcione sort_documents_by_relevance  */
        Document *temp_head = NULL;
        DocumentsListNode *node = combined_results->head;
        while (node) {
            /* Enlazar los Document* en una lista separada */
            Document *d = node->document;
            d->next = temp_head;
            temp_head = d;
            node = node->next;
        }

        /* 6.2) Calcular relevancia y ordenar con merge-sort */
        temp_head = sort_documents_by_relevance(temp_head);

        /* 6.3) Mostrar los cinco primeros resultados en orden descendente */
        printf("\nTop results for '%s':\n", query_str);
        Document *curr = temp_head;
        int displayed = 0;
        while (curr && displayed < 5) {
            printf("\n(%d) %s\n", displayed, curr->title);
            printf("---\n");
            if (curr->body) {
                int cnt = 0;
                const char *p = curr->body;
                while (*p && cnt < 200) {
                    putchar(*p);
                    p++;
                    cnt++;
                }
                if (*p) printf("...");
            }
            printf("\n---\n");
            printf("relevance score: %.2f\n", curr->relevance);
            curr = curr->next;
            displayed++;
        }
        if (displayed == 0) {
            printf("No results to display.\n");
        }
        /* Si queremos indicar “[5 total results]” siempre, aunque haya menos: */
        printf("\n[%d total results]\n", combined_results->number_documents);

        /* 6.4) Interfaz de selección de documento */
        if (displayed > 0) {
            int selection;
            printf("\nSelect document to view (0-%d): ",
                   (displayed - 1));
            fflush(stdout);

            if (scanf("%d", &selection) != 1) {
                while (getchar() != '\n') {
                }
                printf("Invalid input.\n");
                /* Liberar antes de continuar al siguiente prompt */
                free_query_list(query);
                free_documents_list(combined_results);
                /* (No liberar temp_head, pues estamos reutilizando los mismos Document) */
                continue;
            }
            while (getchar() != '\n')
                ;

            if (selection >= 0 && selection < displayed) {
                /* Volver a recorrer temp_head hasta llegar al índice elegido */
                curr = temp_head;
                for (int k = 0; k < selection; k++) {
                    curr = curr->next;
                }
                show_full_document(curr);
            } else {
                printf("Invalid selection.\n");
            }
        }

        /* 6.5) Liberar estructuras temporales (EXCEPTO los Document originales) */
        free_query_list(query);
        free_documents_list(combined_results);

        /* IMPORTANTE: temp_head es solo punteros a Document ya existentes.
         * No lo liberamos con free(): eso borraría los Document mismos.
         * Simplemente “desechamos” el puntero temp_head asignándole NULL: */
        temp_head = NULL;
    }

    /* 7) Al salir del bucle, liberar índice y documentos globales */
    reverseIndexFree(reverse_index, true);
    free_documents(docs);
    free_queue_queries(&recent_queries);

    return 0;
}
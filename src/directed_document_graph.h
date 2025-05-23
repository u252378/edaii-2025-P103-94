#ifndef DIRECTED_DOCUMENT_GRAPH_H
#define DIRECTED_DOCUMENT_GRAPH_H

#include "document.h"

typedef struct {
    int num_vertices;
    float** adjacency_matrix; // adjacency matrix that will store edges weights between vertices
    Document** documents; // array of pointers with pointers to each Document
} Graph;


Graph* create_graph(int N);
void add_vertex(Graph *graph, Document* document);
void add_edge(Graph * graph, int i, int j, float weight);
void delete_vertex(Graph * graph, int i);
void delete_edge(Graph * graph, int i, int j);
void delete_graph(Graph * graph);
void set_vertex(Graph* graph, int i, Document* document);
void set_edge(Graph* graph, int i, int j, float weight);
void list_vertex(Graph* graph);
void list_edges(Graph* graph);
void print_matrix(Graph * graph);

#endif
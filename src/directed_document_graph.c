# include <stdio.h>
# include <stdlib.h>
# include "directed_document_graph.h"

// function to create a graph
Graph* create_graph(int N) {

    // allocate memory for the graph
    Graph* graph = (Graph*)malloc(sizeof(Graph));

    // initialise num_vertices variable to the amount passed as argument to function
    graph -> num_vertices = N;

    // allocating memory for the adjacency matrix, that is an array of float pointers
    // float is used for the weighted edges
    graph -> adjacency_matrix = malloc(sizeof(float*) * N);

    // allocating memory and initialise rows of adjacency matrix
    for (int i = 0; i < N; i++) {
        graph -> adjacency_matrix[i] = calloc(N, sizeof(float));
    }

    // allocate memory for array of Document pointers, and initialise to NULL
    graph -> documents = calloc(N, sizeof(Document*));

    // then when having allocated and initialised the adjacency matrix, it is returned
    return graph;
}





// function to add a vertex to the graph
void add_vertex(Graph *graph, Document* document) {

    // reallocate adjacency matrix to have one more row, this row is for the new vertex
    graph -> adjacency_matrix = realloc(graph -> adjacency_matrix, sizeof(float*) * (graph -> num_vertices + 1));

    // reallcate each row to add new column in the adjacency matrix for the new vertex
    for (int j = 0; j < graph -> num_vertices; j++) {
        graph -> adjacency_matrix[j] = realloc(graph -> adjacency_matrix[j], sizeof(float) * (graph -> num_vertices + 1));
        graph -> adjacency_matrix[j][graph -> num_vertices] = 0.0; // new column is initiliased with 0.0
    }

    // allocate a new row for the new vertex and initialise the row with 0.0
    graph -> adjacency_matrix[graph -> num_vertices] = calloc(graph -> num_vertices + 1, sizeof(float));

    // reallocate array of documents to have another Document pointer
    graph -> documents = realloc(graph -> documents, sizeof(Document*) * (graph -> num_vertices + 1));

    // set the appropiate position in the documents array with the document passed as argument
    graph -> documents[graph -> num_vertices] = document;

    // increse number of vertices in graph
    graph -> num_vertices = graph -> num_vertices + 1;
}





// function to add a directed edge from i to j with an specific weight
void add_edge(Graph * graph, int i, int j, float weight) {
    if (i>=0 && i < graph -> num_vertices && j>= 0 && j < graph -> num_vertices) { // checking both indices of vertex are within valid ranges
        // set the weight of the edge from vertex i to vertex j in adjacency matrix
        graph -> adjacency_matrix[i][j] = weight;
    }
}




// function to delete vertex at index i from graph
void delete_vertex(Graph * graph, int i) {
    
    // handle case where there is not a valid index i
    if (i<0 || i >= graph-> num_vertices){
        return;
    }

    // to shift the array of documents to remove the document at index i
    for (int j = i; j < graph -> num_vertices -1; j++) {
        graph -> documents[j] = graph -> documents[j + 1];
    }

    // reallocate array of documents to have size reduced by 1
    graph -> documents = realloc(graph -> documents, sizeof(Document*) * (graph -> num_vertices -1));

    // free row of corresponding vertex that is deleted
    free(graph -> adjacency_matrix[i]);

    // to shift rows of adjancency matrix to remove row at index i
    for (int k = i; k < graph -> num_vertices - 1; k++) {
        graph -> adjacency_matrix[k] = graph -> adjacency_matrix[k + 1];
    }

    // reallocate adjacency matrix to remove a row
    graph -> adjacency_matrix = realloc(graph -> adjacency_matrix, sizeof(float*) * (graph -> num_vertices - 1));

    // to shift the columns left in each remaining row
    for (int q = 0; q < graph -> num_vertices - 1; q++) {
        for (int l = i; l < graph -> num_vertices - 1; l++) {
            graph -> adjacency_matrix[q][l] = graph -> adjacency_matrix[q][l + 1];
        }
        // reallocate each row, so that it has one column less than earlier
        graph -> adjacency_matrix[q] = realloc(graph -> adjacency_matrix[q], sizeof(float) * (graph -> num_vertices - 1));
    }

    // decrease number of vertices in graph
    graph -> num_vertices--;
}






// function to delete a directed edge from i to j
void delete_edge(Graph * graph, int i, int j) {
    if(i >= 0 && i < graph -> num_vertices && j >= 0 && j < graph -> num_vertices) { // checking both indices of vertex are within valid ranges
        // set the weight of the edge from vettex i to vertex j to 0.0 in adjacency matrix to indicate no there is no edge
        graph -> adjacency_matrix[i][j] = 0.0;
    }
}




// function to delete graph
void delete_graph(Graph * graph) {

    // free each row of adjacency matrix
    for (int i = 0; i < graph -> num_vertices; i++) {
        free(graph -> adjacency_matrix[i]);
    }
    
    // free array of pointers of adjacency matrix
    free(graph -> adjacency_matrix);

    // free array of Document pointer
    free(graph -> documents);

    // free the memory allocated for the graph
    free(graph);
}




// function to set a document at a vertex
void set_vertex(Graph* graph, int i, Document* document) {
    if(i >= 0 && i < graph -> num_vertices) { // checking index i is a valid index
        // set new document at index i in array of documents
        graph -> documents[i] = document;
    }
}



// function to set the weight of the edge from vertex i to vertex j 
void set_edge(Graph* graph, int i, int j, float weight) {
    if (i >= 0 && i < graph -> num_vertices && j >= 0 && j < graph -> num_vertices) { // checking indices i and j are valid indices
        // set new weight of edge from vertex i to vertex j
        graph -> adjacency_matrix[i][j] = weight;
    }
}



// function to print each vertex index and the document id of document assigned to the vertex
void list_vertex(Graph* graph) {
    printf("The vertices of the graph are:\n");

    // iterate through each vertex index of graph
    for (int i = 0; i< graph -> num_vertices; i++) {
        if (graph -> documents[i] != NULL) { // checking if there is a document at index i in the array of documents
            printf("vertex %d has document: %d\n", i, graph -> documents[i] -> doc_id);
        }
    }
}


// function to print edges in the graph with weights
void list_edges(Graph* graph) {
    printf("The edges of the graph are:\n");

    for (int i = 0; i < graph -> num_vertices; i++) {
        for (int j = 0; j < graph -> num_vertices; j++) {
            if (graph -> adjacency_matrix[i][j] != 0.0) { // checking if there is an edge from vertex i to vertex j
                printf("The edge from vertex %d to vertex %d has a weight of %.2f\n", i, j, graph -> adjacency_matrix[i][j]);
            }
        }
    }
}


// function to print adjacency matrix
void print_matrix(Graph * graph) {
    printf("The weighted adjacency matrix is:\n");

    for (int i = 0; i < graph -> num_vertices; i++) {
        for (int j = 0; j < graph -> num_vertices; j++) {
            printf("%.2f ", graph -> adjacency_matrix[i][j]);
        }
        printf("\n");
    }
}

//Get indegree of a document in the graph and print it as the relevance score: 
//function to get indegree (relevance score) of a document at vertex index j
int get_indegree(Graph* graph, int j) {
    int indegree = 0;

    if (j < 0 || j >= graph->num_vertices) {
        return -1; //invalid vertex index
    }

    for (int i = 0; i < graph->num_vertices; i++) {
        if (graph->adjacency_matrix[i][j] != 0.0) {
            indegree++;
        }
    }

    return indegree;
}

//function to print all at once:
void print_relevance_scores(Graph* graph) { 
    printf("Relevance scores (indegree) of each document:\n");
    for (int j = 0; j < graph->num_vertices; j++) {
        if (graph->documents[j] != NULL) {
            int indegree = get_indegree(graph, j);
            printf("Document ID %d has relevance score: %d\n", graph->documents[j]->doc_id, indegree);
        }
    }
}
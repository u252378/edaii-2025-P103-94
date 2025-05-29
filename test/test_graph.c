#include "../src/directed_document_graph.h"
#include "../src/document.h"
#include <stdio.h>
#include <stdlib.h>

int test_graph() {
  int test_fail = 0;

  // creating a graph with 2 vertices
  Graph *graph = create_graph(2);
  if (graph == NULL) {
    printf("Memory allocation failed\n");
    return 1;
  }

  // allocating memory and initialising all fields to NULL for documents doc1
  // and doc2
  Document *doc1 = calloc(1, sizeof(Document));
  Document *doc2 = calloc(1, sizeof(Document));
  if (doc1 == NULL || doc2 == NULL) { // handling memory allocation failed
    printf("Test failed\n");
    free_documents(doc1);
    delete_graph(graph);
    return 1;
  }

  // initialising only the id field of documents
  doc1->doc_id = 1;
  doc2->doc_id = 2;

  // setting vertex 0 with doc1
  set_vertex(graph, 0, doc1);
  // setting vertex 1 with doc2
  set_vertex(graph, 1, doc2);

  // setting one directed edge from node 0 to node 1 weight 3.5
  add_edge(graph, 0, 1, 3.5);

  // checking number of vertices
  if (graph->num_vertices != 2) {
    printf("Test failed\n");
    test_fail++;
  }

  // checking document id of each vertex in graph
  if (graph->documents[0]->doc_id != 1) {
    printf("Test fail\n");
    test_fail++;
  }
  if (graph->documents[1]->doc_id != 2) {
    printf("Test fail\n");
    test_fail++;
  }

  // checking edge weights
  if (graph->adjacency_matrix[0][1] != 3.5) {
    printf("Test failed\n");
    test_fail++;
  }
  if (graph->adjacency_matrix[1][0] != 0.0) {
    printf("Test failed\n");
    test_fail++;
  }

  // checking indegree
  int indegree1 = get_indegree(graph, 1);
  if (indegree1 != 1) {
    printf("Test failed\n");
    test_fail++;
  }
  int indegree0 = get_indegree(graph, 0);
  if (indegree0 != 0) {
    printf("Test failed\n");
    test_fail++;
  }

  // then after the test, freeing memory of graph and of documents doc1 and doc2
  delete_graph(graph);
  free_documents(doc1);

  return test_fail;
}

int main() {
  int result = test_graph();
  if (result == 0) {
    printf("Test passed\n");
    return 0;
  } else {
    printf("Test failed\n");
    return 1;
  }
}
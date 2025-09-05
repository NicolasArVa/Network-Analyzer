#include <stdio.h>
#include <stdlib.h>
#include "../include/core/graph.h"

#define N 4

/*
int main(int argc, char* argv[]) {
    printf("Network Analysis Toolkit v0.1.0\n");
    
    if (argc < 2) {
        printf("Usage: %s <network_file>\n", argv[0]);
        return 1;
    }
    
    // TODO: Implement network loading and analysis
    printf("Loading network from: %s\n", argv[1]);
    
    return 0;
}
*/

int main() {
    printf("Network Analysis Toolkit - Testing Basic Graph\n");

    // initialize small graph
    Graph* g = graph_create(GRAPH_UNDIRECTED, 4);
    if (!g) {
        fprintf(stderr, "Failed to create graph\n");
        return 1;
    }

    // adding nodes
    printf("Adding nodes 1, 2, 3...\n");
    graph_add_node(g, 1, N);
    graph_add_node(g, 2, N);
    graph_add_node(g, 3, N);

    // Add edges
    printf("Adding edges...\n");
    graph_add_edge(g, 1, 2, 1.5);
    graph_add_edge(g, 2, 3, 2.0);
    graph_add_edge(g, 3, 1, 0.8);

    // check result
    printf("Graph has %zu nodes and %zu edges\n",
        graph_node_count(g), graph_edge_count(g));

    // clean up
    graph_destroy(g);
    printf("Test completed succesfully.\n");

    return 0;
}
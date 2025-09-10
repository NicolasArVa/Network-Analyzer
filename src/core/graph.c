#include <stdio.h>
#include <stdlib.h>
#include "../../include/core/graph.h"

// Basic properties getters
size_t graph_node_count(const Graph* graph) {
    if (!graph) return 0;
    return graph->node_count;
}

size_t graph_edge_count(const Graph* graph) {
    if (!graph) return 0;

    size_t total_edges = 0;
    for (size_t i = 0; i < graph->node_count; i++) {
        total_edges += graph->nodes[i]->neighbor_count;
    }

    // For undirected graphs, each edge is counted twice
    if (graph->type == GRAPH_UNDIRECTED) {
        total_edges /= 2;
    }

    return total_edges;
}
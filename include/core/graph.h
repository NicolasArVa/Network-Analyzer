#ifndef GRAPH_H
#define GRAPH_H

#include <stddef.h>
#include <stdbool.h>

// Forward declarations
typedef struct Graph Graph;
typedef struct Node Node;
typedef struct Edge Edge;

// Graph type
typedef enum {
    GRAPH_UNDIRECTED,
    GRAPH_DIRECTED
} GraphType;

// Basic graph operations
Graph* graph_create(GraphType type, size_t initial_capacity);
void graph_destory(Graph* graph);
bool graph_add_node(Graph* graph, int node_id);
bool graph_add_edge(Graph* graph, int from, int to, double weight);
bool graph_remove_node(Graph* graph, int node_id);
bool graph_remove_edge(Graph* graph, int from, int to);
size_t graph_node_count(const Graph* graph);
size_t graph_edge_count(const Graph* graph);

#endif // GRAPH_H
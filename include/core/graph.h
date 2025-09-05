#ifndef GRAPH_H
#define GRAPH_H

#include <stddef.h>
#include <stdbool.h>

// Graph type
typedef enum {
    GRAPH_UNDIRECTED,
    GRAPH_DIRECTED
} GraphType;

// Forward declarations
typedef struct {
    int node_id;
    double weight;
} Edge;

typedef struct {
    int id;
    Edge* edges;
    size_t edge_count;
    size_t edge_capacity;
} Node;

typedef struct {
    GraphType type;
    Node* nodes;
    size_t node_count;
    size_t node_capacity;
} Graph;


// Basic graph operations
Graph* graph_create(GraphType type, size_t initial_capacity);
void graph_destroy(Graph* graph);
bool graph_add_node(Graph* graph, int node_id, int initial_capacity);
bool graph_add_edge(Graph* graph, int from, int to, double weight);

size_t graph_node_count(const Graph* graph);
size_t graph_edge_count(const Graph* graph);

bool graph_remove_node(Graph* graph, int node_id);
bool graph_remove_edge(Graph* graph, int from, int to);
size_t graph_node_count(const Graph* graph);
size_t graph_edge_count(const Graph* graph);

#endif // GRAPH_H
#ifndef GRAPH_BUILD_H
#define GRAPH_BUILD_H

#include <stddef.h>
#include <stdbool.h>
#include "utils/general_utils.h"

#define INITIAL_CAPACITY 4

// Graph type
typedef enum {
    GRAPH_UNDIRECTED,
    GRAPH_DIRECTED
} GraphType;


// * linked list of neighbors and its edges 
typedef struct {
    int node_id;
    double weight;
} EdgeNode;

// * simple struct with dynamic array of edges 
typedef struct Node {
    int id;
    EdgeNode* neighbors; // * Dynamic array
    size_t neighbor_count;
    size_t neighbor_capacity;
    struct Node *next;
} Node;

// * simple struct with dynamic array of nodes
typedef struct {
    GraphType type;
    Node** nodes; // * Hash table for nodes
    Node** node_ids;
    size_t node_count;
    size_t node_capacity;
} Graph;

// TODO: change bool to -1, 0, 1

// Graph initialization/deletion tools
Graph* graph_create(GraphType type, size_t initial_capacity);
void graph_destroy(Graph* graph);

// Edit graph tools
Status graph_insert_node(Graph* graph, int node_id, size_t initial_capacity);
Status graph_remove_node(Graph* graph, int node_id);

Status graph_insert_edge(Graph* graph, int from, int to, double weight);
Status graph_update_edge(Graph* graph, int from, int to, double weight);
Status graph_remove_edge(Graph* graph, int from, int to);

// Basic properties getters
size_t graph_node_count(const Graph* graph);
size_t graph_edge_count(const Graph* graph);

#endif
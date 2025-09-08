#ifndef GRAPH_H
#define GRAPH_H

#include <stddef.h>
#include <stdbool.h>

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
typedef struct {
    int id;
    EdgeNode* neighbors; // ! latter implement hash table for O(1) access for densly connected graphs  
    size_t neighbor_count;
    size_t capacity;
} Node;

// * simple struct with dynamic array of nodes
typedef struct {
    GraphType type;
    Node* nodes; // ! I want a hash table of nodes for O(1) access
    size_t node_count;
    size_t node_capacity;
} Graph;


// Graph building tools
Graph* graph_create(GraphType type, size_t initial_capacity);
void graph_destroy(Graph* graph);
bool graph_add_node(Graph* graph, int node_id, int initial_capacity);
bool graph_add_edge(Graph* graph, int from, int to, double weight);

size_t graph_node_count(const Graph* graph);
size_t graph_edge_count(const Graph* graph);

bool graph_remove_node(Graph* graph, int node_id);
bool graph_remove_edge(Graph* graph, int from, int to);

// Basic properties getters
size_t graph_node_count(const Graph* graph);
size_t graph_edge_count(const Graph* graph);

#endif GRAPH_H
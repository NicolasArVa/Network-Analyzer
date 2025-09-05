#include <stdio.h>
#include <stdlib.h>
#include "../../include/core/graph.h"

# define INITIAL_CAPACITY 4

// Basic graph operations
Graph* graph_create(GraphType type, size_t initial_capacity) {
    Graph* graph = malloc(sizeof(Graph));
    if (!graph) return NULL;

    graph->type = type;
    graph->nodes = malloc(initial_capacity * sizeof(Node));
    if (!graph->nodes) {
        free(graph);
        return NULL;
    };
    graph->node_count = 0;
    graph->node_capacity = initial_capacity;

    return graph;
}

void graph_destroy(Graph* graph) {
    if (!graph) return;

    // Free each node's edges
    for (size_t i = 0; i < graph->node_count; i++) {
        free(graph->nodes[i].edges);
    }
    free(graph->nodes);
    free(graph);
}

// Helper: find Node by ID (linear search)
static Node* find_node(Graph* graph, int node_id) {
    for (size_t i = 0; i < graph->node_count; i++) {
        if (graph->nodes[i].id == node_id) {
            return &graph->nodes[i];
        }
    }
    return NULL;
}

// Helper: detect if edge exists
static bool edge_exists(Node* node, int to) {
    for (size_t i = 0; i < node->edge_count; i++) {
        if (node->edges[i].node_id == to) return true;
    }

    return false;
}

bool graph_add_node(Graph* graph, int node_id, int initial_capacity) {
    if (!graph) return false;

    // Check if node already exists
    if (find_node(graph, node_id)) return false;

    // Check if nodes arrays needs rezising
    if (graph->node_count >= graph->node_capacity) {
        size_t new_capacity = graph->node_capacity * 2;
        Node* new_nodes = realloc(graph->nodes, new_capacity * sizeof(Node));
        if (!new_nodes) return false;

        graph->nodes = new_nodes;
        graph->node_capacity = new_capacity;
    }

    // Initialize new node
    if (initial_capacity <= 0) initial_capacity = INITIAL_CAPACITY;

    Node* new_node = &graph->nodes[graph->node_count++]; // * This increments node_count globally not just in the index
    new_node->id = node_id;
    new_node->edges = malloc(sizeof(Edge) * initial_capacity);
    if (!new_node->edges) return false;

    new_node->edge_count = 0;
    new_node->edge_capacity = initial_capacity;

    // graph->node_count++; //* This is done above when assigning new_node
    return true;
}

bool graph_add_edge(Graph* graph, int from, int to, double weight) {
    if (!graph) return false;

    Node* from_node = find_node(graph, from);
    Node* to_node = find_node(graph, to);

    if (!from_node || !to_node) return false;

    // Check if edge already exists
    if (edge_exists(from_node, to)) return false;

    // Check if edges array needs resizing
    if (from_node->edge_count >= from_node->edge_capacity) {
        size_t new_capacity = from_node->edge_capacity * 2;
        Edge* new_edges = realloc(from_node->edges, new_capacity * sizeof(Edge));
        if (!new_edges) return false;

        from_node->edges = new_edges;
        from_node->edge_capacity = new_capacity;
    }

    // Add new edge
    Edge* new_edge = &from_node->edges[from_node->edge_count++];
    new_edge->node_id = to;
    new_edge->weight = weight;
    from_node->edge_count ++;

    // For undirected graphs, add reverse edge
    if (graph->type == GRAPH_UNDIRECTED) {
        // Check if edge already exists
        if (edge_exists(from_node, to)) return false;

        // Check if edges array needs resizing
        if (to_node->edge_count >= to_node->edge_capacity) {
            size_t new_capacity = to_node->edge_capacity * 2;
            Edge* new_edges = realloc(to_node->edges, new_capacity * sizeof(Edge));
            if (!new_edges) return false;

            to_node->edges = new_edges;
            to_node->edge_capacity = new_capacity;
        }

        // Add reverse edge
        Edge* new_edge = &to_node->edges[to_node->edge_count++];
        new_edge->node_id = from;
        new_edge->weight = weight;

        to_node->edge_count ++;
    }

    return true;
}

size_t graph_node_count(const Graph* graph) {
    if (!graph) return 0;
    return graph->node_count;
}

size_t graph_edge_count(const Graph* graph) {
    if (!graph) return 0;

    size_t total_edges = 0;
    for (size_t i = 0; i < graph->node_count; i++) {
        total_edges += graph->nodes[i].edge_count;
    }

    // For undirected graphs, each edge is counted twice
    if (graph->type == GRAPH_UNDIRECTED) {
        total_edges /= 2;
    }

    return total_edges;
}

bool graph_remove_edge(Graph* graph, int from, int to);

/* 
bool graph_remove_node(Graph* graph, int node_id) {
    if (!graph) return false;

    Node* target_node = find_node(graph, node_id);
    if (!target_node) return false;

    // Remove all edges to this node
    for (size_t i = 0; i < graph->node_count; i++) {
        Node* current_node = &graph->nodes[i];

        if (current_node->id == node_id) {
            graph->nodes[i] = NULL; // ? How do I delete this properly?
            continue; // Skip target node itself
        }

        for (size_t j = 0; j < current_node->edge_count; j++) {
            if (current_node->edges[j].node_id == node_id) {
                graph_remove_edge(graph, current_node->id, node_id);
            }
        }
    }

    // Free target node's edges
    free(target_node->edges);
    free(target_node);

    return true;
}
*/

size_t graph_node_count(const Graph* graph);
size_t graph_edge_count(const Graph* graph);
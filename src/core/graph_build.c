#include <stdio.h>
#include <stdlib.h>
#include "../../include/core/graph.h"

# define INITIAL_CAPACITY 4

// Basic graph operations
Graph* graph_create(GraphType type, size_t initial_capacity) {
    Graph* graph = malloc(sizeof(Graph));
    if (!graph) return NULL;

    if (initial_capacity <= 0) initial_capacity = INITIAL_CAPACITY;

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
        free(graph->nodes[i].neighbors);
    }
    free(graph->nodes);
    free(graph);
}

// Helper: find Node by ID (linear search)
// TODO: look for a faster method
static Node* find_node(Graph* graph, int node_id) {
    for (size_t i = 0; i < graph->node_count; i++) {
        if (graph->nodes[i].id == node_id) {
            return &graph->nodes[i];
        }
    }
    return NULL;
}

// Helper: detect if edge exists
// TODO: look for a faster method
static bool edge_exists(Node* node, int to) {
    for (size_t i = 0; i < node->neighbor_count; i++) {
        if (node->neighbors[i].node_id == to) return true;
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
    new_node->neighbors = malloc(sizeof(EdgeNode) * initial_capacity);
    if (!new_node->neighbors) return false;

    new_node->neighbor_count = 0;
    new_node->capacity = initial_capacity;

    // graph->node_count++; // * This is done above when assigning new_node
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
    if (from_node->neighbor_count >= from_node->capacity) {
        size_t new_capacity = from_node->capacity * 2;
        EdgeNode* new_neighbors = realloc(from_node->neighbors, new_capacity * sizeof(EdgeNode));
        if (!new_neighbors) return false;

        from_node->neighbors = new_neighbors;
        from_node->capacity = new_capacity;
    }

    // Add new edge/neighbor
    EdgeNode* new_neighbor = &from_node->neighbors[from_node->neighbor_count ++];

    new_neighbor->node_id = to;
    new_neighbor->weight = weight;

    // For undirected graphs, add reverse edge
    if (graph->type == GRAPH_UNDIRECTED) {
        // ! no checking for reverse edge here. if graph is undirected, this should have benn detected earlier
        // Check if edges array needs resizing
        if (to_node->neighbor_count >= to_node->capacity) {
            size_t new_capacity = to_node->capacity * 2;
            EdgeNode* new_neighbor = realloc(to_node->neighbors, new_capacity * sizeof(EdgeNode));
            if (!new_neighbor) return false;

            to_node->neighbors = new_neighbor;
            to_node->capacity = new_capacity;
        }

        // Add reverse edge
        EdgeNode* new_edge = &to_node->neighbors[to_node->neighbor_count ++];

        new_edge->node_id = from;
        new_edge->weight = weight;
    }

    return true;
}

bool graph_remove_edge(Graph* graph, int from, int to) {
    if (!graph) return false;

    Node *from_node = find_node(graph, from);
    if (!from_node) return false;

    Node *to_node = find_node(graph, to);
    if (!to_node) return false;

    for (size_t i = 0; i < from_node->neighbor_count - 1; i++) {
        if (from_node->neighbors[i].node_id == to) {
            from_node->neighbors[i] = from_node->neighbors[i + 1];

            from_node->neighbor_count--;
            return true;
        }
    }

    return false;
};


bool graph_remove_node(Graph* graph, int node_id) {
    if (!graph) return false;

    Node* target_node = find_node(graph, node_id);
    if (!target_node) return false;

    // Remove all edges to this node from other nodes
    for (size_t i = 0; i < graph->node_count; i++) {
        Node* current_node = &graph->nodes[i];

        if (current_node->id == node_id) {
            // Free target node's neighbors
            free(current_node->neighbors);

            for (size_t j = i; j < graph->node_count - 1; j++) {
                graph->nodes[j] = graph->nodes[j + 1];
            }
            graph->node_count--;
        }

        for (size_t j = 0; j < current_node->neighbor_count; j++) {
            if (current_node->neighbors[j].node_id == node_id) {
                for (size_t k = j; k < current_node->neighbor_count - 1; k++) {
                    current_node->neighbors[k] = current_node->neighbors[k + 1];
                }
                current_node->neighbor_count--;

                break;
            }
        }
    }

    return true;
}
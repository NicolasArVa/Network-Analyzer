#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../include/core/graph.h"
#include "../../include/utils/hash_table_utils.h"
#include "../../include/utils/graph_build_utils.h"


// Helper: find Node by ID (Hash table lookup)
Node* find_node(Graph* graph, int node_id) {
    unsigned int index = hash(node_id, graph->node_capacity);

    Node *current = graph->nodes[index];

    while (current) {
        if (current->id == node_id) return current;  // found
        current = current->next;
    }
    return NULL;  // not found
}

// Helper: detect if edge exists
// TODO: look for a faster method
bool edge_exists(Node* from, int to) {
    for (size_t i = 0; i < from->neighbor_count; i++) {
        if (from->neighbors[i].node_id == to) return true;
    }
    return false;
}

// Helper: detect if graph needs to be resized
bool graph_needs_resize(Graph* graph) {
    return (graph->node_count >= ALPHA * graph->node_capacity);
}

//Helper: resize nodes array
bool graph_resize (Graph* graph) {
        size_t new_capacity = graph->node_capacity * 2; // * 1.5 to reset the table alpha to 0.5

        Node **new_nodes = calloc(new_capacity, sizeof(Node*));
        if (!new_nodes) {
            fprintf(stderr, "Failed to resize graph's nodes array\n");
            return false;
        }

        for (size_t i = 0; i < graph->node_capacity; i++) {
            Node *bucket = graph->nodes[i];

            while (bucket) {
                Node *head = remove_first(&bucket);
                add_to_hash_table(head, new_capacity, new_nodes);
            }
        }

        free(graph->nodes);
        graph->nodes = new_nodes;
        graph->node_capacity = new_capacity;

        printf("Graph successfully resized\n");
        return true;
}

bool node_needs_resize(Node* node) {
    return (node->neighbor_count >= node->capacity);
}

bool node_resize(Node* node) {
    size_t new_capacity = node->capacity * 2;
    EdgeNode* new_neighbors = realloc(node->neighbors, new_capacity * sizeof(EdgeNode));
    if (!new_neighbors) {
        fprintf(stderr, "Failed to resize node's neighbors array\n");
        return false;
    }

    node->neighbors = new_neighbors;
    node->capacity = new_capacity;
    return true;
}

// Helper: Create node
Node* create_node(int node_id, int node_capacity) {
    // Allocate memory for new node
    Node* new_node = malloc(sizeof(Node));
    if (!new_node) {
        fprintf(stderr, "Failed to create new node\n");
        return NULL;
    }

    new_node->id = node_id;
    new_node->neighbors = NULL;
    new_node->neighbor_count = 0;
    new_node->capacity = node_capacity;
    new_node->next = NULL;

    new_node->neighbors = calloc(node_capacity, sizeof(EdgeNode)); // Initialize neighbors array
    if (!new_node->neighbors) {
        fprintf(stderr, "Failed to initialize new node's neighbors\n");
        free(new_node);
        return NULL;
    }

    return new_node;
}


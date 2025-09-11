#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../include/core/graph.h"
#include "../../include/utils/hash_table_utils.h"
#include "../../include/utils/graph_build_utils.h"

//Helper: resize nodes array
bool graph_resize (Graph* graph) {
    size_t new_capacity = graph->node_capacity * 2; // * 1.5 to reduce the hash table alpha to 0.5

    Node **new_nodes = calloc(new_capacity, sizeof(Node*));
    if (!new_nodes) {
        fprintf(stderr, "Failed to allocate memory for new node table\n");
        return false;
    }

    for (size_t i = 0; i < graph->node_capacity; i++) {
        Node *bucket = graph->nodes[i];

        while (bucket) {
            Node *head = pop_bucket(&bucket);  // head is never NULL due to loop stopping condition
            bool success = add_to_hash_table(head, new_capacity, new_nodes);  // function has own error logs
            if (!success) return false;
        }
    }

    free(graph->nodes);
    graph->nodes = new_nodes;
    graph->node_capacity = new_capacity;

    return true;
}

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

Node* create_node(int node_id, int node_capacity) {
    // Allocate memory for new node
    Node* new_node = malloc(sizeof(Node));
    if (!new_node) {
        fprintf(stderr, "Failed to allocate memory for new node\n");
        return NULL;
    }
    
    new_node->id = node_id;
    new_node->neighbor_count = 0;
    new_node->capacity = node_capacity;
    new_node->next = NULL;
    
    // Initialize neighbors array
    new_node->neighbors = calloc(node_capacity, sizeof(EdgeNode));
    if (!new_node->neighbors) {
        fprintf(stderr, "Failed to initialize new node's neighbors, node will not be created\n");
        free(new_node);
        return NULL;
    }
    
    return new_node;
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

bool node_add_edge(Node* node, int to, double weight){    
    // Check if node needs to be resized
    switch (node_needs_resize(node)) {
        case OK_TRUE:
            bool success = node_resize(node);  // function has own error logs
            if (!success) return false;
            break;
            
        case OK_FALSE:
            break;

        case ERROR:
            return false;
        
    }

    // Add new edge/neighbor
    EdgeNode* new_neighbor = &node->neighbors[node->neighbor_count];
    new_neighbor->node_id = to;
    new_neighbor->weight = weight;
    node->neighbor_count ++;

    return true;
}

bool node_edit_edge(Node* node, int to, double weight) {
    CHECK_EXISTS(node, false, "Node not initialized");

    for (size_t i = 0; i < node->neighbor_count; i++) {
        if (node->neighbors[i].node_id == to) {
            node->neighbors[i].weight = weight;
            return true;
        }
    }

    return false;
}

bool node_remove_edge(Node* node, int to) {
    CHECK_EXISTS(node, false, "Node not initialized");

    for (size_t i = 0; i < node->neighbor_count; i++) {
        if (node->neighbors[i].node_id == to) {
            for (size_t j = i; j < node->neighbor_count - 1; j++) {
                node->neighbors[j] = node->neighbors[j + 1];
            }

            node->neighbor_count--;
            return true;
        }
    }

    fprintf(stderr, "Edge from node %d to node %d does not exist\n", node->id, to);
    return false;
}

bool edge_exists(Node* node, int to) {
    CHECK_EXISTS(node, NULL, "Node not initialized");

    for (size_t i = 0; i < node->neighbor_count; i++) {
        if (node->neighbors[i].node_id == to) {
            return true;
        }
    }

    return false;
}
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "core/graph_build.h"
#include "utils/general_utils.h"
#include "utils/hash_table_utils.h"
#include "utils/graph_build_utils.h"

// TODO: change bool to -1, 0, 1

// * Safety checks are carried out before the functions are called

//Helper: resize nodes array
Status graph_resize (Graph* graph) {
    if (!graph) {
        fprintf(stderr, "Fatal error: Graph is corrupted\n");
        return STATUS_ERROR;
    }
    
    size_t new_capacity = graph->node_capacity * 2; // * 1.5 to reduce the hash table alpha to 0.5
    Node **new_nodes = calloc(new_capacity, sizeof(Node*));
    if (!new_nodes) {
        fprintf(stderr, "Error: Failed to rezize graph, keeping previous capacity\n");
        return STATUS_WARNING;
    }

    for (size_t i = 0; i < graph->node_capacity; i++) {
        Node *bucket = graph->nodes[i];

        while (bucket) {
            Node *head = pop_bucket(&bucket);
            switch(add_to_hash_table(head, new_capacity, new_nodes)) {
                case STATUS_SUCCESS:
                case STATUS_WARNING:
                    break;
                case STATUS_ERROR: // add_to_hash_table has own error messages
                    // free new nodes
                    free(new_nodes);
                    return STATUS_ERROR;
            }
        }
    }
    
    int *new_id_array = realloc(graph->node_ids, new_capacity * sizeof(int));
    if (!new_id_array) {
        fprintf(stderr, "Error: Failed to rezize graph, keeping previous capacity\n");
        free(new_nodes);
        return STATUS_ERROR;
    }
    
    free(graph->nodes);
    graph->nodes = new_nodes;
    graph->node_ids = new_id_array;
    graph->node_capacity = new_capacity;

    return STATUS_SUCCESS;
}

// Helper: find Node by ID (Hash table lookup)
Node* find_node(Graph* graph, int node_id) {
    if (!graph) return NULL;
    unsigned int index = hash(node_id, graph->node_capacity);
    Node *current = graph->nodes[index];

    while (current) {
        if (current->id == node_id) return current;  // found
        current = current->next;
    }
    
    return NULL;  // not found
}

Node* create_node(int node_id, size_t neighbor_capacity) {
    // Allocate memory for new node
    Node* new_node = malloc(sizeof(Node));
    if (!new_node) {
        fprintf(stderr, "Error: Failed to initialize new node, keeping previous capacity\n");
        return NULL;
    }
    
    new_node->id = node_id;
    new_node->neighbor_count = 0;
    new_node->neighbor_capacity = neighbor_capacity;
    new_node->next = NULL;
    
    // Initialize neighbors array
    new_node->neighbors = malloc(neighbor_capacity * sizeof(EdgeNode));
    if (!new_node->neighbors) {
        fprintf(stderr, "Error: Failed to initialize new node\n");
        free(new_node);
        return NULL;
    }

    // Initialize neighbors with sentinel values
    for ( size_t i = 0; i < neighbor_capacity; i++) {
        new_node->neighbors[i].node_id = -1;
        new_node->neighbors[i].weight = NAN;
    }
    
    return new_node;
}

Status node_resize(Node* node) {
    if (!node) {
        fprintf(stderr, "Fatal error: Node is corrupted\n");
        return STATUS_ERROR;
    }

    size_t new_capacity = node->neighbor_capacity * 2;
    EdgeNode* new_neighbors = realloc(node->neighbors, new_capacity * sizeof(EdgeNode));
    if (!new_neighbors) {
        fprintf(stderr, "Error: Failed to resize node\n");
        return STATUS_WARNING;
    }

    // Initialize new neighbors with sentinel values
    for (size_t i = node->neighbor_capacity; i < new_capacity; i++) {
        new_neighbors[i].node_id = -1;
        new_neighbors[i].weight = NAN;
    }

    node->neighbors = new_neighbors;
    node->neighbor_capacity = new_capacity;
    return STATUS_SUCCESS;
}

Status node_add_edge(Node* node, int to, double weight, double* old_weight, bool update) {
    if (!node) {
        fprintf(stderr, "Fatal error: Node is corrupted\n");
        return STATUS_ERROR;
    }

    // Check if edge already exists
    if (update) {
        for (size_t i = 0; i < node->neighbor_count; i++) {
            if (node->neighbors[i].node_id == to) {
                if (old_weight) *old_weight = node->neighbors[i].weight;
                node->neighbors[i].weight = weight;
                return STATUS_SUCCESS;
            }
        }      

    } else {
        // Check if edge already exists
        for (size_t i = 0; i < node->neighbor_count; i++) {
            if (node->neighbors[i].node_id == to) {
                printf("Edge from node %d to node %d already exists\n", node->id, to);
                return STATUS_WARNING;
            }
        }
    }

    // Check if node needs to be resized
    if (node_needs_resize(node)) {
        switch (node_resize(node)) {
            case STATUS_SUCCESS:
            break;
            case STATUS_WARNING:
                return STATUS_WARNING;
            case STATUS_ERROR:
                return STATUS_ERROR;
        }
    }

    // Add new edge/neighbor
    EdgeNode* new_neighbor = &node->neighbors[node->neighbor_count];
    new_neighbor->node_id = to;
    new_neighbor->weight = weight;
    node->neighbor_count ++;

    return STATUS_SUCCESS;
}

Status node_remove_edge(Node* node, int to, double* old_weight) {
    if (!node) {
        fprintf(stderr, "Fatal error: Node is corrupted\n");
        return STATUS_ERROR;
    }

    for (size_t i = 0; i < node->neighbor_count; i++) {
        if (node->neighbors[i].node_id == to) {
            if (old_weight) *old_weight = node->neighbors[i].weight;

            size_t num_to_move = node->neighbor_count - i - 1;
            if (num_to_move > 0) {
                memmove(&node->neighbors[i], 
                    &node->neighbors[i + 1], 
                    num_to_move * sizeof(EdgeNode)); // shifts remaining elements to the left
            }
            
            // decrement neighbor count
            node->neighbor_count--;
            // * OPTIONAL: set sentinel value for cleaner debug
            node->neighbors[node->neighbor_count].node_id = -1;   // invalid id
            node->neighbors[node->neighbor_count].weight = NAN;   // invalid weight
            return STATUS_SUCCESS;
        }
    }

    fprintf(stderr, "Error: Edge from node %d to node %d does not exist\n", node->id, to);
    return STATUS_WARNING;
}
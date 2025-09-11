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

Node* create_node(int node_id, size_t neighbor_capacity) {
    // Allocate memory for new node
    Node* new_node = malloc(sizeof(Node));
    if (!new_node) {
        fprintf(stderr, "Failed to allocate memory for new node\n");
        return NULL;
    }
    
    new_node->id = node_id;
    new_node->neighbor_count = 0;
    new_node->neighbor_capacity = neighbor_capacity;
    new_node->next = NULL;
    
    // Initialize neighbors array
    new_node->neighbors = malloc(neighbor_capacity * sizeof(EdgeNode));
    if (!new_node->neighbors) {
        fprintf(stderr, "Failed to initialize new node's neighbors, node will not be created\n");
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

bool node_resize(Node* node) {
    size_t new_capacity = node->neighbor_capacity * 2;
    EdgeNode* new_neighbors = realloc(node->neighbors, new_capacity * sizeof(EdgeNode));
    if (!new_neighbors) {
        fprintf(stderr, "Failed to resize node's neighbors array\n");
        return false;
    }

    // Initialize new neighbors with sentinel values
    for (size_t i = node->neighbor_capacity; i < new_capacity; i++) {
        new_neighbors[i].node_id = -1;
        new_neighbors[i].weight = NAN;
    }

    node->neighbors = new_neighbors;
    node->neighbor_capacity = new_capacity;
    return true;
}

bool node_add_edge(Node* node, int to, double weight, double* old_weight, bool update) {
    // Check if edge already exists
    if (update) {
        for (size_t i = 0; i < node->neighbor_count; i++) {
            if (node->neighbors[i].node_id == to) {
                if (old_weight) *old_weight = node->neighbors[i].weight;
                node->neighbors[i].weight = weight;
                return true;
            }
        }      

    } else {
        for (size_t i = 0; i < node->neighbor_count; i++) {
            if (node->neighbors[i].node_id == to) {
                fprintf(stderr, "Edge from node %d to node %d already exists\n", node->id, to);
                return false;
            }
        }
    }

    // Check if node needs to be resized
    if (node_needs_resize(node)) {
        bool success = node_resize(node);  // function has own error logs
        if (!success) return false;
        
    }

    // Add new edge/neighbor
    EdgeNode* new_neighbor = &node->neighbors[node->neighbor_count];
    new_neighbor->node_id = to;
    new_neighbor->weight = weight;
    node->neighbor_count ++;

    return true;
}

bool node_remove_edge(Node* node, int to, double* old_weight) {
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
            return true;
        }
    }

    fprintf(stderr, "Edge from node %d to node %d does not exist\n", node->id, to);
    return false;
}
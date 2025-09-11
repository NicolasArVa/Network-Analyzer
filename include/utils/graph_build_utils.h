#ifndef GRAPH_BUILD_UTILS_H
#define GRAPH_BUILD_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../include/core/graph.h"
#include "../../include/utils/hash_table_utils.h"


// Helper macro: check if elements exist allowing for custom error messages
#define CHECK_EXISTS(ptr, value, fmsg, ...)                 \
    do {                                                    \
        if (!(ptr)) {                                       \
            fprintf(stderr, fmsg "\n", ##__VA_ARGS__);      \ 
            return value;                                   \
        }                                                   \
    } while (0)

//  Custom bool to handle errors
typedef enum {
    ERROR = -1,
    OK_FALSE = 0,
    OK_TRUE = 1
} CustomBool;

// Helpers: resize detection
static inline CustomBool graph_needs_resize(Graph* graph) {
    CHECK_EXISTS(graph, ERROR, "Graph not initiallized");

    // Require hash table utils
    if (graph->node_count >= ALPHA * graph->node_capacity) {
        return OK_TRUE;
    } else {
        return OK_FALSE;
    }
}

static inline CustomBool node_needs_resize(Node* node) {
    CHECK_EXISTS(node, ERROR, "Node not initiallized");
    if (node->neighbor_count >= node->capacity) {
        return OK_TRUE;
    } else {
        return OK_FALSE;
    }
}


// Graph related utils
Node* find_node(Graph* graph, int node_id);
bool graph_resize (Graph* graph);

// Node related utils
Node* create_node(int node_id, int node_capacity);
bool node_add_edge(Node* node, int to, double weight);
bool node_edit_edge(Node* node, int to, double weight);
bool node_remove_edge(Node* node, int to);

bool node_resize(Node* node);

// Edge related utils
bool edge_exists(Node* from, int to);

#endif GRAPH_BUILD_UTILS_H
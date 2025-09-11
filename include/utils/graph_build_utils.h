#ifndef GRAPH_BUILD_UTILS_H
#define GRAPH_BUILD_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../include/core/graph_build.h"
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
static inline bool graph_needs_resize(Graph* graph) {
    // Require hash table
    return (graph->node_count >= ALPHA * graph->node_capacity);
}

static inline bool node_needs_resize(Node* node) {
    return (node->neighbor_count >= node->neighbor_capacity);
}


// Graph related utils
bool graph_resize (Graph* graph);
Node* find_node(Graph* graph, int node_id);

// Node related utils
Node* create_node(int node_id, size_t neighbor_capacity);
bool node_resize(Node* node);

bool node_add_edge(Node* node, int to, double weight, double* old_weight, bool overwrite);
bool node_remove_edge(Node* node, int to, double* old_weight);


#endif GRAPH_BUILD_UTILS_H
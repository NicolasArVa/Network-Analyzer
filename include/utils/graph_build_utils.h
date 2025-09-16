#ifndef GRAPH_BUILD_UTILS_H
#define GRAPH_BUILD_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utils/hash_table_utils.h"
#include "utils/general_utils.h"
#include "core/graph_build.h"

# define CHECK_GRAPH \
    do {    if (!graph) {\
        fprintf(stderr, "Error: Invalid graph passed to %s function call\n", __func__);\
        return STATUS_INVALID;\
    }\
} while(0);


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


#endif
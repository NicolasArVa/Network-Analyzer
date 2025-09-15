#ifndef GRAPH_OPS_H
#define GRAPH_OPS_H

#include <stddef.h>
#include <stdbool.h>
#include "utils/general_utils.h"
#include "core/graph_build.h"

// Basic properties getters
// TODO: change bool to -1, 0, 1

bool graph_is_directed(const Graph* graph);     
size_t graph_node_count(const Graph* graph);
size_t graph_edge_count(const Graph* graph);

#endif
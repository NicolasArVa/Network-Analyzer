#ifndef GRAPH_OPS_H
#define GRAPH_OPS_H

#include <stddef.h>
#include <stdbool.h>
#include "core/graph_build.h"

// Basic properties getters
size_t graph_node_count(const Graph* graph);
size_t graph_edge_count(const Graph* graph);

#endif
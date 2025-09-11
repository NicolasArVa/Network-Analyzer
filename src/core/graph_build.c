#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../include/core/graph.h"
#include "../../include/utils/hash_table_utils.h"
#include "../../include/utils/graph_build_utils.h"

# define INITIAL_CAPACITY 4

// Basic graph operations
Graph* graph_create(GraphType type, size_t initial_capacity, float alpha) {
    Graph *graph = malloc(sizeof(Graph));
    if (!graph) {
        fprintf(stderr, "Failed to allocate memory for graph\n");
        return NULL;
    };

    if (initial_capacity <= 0) initial_capacity = INITIAL_CAPACITY;

    graph->type = type;
    graph->nodes = calloc(initial_capacity, sizeof(Node*));
    if (!graph->nodes) {
        fprintf(stderr, "Failed to allocate memory for nodes array while creating graph\n");
        free(graph);
        return NULL;
    };
    graph->node_count = 0;
    graph->node_capacity = initial_capacity;

    return graph;
}

void graph_destroy(Graph* graph) {
    CHECK_EXISTS(graph, NULL, "Graph not initialized");

    for (size_t i = 0; i < graph->node_capacity; i++) {
        Node *current = graph->nodes[i];

        while (current) {
            Node *next = current->next;

            // Free each node's edges
            free(current->neighbors);
            // Free current node
            free(current);

            current = next;
        }
    }

    free(graph->nodes);
    free(graph);
}

bool graph_add_node(Graph* graph, int node_id, int node_capacity) {
    CHECK_EXISTS(graph, false, "Graph not initialized");

    // Check if node already exists in graph
    Node *node = find_node(graph, node_id);
    if (node) {
        fprintf(stderr, "A node with ID %d already exists in the graph\n", node_id);
        return false;
    }

    // Check if nodes arrays needs rezising
    if (graph_needs_resize(graph)) {
        bool success = graph_resize(graph);  // function has own error logs
        if (!success) return false;
    }

    // Initialize new node
    if (node_capacity <= 0) node_capacity = INITIAL_CAPACITY;
    Node* new_node = create_node(node_id, node_capacity);  // function has own error logs
    if (!new_node) return false;
    
    bool success = add_to_hash_table(new_node, graph->node_capacity, graph->nodes);  // function has own error logs
    if (!success) {
        fprintf(stderr, "Failed to add new node to the graph\n");
        free(new_node->neighbors);
        free(new_node);
        return false;
    }

    graph->node_count++;
    return true;
}

bool graph_remove_node(Graph* graph, int node_id) {
    CHECK_EXISTS(graph, false, "Graph not initialized");
    
    Node* node = find_node(graph, node_id);
    if (!node) {
        fprintf(stderr, "A node with ID %d does not exist in the graph\n", node_id);
        return false;
    }
    
    // Remove all edges that pointto this node from other nodes
    if (graph->type == GRAPH_UNDIRECTED) {
        for (size_t i = 0; i < node->neighbor_count; i++) {
            EdgeNode edge = node->neighbors[i];

            Node *current = find_node(graph, edge.node_id);  // quite sure this won't return NULL due how edges are added
            if (current) node_remove_edge(current, node_id);  // just in case 
        }
    } else {
        for (size_t i = 0; i < graph->node_capacity; i++) {
            Node *current = graph->nodes[i];

            while (current) {
                node_remove_edge(current, node_id);
                current = current->next;
            }
        }
    }

    // Remove node from node table (hash table)
    bool success = delete_from_hash_table(node_id, graph->node_capacity, graph->nodes);  // function has own error logs
    if (!success) return false;
    
    graph->node_count--;
    return true;
}

bool graph_add_edge(Graph* graph, int from, int to, double weight) {
    CHECK_EXISTS(graph, false, "Graph not initialized");

    Node* from_node = find_node(graph, from);
    Node* to_node = find_node(graph, to);
    if (!from_node || !to_node) {
        fprintf(stderr, "A node with ID %d or %d does not exist in the graph\n", from, to);
        return false;
    }
    
    if (edge_exists(from_node, to)) {
        fprintf(stderr, "Edge from node %d to node %d already exists\n", from, to);
        return false;
    }

    bool success = node_add_edge(from_node, to, weight);
    if (!success) {
        fprintf(stderr, "Failed to add edge from node %d to node %d\n", from, to);
        return false;
    }
    
    // For undirected graphs, add reverse edge
    if (graph->type == GRAPH_UNDIRECTED) {
        success = node_add_edge(to_node, from, weight);  // checks if edge already exists
        if (!success) {
            // Remove forward edge
            fprintf(stderr, "Failed to add reverse edge from node %d to node %d\n", to, from);
            node_remove_edge(from_node, to); // edge should exist at this point
            return false;
        }
    }

    return true;
}

bool graph_edit_edge(Graph* graph, int from, int to, double weight){
    if (!graph_exists(graph)) return false;

    Node* from_node = find_node(graph, from);
    Node* to_node = find_node(graph, to);
    if (!from_node || !to_node) return false;

    for (size_t i = 0; i < from_node->neighbor_count; i++) {
        if (from_node->neighbors[i].node_id == to) {
            from_node->neighbors[i].weight = weight;
            return true;
        }
    }

    fprintf(stderr, "Edge from node %d to node %d does not exist\n", from, to);
    return false;
}

bool graph_remove_edge(Graph* graph, int from, int to) {
    if (!graph_exists(graph)) return false;

    Node *from_node = find_node(graph, from);
    Node *to_node = find_node(graph, to);
    if (!from_node || !to_node) return false;

    for (size_t i = 0; i < from_node->neighbor_count - 1; i++) {
        if (from_node->neighbors[i].node_id == to) {
            from_node->neighbors[i] = from_node->neighbors[i + 1];

            from_node->neighbor_count--;
        }
    }

    return true;
}
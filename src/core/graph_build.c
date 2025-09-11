#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "core/graph_build.h"
#include "utils/hash_table_utils.h"
#include "utils/graph_build_utils.h"

# define INITIAL_CAPACITY 4

// Basic graph operations
Graph* graph_create(GraphType type, size_t initial_capacity) {
    Graph *graph = malloc(sizeof(Graph));
    if (!graph) {
        fprintf(stderr, "Failed to allocate memory for graph\n");
        return NULL;
    };

    if (initial_capacity == 0) initial_capacity = INITIAL_CAPACITY;

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
    if (!graph) return;

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

bool graph_insert_node(Graph* graph, int node_id, size_t node_capacity) {
    CHECK_EXISTS(graph, false, "%s", "Graph not initialized");

    // Check if node already exists in graph
    Node *node = find_node(graph, node_id);
    CHECK_EXISTS(!node, false, "A node with ID %d already exists in the graph", node_id);

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
    CHECK_EXISTS(graph, false, "%s", "Graph not initialized");
    
    Node* node = find_node(graph, node_id);
    CHECK_EXISTS(node, false, "A node with ID %d does not exist in the graph", node_id);
    
    // Remove all edges that pointto this node from other nodes
    if (graph->type == GRAPH_UNDIRECTED) {
        bool success = false;
        for (size_t i = 0; i < node->neighbor_count; i++) {
            EdgeNode edge = node->neighbors[i];

            Node *current = find_node(graph, edge.node_id);  // quite sure this won't return NULL due to edges only point to existing nodes
            if (current) success = node_remove_edge(current, node_id, NULL);  // just in case 
            if (!success) {
                fprintf(stderr, "Failed to remove edge between nodes %d-%d, removal of node %d incomplete\n", edge.node_id, node_id, node_id);
                fprintf(stderr, "Fatal error: Undirected graph corrupted, it might contain asimmetric edges\n");
                return false;
            }
        }
    } else {
        for (size_t i = 0; i < graph->node_capacity; i++) {
            Node *current = graph->nodes[i];

            while (current) {
                (void) node_remove_edge(current, node_id, NULL);
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

bool graph_insert_edge(Graph* graph, int from, int to, double weight) {
    CHECK_EXISTS(graph, false, "%s", "Graph not initialized");

    Node* from_node = find_node(graph, from);
    CHECK_EXISTS(from_node, false, "A node with ID %d does not exist in the graph", from);

    Node* to_node = find_node(graph, to);
    CHECK_EXISTS(to_node, false, "A node with ID %d does not exist in the graph", to);

    bool success = node_add_edge(from_node, to, weight, NULL, false);
    CHECK_EXISTS(success, false, "Failed to add edge %d->%d", from, to);
    
    // For undirected graphs, add reverse edge
    if (graph->type == GRAPH_UNDIRECTED) {
        success = node_add_edge(to_node, from, weight, NULL, false);
        if (success) return true;

        // Undo addition of forward edge
        success = node_remove_edge(from_node, to, NULL);
        if (!success) {
            fprintf(stderr,
                "Fatal error: Undirected graph has been corrupted. " 
                "rollback failed to revert forward edge %d->%d\n", from, to);
                return false;
        }
    }

    fprintf(stderr, "Failed to add edge between nodes %d and %d\n", from, to);
    return false;
}

bool graph_update_edge(Graph* graph, int from, int to, double weight){
    CHECK_EXISTS(graph, false, "%s", "Graph not initialized");

    Node* from_node = find_node(graph, from);
    CHECK_EXISTS(from_node, false, "A node with ID %d does not exist in the graph", from);

    Node* to_node = find_node(graph, to);
    CHECK_EXISTS(to_node, false, "A node with ID %d does not exist in the graph", to);

    bool success = node_add_edge(from_node, to, weight, NULL, true);
    CHECK_EXISTS(success, false, "Failed to edit edge %d->%d", from, to);

    // For undirected graphs, edit reverse edge
    if (graph->type == GRAPH_UNDIRECTED) {
        double old_weight = 0.0;
        success = node_add_edge(to_node, from, weight, &old_weight, true);
        if (success) return true;

        // Undo changes to forward edge
        success = node_add_edge(from_node, to, old_weight, NULL, true);
        if (!success) {
            fprintf(stderr, 
                "Fatal error: Undirected graph has been corrupted. "
                "rollback failed to restore forward edge %d->%d\n", from, to);
            return false;
        }
    }

    fprintf(stderr, "Failed to edit edge between nodes %d and %d\n", from, to);
    return false;       
}

bool graph_remove_edge(Graph* graph, int from, int to) {
    CHECK_EXISTS(graph, false, "%s", "Graph not initialized");

    Node* from_node = find_node(graph, from);
    CHECK_EXISTS(from_node, false, "A node with ID %d does not exist in the graph", from);

    Node* to_node = find_node(graph, to);
    CHECK_EXISTS(to_node, false, "A node with ID %d does not exist in the graph", to);

    bool success = node_remove_edge(from_node, to, NULL);
    CHECK_EXISTS(success, false, "Failed to remove edge %d->%d", from, to);

    // For undirected graphs, edit reverse edge
    if (graph->type == GRAPH_UNDIRECTED) {
        double old_weight = 0.0;
        success = node_remove_edge(to_node, from, &old_weight);
        if (success) return true;

        // Undo removal of forward edge
        success = node_add_edge(from_node, to, old_weight, NULL, true);
        if (!success) {
            fprintf(stderr, 
                "Fatal error: Undirected graph has been corrupted. "
                "rollback failed to restore forward edge %d->%d\n", from, to);
            return false;
        }
    }

    fprintf(stderr, "Failed to remove edge between nodes %d and %d\n", from, to);
    return false;
}
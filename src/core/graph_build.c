#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "core/graph_build.h"
#include "utils/general_utils.h"
#include "utils/hash_table_utils.h"
#include "utils/graph_build_utils.h"

# define INITIAL_CAPACITY 4
# define CHECK_GRAPH \
    do {    if (!graph) {\
        fprintf(stderr, "Fatal error: Graph is corrupted in %s function call\n", __func__);\
        return STATUS_ERROR;\
    }\
} while(0);

// Basic graph operations
Graph* graph_create(GraphType type, size_t initial_capacity) {
    Graph *graph = malloc(sizeof(Graph));
    if (!graph) {
        fprintf(stderr, "Fatal error: Failed to initialize graph\n");
        return NULL;
    }

    if (initial_capacity == 0) initial_capacity = INITIAL_CAPACITY;

    graph->type = type;
    graph->node_count = 0;
    graph->node_capacity = initial_capacity;

    graph->node_ids = calloc(initial_capacity, sizeof(int));
    if (!graph->node_ids) {
        fprintf(stderr, "Fatal error: Failed to initialize node id array while creating graph\n");
        free(graph);
        return NULL;
    };

    graph->nodes = calloc(initial_capacity, sizeof(Node*));
    if (!graph->nodes) {
        fprintf(stderr, "Fatal error: Failed to initialize node array while creating graph\n");
        free(graph);
        free(graph->node_ids);
        return NULL;
    };

    // initialize id node array
    for (size_t i = 0; i < initial_capacity; i++) graph->node_ids[i] = -1;

    return graph;
}

Status graph_destroy(Graph* graph) {
    CHECK_GRAPH

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
    free(graph->node_ids);
    free(graph);

    return STATUS_SUCCESS;
}

Status graph_insert_node(Graph* graph, int node_id, size_t node_capacity) {
    CHECK_GRAPH

    // Check if node already exists in graph
    Node *node = find_node(graph, node_id);
    if (node) {
        #ifdef DEBUG
        printf("A node with ID %d already exists in the graph", node_id);
        #endif
        return STATUS_WARNING;
    }

    // Check if nodes arrays needs rezising
    if (graph_needs_resize(graph)) {
        switch (graph_resize(graph)) {
            case STATUS_SUCCESS:
                break;
            case STATUS_WARNING:
                return STATUS_WARNING;
            case STATUS_ERROR:
                return STATUS_ERROR;
        }
    }

    // Initialize new node
    if (node_capacity <= 0) node_capacity = INITIAL_CAPACITY;
    Node* new_node = create_node(node_id, node_capacity);
    if (!new_node) {
        // * create_node function has own error logs
        return STATUS_WARNING;
    }

    switch(add_to_hash_table(new_node, graph->node_capacity, graph->nodes)) {
        case STATUS_SUCCESS:
            break;
        case STATUS_WARNING:
            // add_to_hash_table warning means node already exists, should never happen due to previous check
            free(new_node->neighbors);
            free(new_node);
            return STATUS_WARNING;
        case STATUS_ERROR:
            return STATUS_ERROR;
    }

    graph->node_ids[graph->node_count] = node_id;
    graph->node_count++;
    return STATUS_SUCCESS;
}

Status graph_remove_node(Graph* graph, int node_id) {
    CHECK_GRAPH
    
    // Check if node exists
    Node* node = find_node(graph, node_id);
    if (!node) {
        #ifdef DEBUG
        printf("A node with ID %d does not exist in the graph\n", node_id);
        #endif
        return STATUS_WARNING;
    }
    
    // Remove all edges that pointto this node from other nodes
    if (graph->type == GRAPH_UNDIRECTED) {
        for (size_t i = 0; i < node->neighbor_count; i++) {
            EdgeNode edge = node->neighbors[i];
            Node *current = find_node(graph, edge.node_id);  // quite sure this won't return NULL due to edges only point to existing nodes
            if (!current) {
                fprintf(stderr, "Fatal error: Undirected graph has been corrupted. Edge points to non-existing node %d\n", edge.node_id);
                return STATUS_ERROR;
            }
            
            switch (node_remove_edge(current, node_id, NULL)) {
                case STATUS_SUCCESS:
                    break;
                case STATUS_WARNING:
                    // In an undirected graph, every node's neighbor should point back to it.
                    // if it doesn't, the graph is corrupted.
                    fprintf(stderr, "Warning: Directed edge node %d->%d, while removing node %d\n", edge.node_id, node_id, node_id);
                    fprintf(stderr, "Fatal error: Undirected graph has been corrupted\n");
                    return STATUS_ERROR;
                case STATUS_ERROR:
                    return STATUS_ERROR;
            }
        }
    } else {
        for (size_t i = 0; i < graph->node_capacity; i++) {
            Node *current = graph->nodes[i];

            while (current) {
                switch (node_remove_edge(current, node_id, NULL)) {
                    case STATUS_SUCCESS:
                    case STATUS_WARNING:
                        current = current->next;
                        break;
                    case STATUS_ERROR:
                        return STATUS_ERROR;
                }
            }
        }
    }

    // Remove node from node table (hash table)
    switch (delete_from_hash_table(node_id, graph->node_capacity, graph->nodes)) {
        case STATUS_SUCCESS:
            break;
        case STATUS_WARNING:
            // should never happen due to previous check, just for completeness
            // if node removal fails, while the edges have been removed, the graph has been corrupted
            fprintf(stderr, "Fatal error: Graph has been corrupted\n");
            return STATUS_ERROR;
        case STATUS_ERROR:
            return STATUS_ERROR;
    }

    // Remove node ID from node_ids array
    for (size_t i = 0; i < graph->node_count; i++) {
        if (graph->node_ids[i] == node_id) {
            memmove(&graph->node_ids[i], &graph->node_ids[i + 1], (graph->node_count - i - 1) * sizeof(int));
            graph->node_count--;
            // * OPTIONAL: set sentinel value for cleaner debug
            graph->node_ids[graph->node_count] = -1;
            break;
        }
    }
    
    return STATUS_SUCCESS;
}

Status graph_insert_edge(Graph* graph, int from, int to, double weight) {
    CHECK_GRAPH

    Node* from_node = find_node(graph, from);
    if (!from_node) {
        fprintf(stderr, "Warning: A node with ID %d does not exist in the graph", from);
        return STATUS_WARNING;
    }

    Node* to_node = find_node(graph, to);
    if (!to_node) {
        fprintf(stderr, "Warning: A node with ID %d does not exist in the graph", to);
        return STATUS_WARNING;
    }

    switch(node_add_edge(from_node, to, weight, NULL, false)) {
        case STATUS_SUCCESS:
            break;
        case STATUS_WARNING:
            // will throw warning if edge already exists or if resizing fails
            return STATUS_WARNING;
        case STATUS_ERROR:
            return STATUS_ERROR;
    }
    
    // For undirected graphs, add reverse edge
    if (graph->type == GRAPH_UNDIRECTED) {
        switch (node_add_edge(to_node, from, weight, NULL, false)) {
            case STATUS_SUCCESS:
                return STATUS_SUCCESS;
            case STATUS_WARNING:
            case STATUS_ERROR:
                break;
        }

        // Undo addition of forward edge
        switch (node_remove_edge(from_node, to, NULL)) {
            case STATUS_SUCCESS:
                break;
            case STATUS_WARNING:
            case STATUS_ERROR:
                fprintf(stderr,
                    "Fatal error: Undirected graph has been corrupted. " 
                    "rollback failed to revert forward edge %d->%d\n", from, to);
                return STATUS_ERROR;

        }
    } else {
        return STATUS_SUCCESS;
    }

    fprintf(stderr, "Error: Failed to add edge between nodes %d and %d\n", from, to);
    return STATUS_WARNING;
}


// TODO: implement Status instead of bool
Status graph_update_edge(Graph* graph, int from, int to, double weight){
    CHECK_GRAPH

    Node* from_node = find_node(graph, from);
    if (!from_node) {
        fprintf(stderr, "Warning: A node with ID %d does not exist in the graph", from);
        return STATUS_WARNING;
    }

    Node* to_node = find_node(graph, to);
    if (!to_node) {
        fprintf(stderr, "Warning: A node with ID %d does not exist in the graph", to);
        return STATUS_WARNING;
    }

    switch (node_add_edge(from_node, to, weight, NULL, true)) {
        case STATUS_SUCCESS:
            break;
        case STATUS_WARNING:
            // will throw warning if edge does not exist
            return STATUS_WARNING;
        case STATUS_ERROR:
            return STATUS_ERROR;
    }

    // For undirected graphs, edit reverse edge
    if (graph->type == GRAPH_UNDIRECTED) {
        double old_weight = 0.0;
        switch (node_add_edge(to_node, from, weight, &old_weight, true)) {
            case STATUS_SUCCESS:
                return STATUS_SUCCESS;
            case STATUS_WARNING:
            case STATUS_ERROR:
                break;
        }

        // Undo changes to forward edge
        switch (node_add_edge(from_node, to, old_weight, NULL, true)) {
            case STATUS_SUCCESS:
                break;
            case STATUS_WARNING:
            case STATUS_ERROR:
                fprintf(stderr,
                    "Fatal error: Undirected graph has been corrupted. " 
                    "rollback failed to revert forward edge %d->%d\n", from, to);
                return STATUS_ERROR;
        }
    } else {
        return STATUS_SUCCESS;
    }

    fprintf(stderr, "Error: Failed to edit edge between nodes %d and %d\n", from, to);
    return STATUS_WARNING;       
}

Status graph_remove_edge(Graph* graph, int from, int to) {
    CHECK_GRAPH

    Node* from_node = find_node(graph, from);
    if (!from_node) {
        fprintf(stderr, "Warning: A node with ID %d does not exist in the graph", from);
        return false;
    }

    Node* to_node = find_node(graph, to);
    if (!to_node) {
        fprintf(stderr, "Warning: A node with ID %d does not exist in the graph", to);
        return false;
    }

    switch (node_remove_edge(from_node, to, NULL)) {
        case STATUS_SUCCESS:
            break;
        case STATUS_WARNING:
            // will throw warning if edge does not exist
            return STATUS_WARNING;
        case STATUS_ERROR:
            return STATUS_ERROR;
    }

    // For undirected graphs, edit reverse edge
    if (graph->type == GRAPH_UNDIRECTED) {
        double old_weight = 0.0;
        switch (node_remove_edge(to_node, from, &old_weight)) {
            case STATUS_SUCCESS:
                return STATUS_SUCCESS;
            case STATUS_WARNING:
            case STATUS_ERROR:
                break;

        }

        // Undo removal of forward edge
        switch (node_add_edge(from_node, to, old_weight, NULL, true)) {
            case STATUS_SUCCESS:
                break;
            case STATUS_WARNING:
            case STATUS_ERROR:
                fprintf(stderr, 
                    "Fatal error: Undirected graph has been corrupted. "
                    "rollback failed to restore forward edge %d->%d\n", from, to);
                return STATUS_ERROR;
        }
    } else {
        return STATUS_SUCCESS;
    }


    fprintf(stderr, "Failed to remove edge between nodes %d and %d\n", from, to);
    return false;
}
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
    }

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

bool graph_add_node(Graph* graph, int node_id, int node_capacity) {
    // Check if graph
    if (!graph) {
        fprintf(stderr, "Graph does not exist\n");
        return false;
    }

    // Check if node already exists in graph
    if (find_node(graph, node_id)) {
        fprintf(stderr, "A node with ID %d already exists\n", node_id);
        return false;
    }

    // Check if nodes arrays needs rezising
    if (graph_needs_resize(graph)) {
        // Resize nodes table
        printf("Graph needs to be resized...\n");
        if (!graph_resize(graph)) return false;
    }

    // Initialize new node
    if (node_capacity <= 0) node_capacity = INITIAL_CAPACITY;
    Node* new_node = create_node(node_id, node_capacity);
    if (!new_node) {
        fprintf(stderr, "Failed to add new node\n");
        return false;
    }
    
    if (!add_to_hash_table(new_node, graph->node_capacity, graph->nodes)) {
        fprintf(stderr, "Failed to add new node to the graph\n");
        free(new_node->neighbors);
        free(new_node);
        return false;
    }

    graph->node_count++;
    return true;
}

bool graph_add_edge(Graph* graph, int from, int to, double weight) {
    if (!graph) {
        fprintf(stderr, "Graph does not exist\n");
        return false;
    }

    Node* from_node = find_node(graph, from);
    if (!from_node) {
        fprintf(stderr, "No node with ID %d exists\n", from);
        return false;
    }
    Node* to_node = find_node(graph, to);
    if (!to_node) {
        fprintf(stderr, "No node with ID %d exists\n", to);
        return false;
    }

    // Check if edge already exists
    if (edge_exists(from_node, to)) {
        fprintf(stderr, "An edge %d -> %d already exists\n", from, to);
        return false;
    }

    // Check if edges array needs resizing
    if (node_needs_resize(from_node)) {
        if (!node_resize(from_node)) {
            fprintf(stderr, "Failed to add edge to node with ID %d\n", from_node->id);
            return false;
        }
    }

    // Add new edge/neighbor
    EdgeNode* new_neighbor = &from_node->neighbors[from_node->neighbor_count];
    new_neighbor->node_id = to;
    new_neighbor->weight = weight;
    from_node->neighbor_count ++;

    // For undirected graphs, add reverse edge
    if (graph->type == GRAPH_UNDIRECTED) {
        // * no need to check for reverse edge here, the forward exists then the reverse exists too
        // Check if edges array needs resizing
        if (node_needs_resize(to_node)) {
            if (!node_resize(to_node)) {
                fprintf(stderr, "Failed to add new edge\n");
                return false;
            }
        }

        // Add reverse edge
        EdgeNode* new_edge = &to_node->neighbors[to_node->neighbor_count];
        new_edge->node_id = from;
        new_edge->weight = weight;
        to_node->neighbor_count ++;
    }

    return true;
}

bool graph_remove_edge(Graph* graph, int from, int to) {
    if (!graph) return false;

    Node *from_node = find_node(graph, from);
    if (!from_node) return false;

    Node *to_node = find_node(graph, to);
    if (!to_node) return false;

    for (size_t i = 0; i < from_node->neighbor_count - 1; i++) {
        if (from_node->neighbors[i].node_id == to) {
            from_node->neighbors[i] = from_node->neighbors[i + 1];

            from_node->neighbor_count--;
            return true;
        }
    }

    return false;
}


bool graph_remove_node(Graph* graph, int node_id) {
    if (!graph) return false;

    Node* target_node = find_node(graph, node_id);
    if (!target_node) return false;

    // Remove all edges to this node from other nodes
    for (size_t i = 0; i < graph->node_count; i++) {
        Node* current_node = &graph->nodes[i];

        if (current_node->id == node_id) {
            // Free target node's neighbors
            free(current_node->neighbors);

            for (size_t j = i; j < graph->node_count - 1; j++) {
                graph->nodes[j] = graph->nodes[j + 1];
            }
            graph->node_count--;
        }

        for (size_t j = 0; j < current_node->neighbor_count; j++) {
            if (current_node->neighbors[j].node_id == node_id) {
                for (size_t k = j; k < current_node->neighbor_count - 1; k++) {
                    current_node->neighbors[k] = current_node->neighbors[k + 1];
                }
                current_node->neighbor_count--;

                break;
            }
        }
    }

    return true;
}
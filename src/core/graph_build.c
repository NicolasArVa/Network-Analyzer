#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../include/core/graph.h"
#include "../../include/utils/hash_table_utils.h"

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

// Helper: find Node by ID (Hash table lookup)
static Node* find_node(Graph* graph, int node_id) {
    unsigned int index = hash(node_id, graph->node_capacity);

    Node *current = graph->nodes[index];

    while (current) {
        if (current->id == node_id) return current;  // found
        current = current->next;
    }
    return NULL;  // not found
}

// Helper: detect if edge exists
// TODO: look for a faster method
static bool edge_exists(Node* from, int to) {
    for (size_t i = 0; i < from->neighbor_count; i++) {
        if (from->neighbors[i].node_id == to) return true;
    }
    return false;
}

// Helper: detect if graph needs to be resized
static needs_resizing(Graph* graph) {
    return (graph->node_count >= ALPHA * graph->node_capacity);
}

//Helper: resize nodes array
static bool graph_resize (Graph* graph) {
        size_t new_capacity = graph->node_capacity * 2; // * 1.5 to reset the table alpha to 0.5

        Node **new_nodes = calloc(new_capacity, sizeof(Node*));
        if (!new_nodes) {
            fprintf(stderr, "Failed to resize graph's nodes array\n");
            return false;
        }

        for (size_t i = 0; i < graph->node_capacity; i++) {
            Node *bucket = graph->nodes[i];

            while (bucket) {
                Node *head = remove_first(&bucket);
                add_to_hash_table(head, new_capacity, new_nodes);
            }
        }

        free(graph->nodes);
        graph->nodes = new_nodes;
        graph->node_capacity = new_capacity;

        printf("Graph successfully resized\n");
        return true;
}

// Helper: Create node
static Node* create_node(int node_id, int node_capacity) {
    // Allocate memory for new node
    Node* new_node = malloc(sizeof(Node));
    if (!new_node) {
        fprintf(stderr, "Failed to create new node\n");
        return NULL;
    }

    new_node->id = node_id;
    new_node->neighbors = NULL;
    new_node->neighbor_count = 0;
    new_node->capacity = node_capacity;
    new_node->next = NULL;

    new_node->neighbors = calloc(node_capacity, sizeof(EdgeNode)); // Initialize neighbors array
    if (!new_node->neighbors) {
        fprintf(stderr, "Failed to initialize new node's neighbors\n");
        free(new_node);
        return NULL;
    }

    return new_node;
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
    if (needs_resizing(graph)) {
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
    if (!graph) return false;

    Node* from_node = find_node(graph, from);
    Node* to_node = find_node(graph, to);

    if (!from_node || !to_node) return false;

    // Check if edge already exists
    if (edge_exists(from_node, to)) return false;

    // Check if edges array needs resizing
    if (from_node->neighbor_count >= from_node->capacity) {
        size_t new_capacity = from_node->capacity * 2;
        EdgeNode* new_neighbors = realloc(from_node->neighbors, new_capacity * sizeof(EdgeNode));
        if (!new_neighbors) return false;

        from_node->neighbors = new_neighbors;
        from_node->capacity = new_capacity;
    }

    // Add new edge/neighbor
    EdgeNode* new_neighbor = &from_node->neighbors[from_node->neighbor_count ++];

    new_neighbor->node_id = to;
    new_neighbor->weight = weight;

    // For undirected graphs, add reverse edge
    if (graph->type == GRAPH_UNDIRECTED) {
        // ! no checking for reverse edge here. if graph is undirected, this should have benn detected earlier
        // Check if edges array needs resizing
        if (to_node->neighbor_count >= to_node->capacity) {
            size_t new_capacity = to_node->capacity * 2;
            EdgeNode* new_neighbor = realloc(to_node->neighbors, new_capacity * sizeof(EdgeNode));
            if (!new_neighbor) return false;

            to_node->neighbors = new_neighbor;
            to_node->capacity = new_capacity;
        }

        // Add reverse edge
        EdgeNode* new_edge = &to_node->neighbors[to_node->neighbor_count ++];

        new_edge->node_id = from;
        new_edge->weight = weight;
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
};


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
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include "../../include/utils/hash_table_utils.h"
#include "../../include/utils/graph_build_utils.h"

unsigned int hash(int id, int table_size) { 
    unsigned int x = (unsigned int)id;

    // Mix the high and low bits
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;

    // Clamp into the table range
    return x % table_size;
}
 //Helper: detect if node is in a bucket froma a hash table
static bool exists_in_bucket(int id, Node* bucket) {
    Node *current = bucket;

    while (current) {
        if (current->id == id) return true;
        current = current->next;
    }

    return false;
}

Node* pop_bucket(Node** bucket) {
    if (!bucket || !*bucket) return NULL;
    Node *head = *bucket;

    *bucket = head->next;
    head->next = NULL;
    return head;
}

bool add_to_hash_table(Node* node, size_t table_size, Node** table) {
    // get the assigned bucket
    unsigned int index = hash(node->id, table_size);
    Node *head = table[index];

    // Check if node already exists
    if (exists_in_bucket(node->id, head)) {
        fprintf(stderr, "A node with ID %d already exists in the graph\n", node->id);
        return false;
    }

    // Add node to hash table
    node->next = head;
    table[index] = node;

    return true;
}

bool delete_from_hash_table(int id, size_t table_size, Node** table) {
    unsigned int index = hash(id, table_size);
    Node *current = table[index];
    Node *prev = NULL;

    while (current) {
        if (current->id == id) {
            if (!prev) {
                // delete head
                table[index] = current->next;
            } else {
                // delete middle
                prev->next = current->next;
            }

            free(current->neighbors);
            free(current);
            return true;
        }

        prev = current;
        current = current->next;
    }

    fprintf(stderr, "Node with ID %d does not exist in the graph\n", id);
    return false;
}
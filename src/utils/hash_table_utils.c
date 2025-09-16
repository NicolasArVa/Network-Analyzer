#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include "utils/general_utils.h"
#include "utils/hash_table_utils.h"
#include "utils/graph_build_utils.h"

# define CHECK_TABLE \
    do {    if (!table) {\
        fprintf(stderr, "Error: Invalid hash table passed to %s function call\n", __func__);\
        return STATUS_ERROR;\
    }\
} while(0);

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
    if (!bucket) return false;
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

Status add_to_hash_table(Node* node, size_t table_size, Node** table) {
    CHECK_TABLE
    if (!node) {
        fprintf(stderr, "Error: Trying to add NULL node to hash table\n");
        return STATUS_INVALID;
    }

    // get the assigned bucket
    unsigned int index = hash(node->id, table_size);
    Node *head = table[index];

    // Check if node already exists
    if (exists_in_bucket(node->id, head)) {
            #ifdef DEBUG
            printf("A node with ID %d already exists in the graph\n", node->id);
            #endif
            return STATUS_WARNING; // node already exists
        
    } else {
            // Add node to hash table
            node->next = head;
            table[index] = node;

            return STATUS_SUCCESS; 
    }
}

Status delete_from_hash_table(int id, size_t table_size, Node** table) {
    CHECK_TABLE

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
            return STATUS_SUCCESS;
        }

        prev = current;
        current = current->next;
    }

    #ifdef DEBUG
    printf("Node with ID %d does not exist in the graph\n", id);
    #endif
    return STATUS_WARNING;
}
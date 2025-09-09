#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include "../../include/utils/hash_table_utils.h"

unsigned int hash(int id, int table_size) { 
    unsigned int x = (unsigned int)id;

    // Mix the high and low bits
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;

    // Clamp into the table range
    return x % table_size;
}
 //Helper: detect if node is in hash table
static bool exists_in_bucket(int id, Node* bucket) {
    if (!bucket) return false;
    Node *current = bucket;

    while (current) {
        if (current->id == id) return true;
        current = current->next;
    }

    return false;
}

bool add_to_hash_table(Node* node, size_t table_size, Node** table) {
    // Check if table exists and if it already contains the node
    if (!table) {
        fprintf(stderr, "Hash table does not exist\n");
        return false;
    }

    // get the assigned bucket
    unsigned int index = hash(node->id, table_size);
    Node *head = table[index];

    // Check if node already exists
    if (exists_in_bucket(node->id, head)) {
        fprintf(stderr, "A node with ID %d already exists\n", node->id);
        return false;
    }

    // Add node to hash table
    node->next = head;
    table[index] = node;

    return true;
}

Node* remove_first(Node** list) {
    if (!list || !*list) return NULL;

    Node *head = *list;

    *list = head->next;
    head->next = NULL;
    return head;
}

bool delete_from_hash_table(int id, size_t table_size, Node** table) {
    if (!table) {
        fprintf(stderr, "Hash table does not exist\n");
        return false;
    }

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

    return false;
}
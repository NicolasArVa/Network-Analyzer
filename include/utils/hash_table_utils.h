#ifndef HASH_H
#define HASH_H

#include <stddef.h>
#include <stdbool.h>
#include "core/graph_build.h"

# define ALPHA 0.75 // load factor

// Hash function
unsigned int hash(int id, int table_size);

// Linked list helper
Node* pop_bucket(Node** list);

// Hash table functions
bool add_to_hash_table(Node* node, size_t table_size, Node** table);
bool delete_from_hash_table(int id, size_t table_size, Node** table);

#endif
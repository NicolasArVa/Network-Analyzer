#ifndef HASH_H
#define HASH_H

#include <stddef.h>
#include <stdbool.h>
#include "../../include/core/graph.h"

# define ALPHA 0.75 // load factor

unsigned int hash(int id, int table_size);

bool add_to_hash_table(Node* node, size_t table_size, Node** table);
bool delete_from_hash_table(int id, size_t table_size, Node** table);

Node* pop_bucket(Node** list);

#endif HASH_H
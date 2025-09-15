#include <stdio.h>
#include <stdlib.h>
#include "core/graph_build.h"
#include "core/graph_operations.h"
#include "utils/general_utils.h"

// TODO: change bool to -1, 0, 1

int main(int argc, char *argv[]) {
    char input[100];

    get_input(argc, argv, input, sizeof(input));

    printf("Final input: %s\n", input);

    return 0;
}

// Helper: strip leading/trailing whitespaces
static void strip(char *str) {
    char *start = str;
    char *end;

    // Trim leading whitespace
    while(isspace((unsigned char)*str)) str++;

    // if all whitespace, return empty string
    if (*str == 0) {
        start[0] = '\0';
        return;
    }

    // Trim trailing whitespace
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    *(end + 1) = '\0';

    // Shift the string to the left
    memmove(start, str, strlen(str) + 1);  // +1 for null terminator
}

void get_file_name(int argc, char *argv[], char* buffer, size_t size) {
    if (argc >= 2) {
        snprintf(buffer, size, "%s", argv[1]);
    } else {
        printf("Enter the graph's file name: ");
        if (fgets(buffer, size, stdin) == NULL) {
            buffer[0] = '\0';
            return;
        }
        // strip leading/trailing whitespaces
        strip(buffer);
        
    }
}

static int addNode(Graph* graph, int node_id, size_t node_capacity) {
    if (!find_node(graph, node_id)) {
        bool success = graph_insert_node(graph, node_id, node_capacity);
        if (!success) return 1;
    }

    return 0;
}

static int addEdge(Graph* graph, int source, int target, double weight) {
    bool success = graph_insert_edge(graph, source, target, weight);
    if (!success) return 1;

    return 0;
}
int load_graph(const char* filename, Graph* graph) {
    CHECK_EXISTS(graph, -1, "%s", "Graph not initialized");

    FILE *file = fopen(filename, "r");  // "r" for read, "w" for write, "a" for append
    if (file == NULL) {
        fprintf(stderr, "ERROR: Failed to open file %s\n", filename);
        return -1;
    }

    int warnings = 0;

    char line[256];
    int line_number = 0;
    
    while(fgets(line, sizeof(line), file) != NULL) {
        line_number++;
        
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\0') continue; // ignore comments and empty lines
        
        strip(line);
        
        int source, target;
        double weight = 1.0; // default weight is 1.0
        
        int parsed = sscanf(line, "%d %d %lf", &source, &target, &weight);

        if (parsed < 2) {
            printf("Warning: Invalid line %d: '%s'\n", line_number, line);
            warnings += 1;
            continue;
        }
        
        int source = addNode(graph, source, 16);
        int success2 = addNode(graph, target, 16);

        if (success1 && success2) {
            warnings += addEdge(graph, source, target, weight);
        }
        
        
        #ifdef DEBUG
        printf("Added edge %d->%d with weight %lf\n", source, target, weight);
        #endif
    }
    
    fclose(file);
    
    if (!warnings){
        printf("Successfully loaded graph from %s\n", filename);
    } else {
        if (warnings)
        fprintf(stderr, "WARNING: Graph loaded incompletely from %s.\n", filename);
    }
    return warnings;
}



static void string_to_lower(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

static bool is_integer(const char *str) {
    if (*str == '\0') return false;  // reject empty string

    while (*str) {
        if (!isdigit((unsigned char)*str)) {
            return false;  // found something that's not a digit
        }
        str++;
    }
    return true;
}
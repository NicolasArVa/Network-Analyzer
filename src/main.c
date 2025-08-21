#include <stdio.h>
#include <stdlib.h>
#include "include/core/graph.h"

int main(int argc, char* argv[]) {
    printf("Network Analysis Toolkit v0.1.0\n");
    
    if (argc < 2) {
        printf("Usage: %s <network_file>\n", argv[0]);
        return 1;
    }
    
    // TODO: Implement network loading and analysis
    printf("Loading network from: %s\n", argv[1]);
    
    return 0;
}
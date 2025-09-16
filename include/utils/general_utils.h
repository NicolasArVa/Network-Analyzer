#ifndef GENERAL_UTILS_H
#define GENERAL_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include "utils/general_utils.h"

// Helper macro: check if elements exist allowing for custom error messages
#define CHECK_EXISTS(ptr, value, fmsg, ...)\
    do {\
        if (!(ptr)) {\
            fprintf(stderr, fmsg "\n", ##__VA_ARGS__); return value;\
        }\
    } while (0)

// TODO:Implement errorcodes into functions
// Status codes for graph operations

typedef enum {
    STATUS_ERROR = -1,  // Graph integrity compromised, unsafe to continue
    STATUS_SUCCESS = 0, // Operation completed successfully
    STATUS_WARNING = 1, // Operation failed, but graph remains valid
    STATUS_OOM = 2,     // Operation failed due to out-of-memory problems
    STATUS_INVALID = 3  // Invalid input parameter
} Status;

typedef enum {
    ERR_NONE = 0,       // No error
    ERR_EXISTS,         // Node/edge already exists
    ERR_NOT_FOUND,      // Node/edge not found
    ERR_NO_MEMORY,      // Memory allocation failed, graph intact
    ERR_INVALID_ARG,    // Bad input parameter
    ERR_INTERNAL        // Internal logic error (should never happen)
} ErrorCode;

#endif
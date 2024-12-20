#include "header.h"

#define MAX 100
#define MESSAGE "Hello, World!"

// This is a single-line comment

/* This is a 
multi-line comment */

#ifdef DEBUG 
#define DEBUG_MESSAGE "Debugging Enabled"
#else
#define DEBUG_MESSAGE "Debugging Disabled"
#endif

int main() {
    int value = MAX;
    printf("%s\n", MESSAGE);
    printf("%s\n", DEBUG_MESSAGE);
    return 0;
}


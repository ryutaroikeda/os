#include "memory.h"

/**
 * This might not work when addresses overlap.
 */
void memory_copy(char* source, char* destination, int len) {
    for (int i = 0; i < len; i++) {
        destination[i] = source[i];
    }
}

void memory_set(void* destination, unsigned char byte, int len) {
    unsigned char* d = (unsigned char*) destination;
    for (int i = 0; i < len; i++) {
        d[i] = byte;
    }
}


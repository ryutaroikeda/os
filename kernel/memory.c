#include "memory.h"

void memory_copy(char* source, char* destination, int len) {
    for (int i = 0; i < len; i++) {
        destination[i] = source[i];
    }
}


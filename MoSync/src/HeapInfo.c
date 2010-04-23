#include <maheap.h>
#include <tlsf.h>

void* heapStart;

void override_heap_init_crt0(char* start, int length) {
    heapStart = start;
    ansi_heap_init_crt0(start, length);
}

size_t freeHeapMemory() {
    return get_max_size(heapStart) - get_used_size(heapStart);
}

size_t totalHeapMemory() {
    return get_max_size(heapStart);
}

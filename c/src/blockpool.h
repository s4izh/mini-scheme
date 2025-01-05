#ifndef __BLOCKPOOL_H__
#define __BLOCKPOOL_H__

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    void* data;
    bool* used;
    size_t blocks;
    size_t block_size;
} blockpool_t;

void blockpool_init(blockpool_t* bp, void* data, bool* used, size_t blocks, size_t block_size);

void* blockpool_alloc(blockpool_t* bp);

void blockpool_free(blockpool_t* bp, void* block);

#endif // __BLOCKPOOL_H__

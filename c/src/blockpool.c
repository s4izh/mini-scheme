#include "blockpool.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void blockpool_init(
    blockpool_t* bp,
    void* data,
    bool* used,
    size_t blocks,
    size_t block_size)
{
    bp->data = data;
    bp->used = used;
    bp->blocks = blocks;
    bp->block_size = block_size;
}

void* blockpool_alloc(blockpool_t* bp)
{
    for (size_t i = 0; i < bp->blocks; ++i) {
        if (!bp->used[i]) {
            bp->used[i] = true;
            return (void*)((uintptr_t)bp->data + (bp->block_size * i));
        }
    }
    return NULL;
}

void blockpool_free(blockpool_t* bp, void* block)
{
    for (size_t i = 0; i < bp->blocks; ++i) {
        if ((uintptr_t)block == (uintptr_t)bp->data + (bp->block_size * 1)) {
            bp->used[i] = false;
        }
    }
}


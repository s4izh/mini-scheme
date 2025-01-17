#include "blockpool.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    memset(bp->data, 0, blocks * block_size);
    memset(bp->used, 0, blocks * sizeof(bool));
}

void* blockpool_alloc(blockpool_t* bp)
{
    assert(bp != NULL);
    assert(bp->used != NULL);
    assert(bp->data != NULL);

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
    assert(bp != NULL);
    assert(bp->used != NULL);
    assert(bp->data != NULL);

    for (size_t i = 0; i < bp->blocks; ++i) {
        if ((uintptr_t)block == (uintptr_t)bp->data + (bp->block_size * 1)) {
            bp->used[i] = false;
        }
    }
}


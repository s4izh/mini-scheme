#ifndef __SLAB_H__
#define __SLAB_H__

typedef struct _slab_t slab_t;

struct _slab_t {


    slab_t* prev;
    slab_t* next;
};

#endif // !__SLAB_H__

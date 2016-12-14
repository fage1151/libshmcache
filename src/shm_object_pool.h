/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

//shm_object_pool.h

#ifndef _SHM_OBJECT_POOL_H
#define _SHM_OBJECT_POOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "common_define.h"
#include "shmcache_types.h"

struct shm_object_pool_context {
    struct shm_object_pool_info *obj_pool_info;
    int64_t *offsets;  //object offset array
    int index;   //for iterator
};

#ifdef __cplusplus
extern "C" {
#endif

/**
get object pool memory size for empty queue
parameters:
    element_size: the element size
    max_count: the max count
return memory size
*/
static inline int64_t shm_object_pool_get_empty_memory_size(
        const int element_size, const int max_count)
{
    return sizeof(int64_t) * (int64_t)max_count;
}

/**
get object pool memory size for full queue
parameters:
    element_size: the element size
    max_count: the max count
return memory size
*/
static inline int64_t shm_object_pool_get_full_memory_size(
        const int element_size, const int max_count)
{
    return (element_size + sizeof(int64_t)) * (int64_t)max_count;
}

/**
set object pool
parameters:
	op: the object pool
    obj_pool_info: fifo pool in share memory
    offsets: the array offsets
return error no, 0 for success, != 0 fail
*/
void shm_object_pool_set(struct shm_object_pool_context *op,
        struct shm_object_pool_info *obj_pool_info,
        int64_t *offsets);

/**
init object pool to empty queue
parameters:
	op: the object pool
return error no, 0 for success, != 0 fail
*/
void shm_object_pool_init_empty(struct shm_object_pool_context *op);

/**
init object pool to full queue
parameters:
	op: the object pool
return error no, 0 for success, != 0 fail
*/
void shm_object_pool_init_full(struct shm_object_pool_context *op);

/**
get object count in object pool
parameters:
	op: the object pool
return object count
*/
int shm_object_pool_get_count(struct shm_object_pool_context *op);

/**
alloc a node from the object pool
parameters:
	op: the object pool
return the alloced object offset, return -1 if fail
*/
int64_t shm_object_pool_alloc(struct shm_object_pool_context *op);

#define shm_object_pool_pop(op) shm_object_pool_alloc(op)

/**
free  a node to the object pool
parameters:
	op: the object pool
    obj_offset: the object offset
return 0 for success, != 0 fail
*/
int shm_object_pool_free(struct shm_object_pool_context *op, const int64_t obj_offset);

#define shm_object_pool_push(op, obj_offset) shm_object_pool_free(op, obj_offset)

/**
get first object
parameters:
	op: the object pool
return object offset, return -1 if empty
*/
static inline int64_t shm_object_pool_first(struct shm_object_pool_context *op)
{
    if (op->obj_pool_info->queue.head == op->obj_pool_info->queue.tail) {
        op->index = -1;
        return -1;
    }
    op->index = op->obj_pool_info->queue.head;
    return op->offsets[op->index];
}

/**
get next object
parameters:
	op: the object pool
return object offset, return -1 if empty
*/
static inline int64_t shm_object_pool_next(struct shm_object_pool_context *op)
{
    if (op->index == -1) {
        return -1;
    }

    op->index = (op->index + 1) % op->obj_pool_info->queue.capacity;
    if (op->index == op->obj_pool_info->queue.tail) {
        int64_t obj_offset;
        obj_offset = op->offsets[op->index];
        op->index = -1;
        return obj_offset;
    } else {
        return op->offsets[op->index];
    }
}

#ifdef __cplusplus
}
#endif

#endif

#pragma once


#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct
{
    void** buffer;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t count;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
    bool active;
} ring_buffer_t;

ring_buffer_t* rb_create(size_t capacity);
void rb_destroy(ring_buffer_t* rb);
bool rb_enqueue(ring_buffer_t* rb, void* item);
void* rb_dequeue(ring_buffer_t* rb);
void rb_stop(ring_buffer_t* rb);

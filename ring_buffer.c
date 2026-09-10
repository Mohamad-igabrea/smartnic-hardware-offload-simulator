#include "ring_buffer.h"
#include <stdlib.h>

ring_buffer_t* rb_create(size_t capacity)
{
    ring_buffer_t* rb = malloc(sizeof(ring_buffer_t));
    rb->buffer = malloc(capacity * sizeof(void*));
    rb->capacity = capacity;
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    rb->active = true;

    pthread_mutex_init(&rb->lock, NULL);
    pthread_cond_init(&rb->not_empty, NULL);
    pthread_cond_init(&rb->not_full, NULL);

    return rb;
}

void rb_destroy(ring_buffer_t* rb)
{
    if (!rb) return;
    pthread_mutex_destroy(&rb->lock);
    pthread_cond_destroy(&rb->not_empty);
    pthread_cond_destroy(&rb->not_full);
    free(rb->buffer);
    free(rb);
}

bool rb_enqueue(ring_buffer_t* rb, void* item)
{
    pthread_mutex_lock(&rb->lock);

    while (rb->count == rb->capacity && rb->active)
    {
        pthread_cond_wait(&rb->not_full, &rb->lock);
    }

    if (!rb->active)
    {
        pthread_mutex_unlock(&rb->lock);
        return false;
    }

    rb->buffer[rb->tail] = item;
    rb->tail = (rb->tail + 1) % rb->capacity;
    rb->count++;

    pthread_cond_signal(&rb->not_empty);
    pthread_mutex_unlock(&rb->lock);

    return true;
}

void* rb_dequeue(ring_buffer_t* rb)
{
    pthread_mutex_lock(&rb->lock);

    while (rb->count == 0 && rb->active)
    {
        pthread_cond_wait(&rb->not_empty, &rb->lock);
    }

    if (rb->count == 0 && !rb->active)
    {
        pthread_mutex_unlock(&rb->lock);
        return NULL;
    }

    void* item = rb->buffer[rb->head];
    rb->head = (rb->head + 1) % rb->capacity;
    rb->count--;

    pthread_cond_signal(&rb->not_full);
    pthread_mutex_unlock(&rb->lock);

    return item;
}

void rb_stop(ring_buffer_t* rb)
{
    pthread_mutex_lock(&rb->lock);
    rb->active = false;
    pthread_cond_broadcast(&rb->not_empty);
    pthread_cond_broadcast(&rb->not_full);
    pthread_mutex_unlock(&rb->lock);
}

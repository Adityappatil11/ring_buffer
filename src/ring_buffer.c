#include "ring_buffer.h"

void ring_buffer_init(ring_buffer_t *rb,uint8_t *storage,size_t capacity){
    if(rb == NULL){
        return;
    }

    rb->buffer = storage;
    rb->capacity = capacity;
    ring_buffer_reset(rb);
}

void ring_buffer_reset(ring_buffer_t *rb){
    if(rb == NULL){
        return;
    }
    rb->head = 0;
    rb->tail = 0;
    rb->size = 0;
}

bool ring_buffer_is_empty(const ring_buffer_t *rb){
    if(rb == NULL){
        return true;
    }
    return (rb->size == 0);
}

bool ring_buffer_is_full(const ring_buffer_t *rb){
    if(rb == NULL || rb->capacity == 0){
        return true;
    }
    return (rb->size == rb->capacity);
}

size_t ring_buffer_capacity(const ring_buffer_t *rb){
    return (rb != NULL) ? rb->capacity : 0;
}

size_t ring_buffer_size(const ring_buffer_t *rb){
    return (rb != NULL) ? rb->size : 0;
}

bool ring_buffer_push(ring_buffer_t *rb,uint8_t data){
    if(rb == NULL || rb->buffer == NULL || ring_buffer_is_full(rb)){
        return false;
    }
    rb->buffer[rb->head]=data;
    rb->head = (rb->head + 1) % rb->capacity;
    rb->size++;

    return true;
}

bool ring_buffer_pop(ring_buffer_t *rb,uint8_t *data){
    if(rb == NULL || rb->buffer == NULL || ring_buffer_is_empty(rb)){
        return false;
    }
    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % rb->capacity;
    rb->size--;

    return true;
}
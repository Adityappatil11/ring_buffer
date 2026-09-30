#include "ring_buffer.h"
#include "string.h"

#define MIN(a, b) ((a)<(b) ? (a) : (b))

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

size_t ring_buffer_write(ring_buffer_t *rb,const uint8_t *data,size_t len){
    if(rb == NULL || rb->buffer == NULL || data == NULL || len == 0){
        return 0;
    }
    size_t available_space = rb->capacity - rb->size;

    size_t write_len = MIN(len,available_space);
    if(write_len == 0){
        return 0;
    }

    size_t space_until_end = rb->capacity - rb->head;

    if(write_len <= space_until_end){
        memcpy(&rb->buffer[rb->head],data,write_len);
    }else{
        // part 1
        memcpy(&rb->buffer[rb->head],data,space_until_end);
        // part 2
        memcpy(&rb->buffer[rb->head],data+space_until_end,write_len-space_until_end);
    }

    rb->head = (rb->head + write_len) % rb->capacity;
    rb->size += write_len;

    return write_len;
}

size_t ring_buffer_read(ring_buffer_t *rb,uint8_t *data,size_t len){
    if(rb == NULL || rb->buffer == NULL || data == NULL || len == 0){
        return 0;
    }

    size_t available_data = rb->size;

    size_t read_len = MIN(len,available_data);
    if(read_len == 0){
        return 0;
    }

    size_t data_until_end = rb->capacity - rb->tail;

    if(read_len <= data_until_end){
        memcpy(data,&rb->buffer[rb->tail],read_len);
    }else{
        // part 1
        memcpy(data,&rb->buffer[rb->tail],data_until_end);
        // part 2
        memcpy(data+data_until_end,&rb->buffer[0],read_len-data_until_end);
    }

    rb->tail = (rb->tail + read_len) % rb->capacity;
    rb->size -= read_len;

    return read_len;
}
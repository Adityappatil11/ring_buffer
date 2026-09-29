#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*
@brief ring buffer state structure 
The caller is responsible for allocation this struct and backing array
  */
typedef struct{
    uint8_t *buffer;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t size;
}ring_buffer_t;

/* 
@brief initilize the ring buffer with caller provided memory

@param rb pointer to the ring buffer structure
@param storage pointer to memory array used for storage
@param capacity the size of the storage array in bytes

*/
void ring_buffer_init(ring_buffer_t *rb,uint8_t *storage,size_t capacity);

/* 
@brief resets the buffer to an empty state
 */

void ring_buffer_reset(ring_buffer_t *rb);

// State queries
bool ring_buffer_is_empty(const ring_buffer_t *rb);
bool ring_buffer_is_full(const ring_buffer_t *rb);
size_t ring_buffer_capacity(const ring_buffer_t *rb);
size_t ring_buffer_size(const ring_buffer_t *rb);

// Single byte operation

/* 
@brief push single byte data into buffer
@return returns true if successfull or false if fail */
bool ring_buffer_push(ring_buffer_t *rb,uint8_t data);

/* 
@brief pops a single data from buffer
@returns true if successfull or false if buffer is empty */
bool ring_buffer_pop(ring_buffer_t *rb,uint8_t *data);

// Multi byte operations

/* 
@brief writes multiple bytes into buffer
@param rb pointer to write the data
@param data pointer of the data
@param length the number of bytes to write
@return number of bytes sucessfully written */
size_t ring_buffer_write(ring_buffer_t *rb,const uint8_t *data,size_t len);

/* 
@brief reads multiple data form the ring buffer 
@params rb pointer to the ring buffer
@params data pointer to the destination buffer
@params len number of bytes to be read
@returns number of bytes successfully read */
size_t read_ring_buffer(ring_buffer_t *rb,uint8_t *data,size_t len);

#endif  //RING_BUFFER_H
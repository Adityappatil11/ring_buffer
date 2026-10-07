#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../include/ring_buffer.h"

#define BUF_SIZE 5

void test_initilization(){
    uint8_t storage[BUF_SIZE];
    ring_buffer_t rb;

    ring_buffer_init(&rb,storage,BUF_SIZE);

    assert(ring_buffer_capacity(&rb) == BUF_SIZE);
    assert(ring_buffer_size(&rb) == 0);
    assert(ring_buffer_is_empty(&rb) == true);
    assert(ring_buffer_is_full(&rb) == false);
    printf("Initilization test passed.\n");
}

void test_single_byte(){
    uint8_t storage[BUF_SIZE];
    ring_buffer_t rb;

    ring_buffer_init(&rb,storage,BUF_SIZE);

    for(uint8_t i = 0;i<BUF_SIZE;i++){
        assert(ring_buffer_push(&rb,i) == true);
    }

    assert(ring_buffer_is_full(&rb) == true);
    assert(ring_buffer_push(&rb,99) == false);  //should reject

    uint8_t val;
    assert(ring_buffer_pop(&rb,&val) == true);
    assert(val == 0);
    assert(ring_buffer_is_full(&rb) == false);

    printf("Single byte push/pop test passed.\n");
}

void test_multi_byte(){
    uint8_t storage[BUF_SIZE];
    ring_buffer_t rb;

    ring_buffer_init(&rb,storage,BUF_SIZE);

    // Push 3 bytes: [A, B, C, _, _]
    uint8_t in_data[] = {'A','B','C'};
    assert(ring_buffer_write(&rb,in_data,3) == 3);

    // Pop 2 bytes: [_, _, C, _, _] -> Head is 3, Tail is 2
    uint8_t out_data[5] = {0};
    assert(ring_buffer_read(&rb,out_data,2) == 2);
    assert(out_data[0] == 'A' && out_data[1] == 'B');

    // Write 4 bytes: should only write 4 because capacity is 5, size is 1.
    // This forces a wrap: [Y, Z, C, W, X] -> Head is 2, Tail is 2 (Full)
    uint8_t more_data[] = {'W','X','Y','Z'};
    assert(ring_buffer_write(&rb,more_data,4) == 4);
    assert(ring_buffer_is_full(&rb) == true);

    // Read all 5 bytes, verifying order: C, W, X, Y, Z
    assert(ring_buffer_read(&rb, out_data, 5) == 5);
    assert(out_data[0] == 'C');
    assert(out_data[1] == 'W');
    assert(out_data[2] == 'X');
    assert(out_data[3] == 'Y');
    assert(out_data[4] == 'Z');
    
    assert(ring_buffer_is_empty(&rb) == true);
    printf("Wraparound and multi-byte test passed.\n");
}


int main(){
    printf("Running Ring Buffer tests...\n");
    test_initilization();
    test_single_byte();
    test_multi_byte();
    printf("All tests passed Successfully!\n");
    return 0;
}
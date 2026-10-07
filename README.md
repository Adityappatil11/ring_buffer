# Fixed-Capacity Ring Buffer

A lightweight, statically allocated byte-oriented ring buffer (circular buffer) written in C. 

This project is specifically designed for bare-metal embedded systems and resource-constrained environments. It is ideal for handling streaming data, such as UART receive/transmit queues, I2C/SPI sensor data streams, or audio buffering in a Producer/Consumer architecture.

## Key Features

* **Zero Dynamic Allocation:** Completely avoids `malloc()` and `free()`. The caller provides the backing memory array, guaranteeing zero memory fragmentation and deterministic initialization.
* **Partial Progress Semantics:** Multi-byte read and write operations process as much data as currently possible and return the number of bytes successfully handled. This prevents blocking and is perfect for asynchronous stream pipelines.
* **Optimized Block Transfers:** Leverages standard `memcpy()` for multi-byte operations instead of looping byte-by-byte, efficiently handling the complex pointer arithmetic required when data wraps around the physical ends of the array.
* **Strict API Contract:** Clear separation of concerns with predictable return values for boundary conditions (e.g., pushing to a full buffer, popping from an empty one).

## Project Structure

```text
.
├── Makefile               # Build system for compilation and testing
├── README.md              # Project documentation
├── include/
│   └── ring_buffer.h      # API contract and data structure definitions
├── src/
│   └── ring_buffer.c      # Core library implementation
└── tests/
    └── test_ring_buffer.c # Automated test suite using <assert.h>
```

## API Reference

### Initialization & State

* `void ring_buffer_init(ring_buffer_t *rb, uint8_t *storage, size_t capacity)`
  Initializes the ring buffer. You must pass a pre-allocated `storage` array of `capacity` bytes.
* `void ring_buffer_reset(ring_buffer_t *rb)`
  Clears the buffer, resetting head, tail, and size to 0.
* `bool ring_buffer_is_empty(const ring_buffer_t *rb)`
  Returns `true` if the buffer contains no unread data.
* `bool ring_buffer_is_full(const ring_buffer_t *rb)`
  Returns `true` if the buffer is at maximum capacity.
* `size_t ring_buffer_capacity(const ring_buffer_t *rb)`
  Returns the total maximum capacity.
* `size_t ring_buffer_size(const ring_buffer_t *rb)`
  Returns the current number of unread bytes.

### Single-Byte Operations

* `bool ring_buffer_push(ring_buffer_t *rb, uint8_t data)`
  Inserts a single byte. Returns `true` on success, `false` if the buffer is full.
* `bool ring_buffer_pop(ring_buffer_t *rb, uint8_t *data)`
  Retrieves a single byte. Returns `true` on success, `false` if the buffer is empty.

### Multi-Byte Operations

* `size_t ring_buffer_write(ring_buffer_t *rb, const uint8_t *data, size_t len)`
  Attempts to write `len` bytes. Returns the actual number of bytes written (which will be `< len` if the buffer fills up).
* `size_t ring_buffer_read(ring_buffer_t *rb, uint8_t *data, size_t len)`
  Attempts to read `len` bytes. Returns the actual number of bytes read (which will be `< len` if the buffer empties out).

## Usage Examples

### 1. Basic Initialization and Single-Byte Usage
```c
#include <stdio.h>
#include "ring_buffer.h"

#define BUF_SIZE 128
uint8_t rx_storage[BUF_SIZE];
ring_buffer_t rx_buffer;

int main() {
    ring_buffer_init(&rx_buffer, rx_storage, BUF_SIZE);

    // Simulate receiving a byte from hardware
    if (ring_buffer_push(&rx_buffer, 0xAA)) {
        printf("Byte buffered successfully.\n");
    }

    // Read it back
    uint8_t data;
    if (ring_buffer_pop(&rx_buffer, &data)) {
        printf("Processed: 0x%02X\n", data);
    }
    return 0;
}
```

### 2. Multi-Byte Stream Pipeline
```c
// Producer side
uint8_t payload[] = "Hello Embedded World!";
size_t bytes_to_send = sizeof(payload);
size_t bytes_sent = 0;

// Write as much as possible, keep track of what's left
bytes_sent += ring_buffer_write(&tx_buffer, payload, bytes_to_send);

// Consumer side
uint8_t chunk[10];
size_t processed = ring_buffer_read(&tx_buffer, chunk, sizeof(chunk));
// processed might be 10, or it might be less if the buffer had less data
```

## Building and Testing

This project relies on `make` and `gcc`.

* **Compile the code:** `make`
* **Run the test suite:** `make test`
* **Clean build artifacts:** `make clean`

The test suite runs assertions to guarantee that boundary logic, wraparound math, and state tracking behave correctly under stress.

## Architectural & Design Notes

This project is structured to highlight key embedded software engineering principles.

### Concurrency and Interrupt Safety
**Warning:** This specific implementation utilizes a shared `size` variable to track the buffer's fullness. Consequently, it is **not thread-safe or interrupt-safe** out of the box. 

If a hardware interrupt (e.g., an Rx ISR) calls `push()` while the main loop is calling `pop()`, the non-atomic Read-Modify-Write cycle on the `size` variable will cause data corruption. Marking the structure as `volatile` will *not* resolve this atomicity issue.

To use this in a concurrent context, you must either:
1. Wrap push/pop calls in critical sections (disabling interrupts temporarily).
2. Adapt the code into a **Lock-Free Single-Producer/Single-Consumer** queue. This involves removing the `size` variable entirely and sacrificing one byte of array space to distinguish a full state from an empty state by comparing only the `head` and `tail` pointers.

### Arithmetic Optimization
Index wraparound is achieved using the modulo (`%`) operator. On high-end processors, this is negligible. However, on low-end microcontrollers (like the ARM Cortex-M0) which lack hardware division, modulo takes dozens of clock cycles. In heavily optimized environments, the `capacity` is strictly locked to a power of 2 (e.g., 64, 128, 256), allowing the modulo operation to be replaced by a single-cycle bitwise AND mask: 
`next_index = (current_index + 1) & (capacity - 1)`.
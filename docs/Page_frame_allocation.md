# Page Frame Allocation

## 1. Physical Memory Allocators

These are the algorithms that will provide you with a new page frame when you need it. The client of this algorithm is usually indifferent to which frame is returned, and especially, a request for n frames doesn't need to return contiguous frames (unless you are allocating memory for DMA operations like network packet buffers).

N will be the size of the memory in pages in the following text.

### 1.1. Bit map

### 1.2. Stack/List of pages

### 1.3. Sized Portion Scheme

### 1.4. Buddy Allocation System

### 1.5. Hybrid Scheme

### 1.6. Hybrid Scheme

## 2. Virtual Addresses Allocator

### 2.1. Flat List

### 2.2. Tree-based approach

# libds — Data Structures Library in C

libds is a modular data structure library implemented in C.
The project focuses on low-level memory management, API design,
and building safe, reusable components similar to real-world C libraries.

This project was built while studying C and core data structures
to strengthen fundamentals for future Software Engineering and ML/AI work.

---

## 🎯 Motivation

Many beginner projects focus only on making things "work".
This project focuses on building data structures **correctly and safely**:

- Manual memory management using malloc / realloc / free
- Designing clean and reusable APIs
- Separating unsafe fast paths and safe error-checked APIs
- Testing memory safety using AddressSanitizer and UBSanitizer
- Organizing code into reusable modules

The goal is to build strong low-level foundations before moving to
higher-level ML/AI systems.

---

## 📚 Implemented Data Structures

### Core Containers
- Dynamic Array
- Singly Linked List

### Abstract Data Types (Built on Dynamic Array)
- Stack (LIFO)
- Queue (FIFO)

### Hash Tables
- Hash Table (string → int)
  - Separate chaining for collision handling
  - Automatic resizing based on load factor

- Generic Hash Table (string → void*)
  - Optional value ownership using destructor callbacks
  - Safe overwrite and removal with automatic cleanup
  - Iterator API for traversing all entries

---

## 🛡 Safety Design

Each module provides two layers of APIs:

### Unsafe API
- No bounds checking
- Faster
- Intended for performance-critical internal use

### Safe API
- Explicit error codes
- Null checks and bounds checks
- Prevents out-of-bounds access and invalid operations

Memory correctness is verified using:
- AddressSanitizer (ASan)
- UndefinedBehaviorSanitizer (UBSan)

---

## 🔁 Iterator Support

The generic hash table provides an iterator API:

```c
HashTableGIter it = htg_iter_begin(ht);
const char *key;
void *value;

while (htg_iter_next(&it, &key, &value)) {
    // process key and value
}
This allows traversal without exposing internal bucket or node structures.

---

## 🧪 Testing

Each data structure has its own test program using assert():
 test_dynamic_array
 test_linked_list
 test_stack
 test_queue
 test_hash_table
 test_hash_table_generic

Tests cover:
 Normal usage
 Boundary cases
 Error handling
 Destructor correctness (for generic hash table)

---

## 🔧 Build & Run

Build all tests
make clean
make

Run individual tests
./test_dynamic_array
./test_linked_list
./test_stack
./test_queue
./test_hash_table
./test_hash_table_generic

All builds are compiled with:
 -fsanitize=address,undefined
 -Wall -Wextra
to catch memory and undefined behavior bugs early.

---

## 🗂 Project Structure

libds/
├─ include/   # Public API headers
├─ src/       # Implementations
├─ tests/     # Unit tests
├─ Makefile
└─ README.md

---

## 🚀 Future Work

Planned or possible extensions:
 Open addressing hash table (linear / quadratic probing)
 Circular buffer implementation for O(1) queue operations
 Generic containers with custom allocator support
 Using this library as a backend for small ML/AI experiments

---

## 📌 What I Learned

Through this project, I practiced:
 Manual heap management and lifetime control
 Pointer-based data structure implementation
 Designing stable and safe C APIs
 Writing defensive code with explicit error handling
 Testing memory safety using sanitizers
 Structuring multi-file C projects with Makefiles
 This project serves as a foundation for more advanced systems and ML-related projects.

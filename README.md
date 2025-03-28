# 🧠 ADS_set – Custom Hash Set Implementation (C++)

This header file defines a generic hash set data structure named `ADS_set`, developed as part of a university-level **Algorithms and Data Structures** course. It supports constant-time average complexity for insertion, deletion, and lookup, using **separate chaining** (linked lists) and **dynamic rehashing** to manage collisions and growth.

---

## 📦 File

- `ADS_set.h` – Header-only implementation of a hash-based set container.

---

## ✨ Features

- Templated for any key type (`ADS_set<Key, N>`)
- Separate chaining via singly linked lists
- Custom iterator with forward iteration
- Dynamic resizing and rehashing
- Supports:
  - `insert`
  - `erase`
  - `find`
  - `clear`
  - `count`
  - `swap`
  - range and initializer list insertion
- Compatible with range-based `for` loops

---

## 🔧 Example Usage

```cpp
#include "ADS_set.h"
#include <iostream>

int main() {
    ADS_set<int> myset;

    myset.insert({1, 2, 3, 4});
    myset.erase(2);

    for (int val : myset) {
        std::cout << val << ' ';
    }

    return 0;
}
```

## Iterator Support
- Fully supports STL-style iteration with begin() and end()
- Returns constant iterators
- Allows seamless use in `for-each` loops


## Internal Design Notes
- Hashing: Uses std::hash<Key> for key hashing.
- Load Factor: Automatically resizes when load exceeds 0.7
- Rehashing: Table size scales up (×3) to maintain performance

## Dump Function
- Includes a helpful dump() function to print the internal state of the hash table for debugging purposes.
```cpp
myset.dump(); // Outputs contents and structure to std::cerr
```

## 📘 Course Context
This code was developed as part of the "Algorithms and Data Structures" course at university, focusing on low-level data structure implementation and performance.

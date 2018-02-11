![libconcurrency]

# Overview

libconcurrency is a multi-platform support library for concurrent programming.
This library provides both [C] implementation and [C++] implementation.

# Prerequisites

* a C compiler which supports C17
* a C++ compiler which supports C++17

# Build and Install

There are two build methods: [make] and [CMake].

To build with [make]:

```bash
$ make
$ make install
```

To build with [CMake]:

```bash
$ mkdir build
$ cd build
$ cmake .. 
```

# Acknowledgements

* JSR 166: Concurrency Utilities (java.util.concurrent package)

# References

* Michael, Maged M. and Scot, Michael L. 1996. Simple, Fast, and Practical Non-Blocking and Blocking Concurrent Queue Algorithms.

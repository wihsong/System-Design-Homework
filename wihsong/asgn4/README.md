# Assignment 4 Directory
This directory contains source code and other files for Assignment 4.

## Overview
Assignment 4 expands upon the concepts of synchronization and concurrency, introducing a multi-threaded HTTP server in C. The server handles multiple client requests simultaneously, ensuring thread-safe operations and efficient resource management.

## Design Summary
- The implementation is built around a multi-threaded architecture, utilizing a pool of worker threads to process incoming HTTP requests concurrently.
- A thread-safe queue efficiently manages incoming connections, allowing the main thread to quickly return to accepting new requests.
- Reader-writer locks are employed to ensure data consistency during concurrent read-write operations on shared resources.

## Components
- **Listener_Socket**: A structure to manage server socket operations, including initialization and accepting incoming connections.
- **queue_t**: A thread-safe queue that stores incoming connections for processing by worker threads.
- **rwlock_t**: A customizable reader-writer lock that supports various priority schemes, ensuring fair access to shared resources.
- **Worker Threads**: A set of pre-spawned threads responsible for handling HTTP requests and executing appropriate read or write operations.

## Implementation Highlights
- The server is designed to handle GET and PUT requests, efficiently serving files and accepting uploads.
- Error handling is a critical aspect, with the server capable of returning appropriate HTTP response codes for various error scenarios.
- A focus on performance optimization ensures the server can handle multiple requests without significant delays, making it suitable for environments with high concurrency.

## Building and Running
- Compile the server using `make`, which produces the `httpserver` executable.
- Run the server with `./httpserver <port_number> [-t <thread_count>]`, where `port_number` specifies the listening port and `thread_count` sets the number of worker threads.

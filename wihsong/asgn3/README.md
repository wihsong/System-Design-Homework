#Assignment 3 directory

This directory contains source code and other files for Assignment 3.

## Overview

This repository offers a concise implementation of a thread-safe queue and a reader-writer lock in C, focusing on synchronization and concurrency control with priority handling.

## Design Summary

The thread-safe queue leverages a dynamic array with circular buffer logic, ensuring O(1) complexity for typical operations while maintaining thread safety with `pthread` mutexes and condition variables.

The reader-writer lock is configurable with various priority schemes to prevent thread starvation. The design aims for flexibility and extensibility to accommodate future enhancements like the `N_WAY` access pattern.

## Components

- `queue_t`: A fixed-size queue data structure with thread-safe enqueue and dequeue capabilities.
- `rwlock_t`: A reader-writer lock system supporting multiple priority options for fine-tuned access control in concurrent environments.

## Implementation Highlights

The project addresses ambiguous specifications by setting a fixed queue size and prioritizing writers by default to prevent starvation, keeping in mind the possibility of adjusting these decisions as needed.

# High-Performance Real-Time Order Book Simulator (C)

## Overview

A real-time, multithreaded order book simulator in C that gets live market data from Coinbase's WebSocket feed for BTC-USD.
The system automatically executes buy/sell orders by price priority and provides measurement of throughput as well as latency under actual trading workload.

## Features

- Multithreaded order processing with POSIX threads
- Lock-free order insertion
- Real-time trade ingestion from Coinbase WebSocket API
- Price-priority matching (highest bid and lowest ask)
- Live benchmarking: latency (μs) and throughput (orders/sec)
- Lightweight C codebase with manual memory management

## Build Instructions

```bash
make
```

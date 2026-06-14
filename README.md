# C++ Order Book

A low-latency limit order book written in C++17, built to approximate the architecture used in high-frequency trading systems.

Orders flow from a producer thread through a **lock-free SPSC ring buffer** into a **price-ladder matching engine** that runs price-time priority matching with O(1) lookups. Latency is measured end-to-end across both the queue and matching stages.

---

## Architecture

### Lock-free SPSC Queue
Orders are passed between the producer and consumer thread via a single-producer single-consumer ring buffer. The head and tail pointers are `std::atomic` with acquire/release semantics, and each is placed on its own **64-byte cache line** to eliminate false sharing — the main performance killer in concurrent low-latency systems.

### Price Ladder
The order book uses a **flat array indexed by price tick** rather than a tree or hash map. Each slot in the array corresponds to one price level — lookup is a single array index operation with no hashing, no pointer chasing, and guaranteed cache locality on hot levels.

```
index = price - MIN_PRICE        // O(1), no heap allocation
```

This replaces the O(log n) traversals of `std::map` and the unpredictable cache behaviour of `std::unordered_map`.

### Matching Engine
- **Price-time priority** — best price fills first; FIFO within a price level
- **O(1) cancel** — every resting order is indexed by ID in an `unordered_map`, so cancellation is a direct lookup with no level scanning
- Handles full fills, partial fills, and multi-level sweeps

### Latency Measurement
Queue latency (time from order creation to dequeue) and matching latency are tracked separately. p50/p99 percentiles are reported at shutdown.

---

## Performance

Measured on an i7-12700, release build (`-O3 -march=native`), with `cout` removed from the hot path:

| Metric | Latency |
|--------|---------|
| p50    | ~1200ns |
| p99    | ~14700ns |

---

## Build

```bash
g++ -std=c++17 -O3 -march=native -o orderbook main.cpp OrderBook.cpp MarketData.cpp
./orderbook
```

## Tests

12 unit tests via Catch2 covering full fill, partial fill, price priority, time priority, cancellation, and multi-level sweeps.

```bash
g++ -std=c++17 -O2 -o tests tests.cpp OrderBook.cpp MarketData.cpp catch_amalgamated.cpp
./tests
```

---

## Project Structure

```
├── Order.h           # Order struct and Side enum
├── OrderBook.h/.cpp  # Price ladder, matching engine, cancellation
├── SPSC.h/.tpp       # Lock-free ring buffer
├── MarketData.h/.cpp # Random order generation
├── main.cpp          # Producer/consumer threads
└── tests.cpp         # Catch2 unit tests
```

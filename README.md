# Order Book

A low-latency order book implementation in C++ with a lock-free SPSC queue.

## Architecture
- **SPSC ring buffer** — lock-free single-producer single-consumer queue 
  with cache-line aligned atomics to prevent false sharing
- **Order matching engine** — price-time priority matching with O(1) 
  order lookup and cancellation via unordered_map index
- **Latency measurement** — queue latency and matching latency tracked 
  separately, p50/p99 reported at shutdown

## Build
```bash
g++ -std=c++17 -O3 -o orderbook main.cpp OrderBook.cpp MarketData.cpp
./orderbook
```

## Latency (release build, i7-12700)
| Metric | Value |
|--------|-------|
| p50    | ~1200ns |
| p99    | ~14700ns |
#include "OrderBook.h"
#include <iostream>
#include <algorithm>
#include <vector>

void OrderBook::addOrder(Order order) {
    if (order.side == Side::Buy) {
        bids[order.price].push_back(order);
        orderIndex[order.id] = {order.price, order.side};
        if (order.price > bestBid) bestBid = order.price;
    } else {
        asks[order.price].push_back(order);
        orderIndex[order.id] = {order.price, order.side};
        if (bestAsk == -1 || order.price < bestAsk) bestAsk = order.price;
    }
}


/**
 * Auto - compiler figures out what type to use and the & is memory reference as otherwise you make a copy, we use const to make it not modifiable as compiler optimises to prevent accidental writes
 * 
 * The [price ,orders ] is just same as the python for loop where you can do 
 * 
 *  pairs = [(1, "one"), (2, "two"), (3, "three")]
    for number, word in pairs:
        print(number, word)
 
    * 
    * 
 */
void OrderBook::printBook() {
    std::cout << "Bids:\n";
    std::vector<int64_t> bidPrices;
    for (const auto& [price, _] : bids) bidPrices.push_back(price);
    std::sort(bidPrices.rbegin(), bidPrices.rend()); // highest first
    for (int64_t price : bidPrices) {
        for (const auto& order : bids[price]) {
            std::cout << "ID: " << order.id << ", Price: $" << price / 100.0 << ", Qty: " << order.quantity << "\n";
        }
    }

    std::cout << "Asks:\n";
    std::vector<int64_t> askPrices;
    for (const auto& [price, _] : asks) askPrices.push_back(price);
    std::sort(askPrices.begin(), askPrices.end()); // lowest first
    for (int64_t price : askPrices) {
        for (const auto& order : asks[price]) {
            std::cout << "ID: " << order.id << ", Price: $" << price / 100.0 << ", Qty: " << order.quantity << "\n";
        }
    }
}

//asks.begin()->first   = the price    e.g. 9000
//asks.begin()->second  = the deque    e.g. [order, order, order]
void OrderBook::matchOrders(Order order) {
    auto start = std::chrono::high_resolution_clock::now();

    if (order.side == Side::Buy) {
        while (!asks.empty() && bestAsk != -1 && order.price >= bestAsk) {
            auto& askQueue = asks[bestAsk];
            Order& firstOrder = askQueue.front();

            int64_t fillQty = std::min(order.quantity, firstOrder.quantity);
            std::cout << "FILL: " << fillQty << " @ " << bestAsk / 100.0 << "\n";

            order.quantity -= fillQty;
            firstOrder.quantity -= fillQty;

            if (firstOrder.quantity == 0) {
                askQueue.pop_front();
            }
            if (askQueue.empty()) {
                asks.erase(bestAsk);
                bestAsk = findNextAsk();
            }
            if (order.quantity == 0) break;
        }
        if (order.quantity > 0) addOrder(order);
    }
    else {
        while (!bids.empty() && bestBid != -1 && order.price <= bestBid) {
            auto& bidQueue = bids[bestBid];
            Order& firstOrder = bidQueue.front();

            int64_t fillQty = std::min(order.quantity, firstOrder.quantity);
            std::cout << "FILL: " << fillQty << " @ " << bestBid / 100.0 << "\n";

            order.quantity -= fillQty;
            firstOrder.quantity -= fillQty;

            if (firstOrder.quantity == 0) {
                bidQueue.pop_front();
            }
            if (bidQueue.empty()) {
                bids.erase(bestBid);
                bestBid = findNextBid();
            }
            if (order.quantity == 0) break;
        }
        if (order.quantity > 0) addOrder(order);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto queueNs = std::chrono::duration_cast<std::chrono::nanoseconds>(start - order.timestamp).count();
    latencies.push_back(queueNs);
}

bool OrderBook::cancelOrder(int64_t id) {
    auto it = orderIndex.find(id);
    if (it == orderIndex.end()) return false;

    int64_t price = it->second.price;
    Side side = it->second.side;

    auto& book = (side == Side::Buy) ? bids : asks;
    auto& queue = book[price];

    queue.erase(
        std::remove_if(queue.begin(), queue.end(),
            [id](const Order& o) { return o.id == id; }),
        queue.end()
    );

    if (queue.empty()) {
        book.erase(price);
        if (side == Side::Buy) bestBid = findNextBid();
        else bestAsk = findNextAsk();
    }

    orderIndex.erase(it);
    return true;
}

int64_t OrderBook::findNextAsk() {
    int64_t best = -1;
    for (const auto& [price, queue] : asks) {
        if (best == -1 || price < best) best = price;
    }
    return best;
}

int64_t OrderBook::findNextBid() {
    int64_t best = -1;
    for (const auto& [price, queue] : bids) {
        if (price > best) best = price;
    }
    return best;
}

void OrderBook::printStats() {
    if(latencies.empty()) return;

    std::vector<int64_t> sorted = latencies;
    std::sort(sorted.begin(), sorted.end());

    // printStats - replace floating point index with integer arithmetic
    size_t p50 = sorted[(sorted.size() - 1) * 50 / 100];
    size_t p99 = sorted[(sorted.size() - 1) * 99 / 100];

    std::cout << "=== LATENCY STATS ===\n";
    std::cout << "Samples: " << sorted.size() << "\n";
    std::cout << "p50: " << p50 << "ns\n";
    std::cout << "p99: " << p99 << "ns\n";
}
#include "OrderBook.h"
#include <iostream>
#include <algorithm>
#include <vector>

void OrderBook::addOrder(Order order) {
    size_t idx = order.price - MIN_PRICE;
    if (order.side == Side::Buy) {
        bids[idx].orders.push_back(order);
        orderIndex[order.id] = {order.price, order.side};
        if (order.price > bestBid) bestBid = order.price;
    } else {
        asks[idx].orders.push_back(order);
        orderIndex[order.id] = {order.price, order.side};
        if (bestAsk == -1 || order.price < bestAsk) bestAsk = order.price;
    }
}


void OrderBook::reduceOrder(int64_t id, int64_t executed_shares) {
    auto it = orderIndex.find(id);
    if (it == orderIndex.end()) return; // unknown order — log/ignore

    int64_t price = it->second.price;
    Side side = it->second.side;
    size_t idx = price - MIN_PRICE;
    auto& level = (side == Side::Buy) ? bids[idx] : asks[idx];

    auto orderIt = std::find_if(level.orders.begin(), level.orders.end(),
        [id](const Order& o) { return o.id == id; });
    if (orderIt == level.orders.end()) return;

    orderIt->quantity -= executed_shares;
    if (orderIt->quantity <= 0) {
        level.orders.erase(orderIt);
        orderIndex.erase(it);
        if (level.orders.empty()) {
            if (side == Side::Buy) bestBid = findNextBid();
            else bestAsk = findNextAsk();
        }
    }
}

void OrderBook::printBook() {
    std::cout << "Bids:\n";
    for (int64_t price = MAX_PRICE; price >= MIN_PRICE; price--) {
        auto& level = bids[price - MIN_PRICE];
        for (const auto& order : level.orders) {
            std::cout << "ID: " << order.id << ", Price: $" << price / 100.0 << ", Qty: " << order.quantity << "\n";
        }
    }

    std::cout << "Asks:\n";
    for (int64_t price = MIN_PRICE; price <= MAX_PRICE; price++) {
        auto& level = asks[price - MIN_PRICE];
        for (const auto& order : level.orders) {
            std::cout << "ID: " << order.id << ", Price: $" << price / 100.0 << ", Qty: " << order.quantity << "\n";
        }
    }
}

void OrderBook::matchOrders(Order order) {
    auto start = std::chrono::high_resolution_clock::now();

    if (order.side == Side::Buy) {
        while (bestAsk != -1 && order.price >= bestAsk) {
            auto& askLevel = asks[bestAsk - MIN_PRICE];
            Order& firstOrder = askLevel.orders.front();

            int64_t fillQty = std::min(order.quantity, firstOrder.quantity);
            std::cout << "FILL: " << fillQty << " @ " << bestAsk / 100.0 << "\n";

            order.quantity -= fillQty;
            firstOrder.quantity -= fillQty;

            if (firstOrder.quantity == 0) {
                orderIndex.erase(firstOrder.id);
                askLevel.orders.pop_front();
            }
            if (askLevel.orders.empty()) {
                bestAsk = findNextAsk();
            }
            if (order.quantity == 0) break;
        }
        if (order.quantity > 0) addOrder(order);
    }
    else {
        while (bestBid != -1 && order.price <= bestBid) {
            auto& bidLevel = bids[bestBid - MIN_PRICE];
            Order& firstOrder = bidLevel.orders.front();

            int64_t fillQty = std::min(order.quantity, firstOrder.quantity);
            std::cout << "FILL: " << fillQty << " @ " << bestBid / 100.0 << "\n";

            order.quantity -= fillQty;
            firstOrder.quantity -= fillQty;

            if (firstOrder.quantity == 0) {
                orderIndex.erase(firstOrder.id);
                bidLevel.orders.pop_front();
            }
            if (bidLevel.orders.empty()) {
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
    size_t idx = price - MIN_PRICE;

    auto& level = (side == Side::Buy) ? bids[idx] : asks[idx];

    level.orders.erase(
        std::remove_if(level.orders.begin(), level.orders.end(),
            [id](const Order& o) { return o.id == id; }),
        level.orders.end()
    );

    if (level.orders.empty()) {
        if (side == Side::Buy) bestBid = findNextBid();
        else bestAsk = findNextAsk();
    }

    orderIndex.erase(it);
    return true;
}

int64_t OrderBook::findNextAsk() {
    for (int64_t price = MIN_PRICE; price <= MAX_PRICE; price++) {
        if (!asks[price - MIN_PRICE].orders.empty()) return price;
    }
    return -1;
}

int64_t OrderBook::findNextBid() {
    for (int64_t price = MAX_PRICE; price >= MIN_PRICE; price--) {
        if (!bids[price - MIN_PRICE].orders.empty()) return price;
    }
    return -1;
}

void OrderBook::printStats() {
    if (latencies.empty()) return;

    std::vector<int64_t> sorted = latencies;
    std::sort(sorted.begin(), sorted.end());

    size_t p50 = sorted[(sorted.size() - 1) * 50 / 100];
    size_t p99 = sorted[(sorted.size() - 1) * 99 / 100];

    std::cout << "=== LATENCY STATS ===\n";
    std::cout << "Samples: " << sorted.size() << "\n";
    std::cout << "p50: " << p50 << "ns\n";
    std::cout << "p99: " << p99 << "ns\n";
}
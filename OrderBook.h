#pragma once

#include <string>
#include <cstdint>
#include "Order.h"
#include <unordered_map>
#include <deque>
#include <vector>

class OrderBook {
public:
    void addOrder(Order order);
    void printBook();
    void matchOrders(Order order);
    bool cancelOrder(int64_t id);
    void printStats();

private:
    std::unordered_map<int64_t, std::deque<Order>> bids;
    std::unordered_map<int64_t, std::deque<Order>> asks;

    struct OrderLocation {
        int64_t price;
        Side side;
    };
    std::unordered_map<int64_t, OrderLocation> orderIndex;

    std::vector<int64_t> latencies;

    int64_t bestBid = -1;
    int64_t bestAsk = -1;
    int64_t findNextAsk();
    int64_t findNextBid();
};
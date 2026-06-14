#pragma once

#include <string>
#include <cstdint>
#include "Order.h"
#include <unordered_map>
#include <deque>
#include <vector>
#include <array>

class OrderBook {
public:
    void addOrder(Order order);
    void printBook();
    void matchOrders(Order order);
    bool cancelOrder(int64_t id);
    void printStats();

  


private:

    static constexpr int64_t MIN_PRICE   = 9000;   // $90.00
    static constexpr int64_t MAX_PRICE   = 11000;  // $110.00
    static constexpr size_t  LADDER_SIZE = MAX_PRICE - MIN_PRICE + 1;

    struct PriceLevel {
        std::deque<Order> orders;
    };
        std::array<PriceLevel, LADDER_SIZE> bids;
        std::array<PriceLevel, LADDER_SIZE> asks;

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
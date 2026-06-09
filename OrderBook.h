#pragma once
#include <string>
#include <cstdint>
#include "Order.h"
#include <map>
#include <unordered_map>
#include <deque>


struct PriceLevel{
    std::deque<Order> orders;
    bool active = false;
};

static constexpr int64_t MIN_PRICE = 1;
static constexpr int64_t MAX_PRICE = 1000000;
static constexpr size_t LADDER_SIZE = MAX_PRICE - MIN_PRICE - 1;


std::array<PriceLevel, LADDER_SIZE> bids;
std::array<PriceLevel, LADDER_SIZE> asks;


class OrderBook {
public: //everything written after will be public

    // searching is O(log n)
    std::unordered_map<int64_t, std::deque<Order> > bids; // highest first
    std::unordered_map<int64_t, std::deque<Order> > asks; //lowest first -> deque is double ended queue (can add/remove from both ends ), instead of using a vector which is slow to move from front

    void addOrder(Order order);
    void printBook();
    void matchOrders(Order order); //match orders and remove from book

    void printStats();
    std::vector<int64_t> latencies;


private:
    int64_t bestBid = -1;
    int64_t bestAsk = -1;
    int64_t findNextAsk();
    int64_t findNextBid();
};




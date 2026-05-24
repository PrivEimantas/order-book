#pragma once
#include <string>
#include <cstdint>
#include "Order.h"
#include <map>
#include <deque>



class OrderBook {
public: //everything written after will be public
    std::map<int64_t, std::deque<Order>, std::greater<int64_t>> bids; // highest first
    std::map<int64_t, std::deque<Order>> asks; //lowest first -> deque is double ended queue (can add/remove from both ends ), instead of using a vector which is slow to move from front

    void addOrder(Order order);
    void printBook();
    void matchOrders(Order order); //match orders and remove from book

};




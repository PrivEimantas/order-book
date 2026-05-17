#pragma once
#include <string>
#include <cstdint>

enum class Side {Buy, Sell};

struct Order{
    int64_t id;
    Side side;
    double price;
    int quantity;
};
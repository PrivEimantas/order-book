#pragma once
#include <string>
#include <cstdint>

enum class Side {Buy, Sell};

struct Order{

    int64_t id;
    Side side;
    int64_t price; //we dont want double for price due non-deterministic across platforms and has rounding issues
    int64_t quantity;
};
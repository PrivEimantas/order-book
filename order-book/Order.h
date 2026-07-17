#pragma once
#include <string>
#include <cstdint>
#include <chrono>
#include <vector>

enum class Side {Buy, Sell};

struct Order{

    int64_t id;
    Side side;
    int64_t price; //we dont want double for price due non-deterministic across platforms and has rounding issues
    int64_t quantity;

    std::chrono::high_resolution_clock::time_point timestamp; 

     
};
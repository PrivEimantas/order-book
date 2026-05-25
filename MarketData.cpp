#pragma once
#include <string>
#include <cstdint>
#include "Order.h"
#include <random>

static std::mt19937 rng(std::random_device{}());
static std::uniform_int_distribution<int64_t> priceDist(9800, 10200);
static std::uniform_int_distribution<int64_t> qtyDist(1, 100);
static std::uniform_int_distribution<int64_t> sideDist(0, 1);


Order generateRandomData(){
    Order o;
    o.id = priceDist(rng);        
    o.price = priceDist(rng);
    o.quantity = qtyDist(rng);
    o.side = sideDist(rng) == 1 ? Side::Buy : Side::Sell;
    return o;

    return o;
}
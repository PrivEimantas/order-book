#include <string>
#include <cstdint>
#include "Order.h"
#include <random>

static std::mt19937 rng(std::random_device{}());
static std::uniform_int_distribution<int64_t> priceDist(9800, 10200);
static std::uniform_int_distribution<int64_t> qtyDist(1, 100);
static std::uniform_int_distribution<int64_t> sideDist(0, 1);


static std::uniform_int_distribution<int64_t> bidPriceDist(9900, 10200);  // buys bid higher
static std::uniform_int_distribution<int64_t> askPriceDist(9800, 10100);  // sells ask lower

Order generateRandomData(int64_t id) {
    Order o;
    o.id = id;
    o.side = sideDist(rng) == 1 ? Side::Buy : Side::Sell;
    o.price = (o.side == Side::Buy) ? bidPriceDist(rng) : askPriceDist(rng);
    o.quantity = qtyDist(rng);
    o.timestamp = std::chrono::high_resolution_clock::now();
    return o;
}
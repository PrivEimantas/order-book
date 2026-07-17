#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"
#include "OrderBook.h"

// helper so we're not typing this out every time
static Order makeOrder(int64_t id, Side side, int64_t price, int64_t qty) {
    Order o;
    o.id = id;
    o.side = side;
    o.price = price;
    o.quantity = qty;
    o.timestamp = std::chrono::high_resolution_clock::now();
    return o;
}

// --- basic matching ---

TEST_CASE("Full fill - both sides completely satisfied") {
    OrderBook book;

    book.addOrder(makeOrder(1, Side::Sell, 10000, 50));
    book.matchOrders(makeOrder(2, Side::Buy, 10000, 50));

    // book should be empty - cancel should fail for both
    REQUIRE(book.cancelOrder(1) == false);
    REQUIRE(book.cancelOrder(2) == false);
}

TEST_CASE("Partial fill - buyer wants more than available") {
    OrderBook book;

    book.addOrder(makeOrder(1, Side::Sell, 10000, 30));
    book.matchOrders(makeOrder(2, Side::Buy, 10000, 100));

    // ask should be gone
    REQUIRE(book.cancelOrder(1) == false);
    // buyer's remainder (70) should be resting in book
    REQUIRE(book.cancelOrder(2) == true);
}

TEST_CASE("Partial fill - seller has more than buyer wants") {
    OrderBook book;

    book.addOrder(makeOrder(1, Side::Sell, 10000, 100));
    book.matchOrders(makeOrder(2, Side::Buy, 10000, 30));

    // buyer is satisfied and gone
    REQUIRE(book.cancelOrder(2) == false);
    // seller still has 70 remaining
    REQUIRE(book.cancelOrder(1) == true);
}

TEST_CASE("No match - bid below ask rests in book") {
    OrderBook book;

    book.addOrder(makeOrder(1, Side::Sell, 10100, 50));
    book.matchOrders(makeOrder(2, Side::Buy, 9900, 50));

    // prices don't cross so both should be resting
    REQUIRE(book.cancelOrder(1) == true);
    REQUIRE(book.cancelOrder(2) == true);
}

TEST_CASE("Price priority - buyer matches cheapest ask first") {
    OrderBook book;

    // two asks at different prices
    book.addOrder(makeOrder(1, Side::Sell, 10200, 50));
    book.addOrder(makeOrder(2, Side::Sell, 10000, 50)); // cheaper, should fill first

    book.matchOrders(makeOrder(3, Side::Buy, 10200, 50));

    // cheaper ask (id 2) should be gone
    REQUIRE(book.cancelOrder(2) == false);
    // expensive ask (id 1) should still be there
    REQUIRE(book.cancelOrder(1) == true);
}

TEST_CASE("Time priority - orders at same price fill in FIFO order") {
    OrderBook book;

    book.addOrder(makeOrder(1, Side::Sell, 10000, 50)); // arrives first
    book.addOrder(makeOrder(2, Side::Sell, 10000, 50)); // arrives second

    // only enough to fill one
    book.matchOrders(makeOrder(3, Side::Buy, 10000, 50));

    // first order should be gone
    REQUIRE(book.cancelOrder(1) == false);
    // second order should still be waiting
    REQUIRE(book.cancelOrder(2) == true);
}

// --- cancellation ---

TEST_CASE("Cancel removes a resting bid") {
    OrderBook book;

    book.addOrder(makeOrder(1, Side::Buy, 9900, 50));
    REQUIRE(book.cancelOrder(1) == true);
    REQUIRE(book.cancelOrder(1) == false); // second cancel should do nothing
}

TEST_CASE("Cancel removes a resting ask") {
    OrderBook book;

    book.addOrder(makeOrder(1, Side::Sell, 10100, 50));
    REQUIRE(book.cancelOrder(1) == true);
    REQUIRE(book.cancelOrder(1) == false);
}

TEST_CASE("Cancel nonexistent order returns false") {
    OrderBook book;
    REQUIRE(book.cancelOrder(999) == false);
}

TEST_CASE("Cancel one of multiple orders at same price level") {
    OrderBook book;

    book.addOrder(makeOrder(1, Side::Sell, 10000, 20));
    book.addOrder(makeOrder(2, Side::Sell, 10000, 30));
    book.addOrder(makeOrder(3, Side::Sell, 10000, 40));

    REQUIRE(book.cancelOrder(2) == true);

    // other two should still be there
    REQUIRE(book.cancelOrder(1) == true);
    REQUIRE(book.cancelOrder(3) == true);
}

// --- edge cases ---

TEST_CASE("Match against multiple price levels") {
    OrderBook book;

    book.addOrder(makeOrder(1, Side::Sell, 10000, 20));
    book.addOrder(makeOrder(2, Side::Sell, 10100, 20));
    book.addOrder(makeOrder(3, Side::Sell, 10200, 20));

    // buyer willing to pay up to 10200, should sweep all three levels
    book.matchOrders(makeOrder(4, Side::Buy, 10200, 60));

    REQUIRE(book.cancelOrder(1) == false);
    REQUIRE(book.cancelOrder(2) == false);
    REQUIRE(book.cancelOrder(3) == false);
    REQUIRE(book.cancelOrder(4) == false); // fully filled
}

TEST_CASE("Sell order matches against multiple bid levels") {
    OrderBook book;

    book.addOrder(makeOrder(1, Side::Buy, 10200, 20));
    book.addOrder(makeOrder(2, Side::Buy, 10100, 20));
    book.addOrder(makeOrder(3, Side::Buy, 10000, 20));

    // seller willing to go as low as 10000, sweeps all three
    book.matchOrders(makeOrder(4, Side::Sell, 10000, 60));

    REQUIRE(book.cancelOrder(1) == false);
    REQUIRE(book.cancelOrder(2) == false);
    REQUIRE(book.cancelOrder(3) == false);
    REQUIRE(book.cancelOrder(4) == false);
}

#include <iostream>
#include "Order.h"
#include "Share.h"
#include "OrderBook.h"
#include "SPSC.h"
#include "SPSC.tpp"
#include <thread>
// argument is a constant string from std and we reference its memory address to avoid copying the string, so we dont actually modify the string that we passed in
// you can pass in a non-const reference if you want to modify the string, but in this case we just want to read it and convert it to an enum, so we use a const reference
//default is by value, so if you pass in it becomes duplicated so we waste memory and time, so we use reference to avoid that, and const to prevent modification of the original string
Side parseSide(const std::string& input) {
    if (input == "Buy") return Side::Buy;
    if (input == "Sell") return Side::Sell;
    throw std::invalid_argument("Invalid side: " + input);
}


Order setup_order(int& currentI)
{
    Order userOrder;
    std::cout << "Enter order details ( side, price, quantity): ";
    int id;
    std::string sideStr;
    int64_t price;
    int quantity;
        
    std::cin >> sideStr >> price >> quantity;

    userOrder.id = currentI++;
    userOrder.side = parseSide(sideStr);
    userOrder.price = price;
    userOrder.quantity = quantity;

    return userOrder;
}


void quickTest()
{
    
    SPSC<Order> buffer(10);

    Order o;
    o.id = 1;
    o.price = 10000;
    o.quantity = 5;
    o.side = Side::Buy;

    buffer.push(o);

    Order result;
    buffer.pop(result);
    std::cout << result.price << "\n"; // should print 10000

}

int main(){

    SPSC<Order> buffer(10);

    // producer thread - pushes 5 orders
    std::thread producer([&]() {
        for (int i = 0; i < 5; i++) {
            Order o;
            o.id = i;
            o.price = 10000 + (i * 100);
            o.quantity = i + 1;
            o.side = Side::Buy;
            while (!buffer.push(o)) {}  // spin until space available
            std::cout << "Pushed order " << o.id << "\n";
        }
    });

    // consumer thread - pops 5 orders
    std::thread consumer([&]() {
        for (int i = 0; i < 5; i++) {
            Order result;
            while (!buffer.pop(result)) {}  // spin until item available
            std::cout << "Popped order " << result.id << " price: " << result.price << "\n";
        }
    });

    producer.join();
    consumer.join();

    return 0;
}
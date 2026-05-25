#include <iostream>
#include "Order.h"
#include "Share.h"
#include "OrderBook.h"
#include "SPSC.h"
#include "SPSC.tpp"
#include <thread>
#include "MarketData.cpp"
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
    OrderBook book;
    std::atomic<bool> running(true);

    // producer thread - pushes 5 orders
    std::thread producer([&]() {
        int i =0;
        while(running){

            Order order = generateRandomData(i++);
            while(!buffer.push(order)){}

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
        };
    });

    // consumer thread - pops 5 orders
    std::thread consumer([&]() {
        while(running){
            Order result;
            if(buffer.pop(result)){
                book.matchOrders(result);
                book.printBook();
            }
        }
    });


    std::this_thread::sleep_for(std::chrono::seconds(5));
    running = false;

    producer.join();
    consumer.join();

    book.printStats();

    return 0;
}
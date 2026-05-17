#include <iostream>
#include "Order.h"
#include "Share.h"

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
    double price;
    int quantity;
        
    std::cin >> sideStr >> price >> quantity;

    userOrder.id = currentI++;
    userOrder.side = parseSide(sideStr);
    userOrder.price = price;
    userOrder.quantity = quantity;

    return userOrder;
}

int main() {
    std::cout << "Order book starting" << std::endl;

    Order buyArray[5];
    Order sellArray[5];
    Order userOrder;
    int currentID = 0;
    
    while (true) {
        std::string decision;
        std::cout << "-----------------------------" << std::endl;
        std::cout << "Buy or Sell? (type 'exit' to quit)" << std::endl;

        
        std::cin >> decision;

        if (decision == "exit") {
            std::cout << "Exiting order book." << std::endl;
            break;
        }

        else if (decision == "Buy"){
            std::cout << "You chose to Buy." << std::endl;
            userOrder = setup_order(currentID);
            buyArray[userOrder.id % 5] = userOrder; // Simple way to store orders in a circular buffer

        }
        else if (decision == "Sell"){
            std::cout << "You chose to Sell." << std::endl;
            userOrder = setup_order(currentID);
            sellArray[userOrder.id % 5] = userOrder; // Simple way to store orders in a circular buffer
        }
        else {
            std::cout << "Invalid choice. Please enter 'Buy', 'Sell', or 'exit'." << std::endl;
            continue; // Skip the rest of the loop and ask again
        }


        std::cout << "Order added: " << userOrder.id 
                   << " | " << (userOrder.side == Side::Buy ? "Buy" : "Sell")
                   << " | $" << userOrder.price << " | x" << userOrder.quantity << std::endl;
    
        
    }
    return 0;
}

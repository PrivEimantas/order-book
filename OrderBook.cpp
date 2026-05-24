#include "OrderBook.h"
#include <iostream>
#include <algorithm>


void OrderBook::addOrder(Order order) {
    if (order.side == Side::Buy) {
        bids[order.price].push_back(order);
    } else {
        asks[order.price].push_back(order);
    }
}


/**
 * Auto - compiler figures out what type to use and the & is memory reference as otherwise you make a copy, we use const to make it not modifiable as compiler optimises to prevent accidental writes
 * 
 * The [price ,orders ] is just same as the python for loop where you can do 
 * 
 *  pairs = [(1, "one"), (2, "two"), (3, "three")]
    for number, word in pairs:
        print(number, word)
 
    * 
    * 
 */
void OrderBook::printBook() {
    std::cout << "Order Book:" << std::endl;
    std::cout << "Bids:" << std::endl;
    for (const auto& [price, orders] : bids) {
        for (const auto& order : orders) {
            std::cout << "ID: " << order.id << ", Price: " << order.price << ", Quantity: " << order.quantity << std::endl;
        }
    }
    std::cout << "Asks:" << std::endl;
    for (const auto& [price, orders] : asks) {
        for (const auto& order : orders) {
            std::cout << "ID: " << order.id << ", Price: " << order.price << ", Quantity: " << order.quantity << std::endl;
        }
    }
}

//asks.begin()->first   = the price    e.g. 9000
//asks.begin()->second  = the deque    e.g. [order, order, order]

void OrderBook::matchOrders(Order order){
    if(order.side == Side::Buy)
    {

        while(!asks.empty() && order.price >= asks.begin()->first)
        {
            int64_t sellingPrice = asks.begin()->first; // get the price at the first valid option
            auto& askQueue = asks.begin()->second; // get the list of sellers for that price 
            Order& firstOrder = askQueue.front(); //get the first seller
            
            int32_t fillqty = std::min(order.quantity,firstOrder.quantity); //take as many shares as possible, e.g. we want 20 and they have 10 or 30 we need to know who is satisfied

            std::cout << "FILL: " << fillqty << " @ " << sellingPrice / 100.0 << "\n";
            
            order.quantity = order.quantity - fillqty; // are we satisfied?
            firstOrder.quantity = firstOrder.quantity - fillqty; // has the seller sold all their shares?
             
            if(firstOrder.quantity==0) //if so then get rid of it from the queue
            {
                askQueue.pop_front();
            }

            if(askQueue.empty()){ //no sellers left
                asks.erase(asks.begin());
            }

            if(order.quantity==0){ //we are satisfied so we continue
                break;
            }

        }
        if(order.quantity > 0) //if no sellers left but still wanting to buy we 'write' this into our book
        {
            addOrder(order);
        }
    }
    else //same logic but reverse the seller/buyer
    {
        while(!bids.empty() && order.price <= bids.begin()->first)
        {
            int64_t buyingPrice = bids.begin()->first;
            auto& bidQueue = bids.begin()->second;
            Order& firstOrder = bidQueue.front();

            int32_t fillQty = std::min(order.quantity, firstOrder.quantity);

            std::cout << "FILL: " << fillQty << " @ " << buyingPrice / 100.0 << "\n";

            order.quantity -= fillQty;
            firstOrder.quantity -= fillQty;

            if(firstOrder.quantity == 0){
                bidQueue.pop_front();
            }
            if(bidQueue.empty()){
                bids.erase(bids.begin());
            }
            if(order.quantity == 0){
                break;
            }
        }

        if(order.quantity > 0)
        {
            addOrder(order);
        }
        
    }

    
}
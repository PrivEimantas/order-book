#pragma once
#include <string>
#include <cstdint>
#include "Order.h"
#include <map>
#include <deque>
#include "SPSC.h"
#include <iostream>
#include <stdexcept>



//only need to use SPSC:: if outside class defining a method, since were interacting directly here we dont need it

// SPSC.tpp
template <typename T>
bool SPSC<T>::push(const T& item) {

    if (  ((tailPtr +1) % capacity) == headPtr   ){
        return false;
    }

    else{
        arr[tailPtr]=item;
        tailPtr = (tailPtr + 1) % capacity;
        return true;
    }

}

template <typename T>
bool SPSC<T>::pop( T& item){ //the consumer passes in an empty order which gets populated hence we need the argument
    if(headPtr== tailPtr){
        return false;
    }
    item = arr[headPtr];
    headPtr = (headPtr+1)%capacity;
    return true;
}




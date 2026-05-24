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



/*
TEMPLATE EXPLANATION - Right now you're building a buffer for Order objects. 
But a ring buffer is a general concept — there's nothing about it that's specific to orders. 
You could use it to pass strings, integers, market data ticks, anything.template <typename T> 
means "don't hardcode the type, let the caller decide." When you write:

*/


/* WHAT IS const T&
Two things combined:
T& — pass by reference. Instead of copying the entire Order into the function, you just pass a pointer to the original. Cheaper, especially for large objects.
const — you're promising not to modify it. The producer is just reading the item to copy it into the buffer. It has no business modifying the caller's order.
So const T& means "give me a reference to your item so I don't have to copy it, and I promise I won't touch it."
*/

/*
memory_order_relaxed — no restrictions at all. Just do the atomic load/store, don't add any fences. Used when you're reading your own index and don't need any synchronisation guarantee from it.
memory_order_release — "everything I wrote before this store must be visible to other threads before this store becomes visible." Used on the tail store in push — guarantees the data write happens before the tail update is published.
memory_order_acquire — "don't let any reads after this load be reordered before it." Used when reading the other side's index — guarantees you see everything the other thread wrote before its release store.

*/


// SPSC.tpp
template <typename T>
bool SPSC<T>::push(const T& item) {

    size_t tail = tailPtr.load(std::memory_order_relaxed);
    size_t nextTail = (tail + 1) % capacity;

    if (nextTail == headPtr.load(std::memory_order_acquire)) return false;
    arr[tail] = item;
    tailPtr.store(nextTail, std::memory_order_release);
    return true;

}

template <typename T>
bool SPSC<T>::pop( T& item){ //the consumer passes in an empty order which gets populated hence we need the argument

    size_t head = headPtr.load(std::memory_order_relaxed);
    if (head == tailPtr.load(std::memory_order_acquire)) return false;
    item = arr[head];
    headPtr.store((head + 1) % capacity, std::memory_order_release);
    return true;
}




#pragma once
#include <string>
#include <cstdint>
#include "Order.h"
#include <map>
#include <deque>


template <typename T>
class SPSC{
    private:
        size_t capacity=0;
        T* arr;
        size_t headPtr=0;
        size_t tailPtr=0;

    public:

        SPSC(size_t n) : capacity(n){
            arr = new T[n];
            capacity=n;
        }

        ~SPSC() {
            delete[] arr;
        }   

        bool push(const T& item); //producer calls
        bool pop(T& item); //consumer calls

        size_t getSize() const {
            return capacity;
        }
        
        T* getArray(){
            return arr;
        }

        // no need if just accessing own class's private variables
        // size_t getHeadPtr(){
        //     return headPtr;
        // }

        // size_t getTailPtr(){
        //     return tailPtr;
        // }

};
#include "SPSC.tpp"
cmake_minimum_required(VERSION 3.16)
project(OrderBook)

set(CMAKE_CXX_STANDARD 17)

add_executable(orderbook main.cpp OrderBook.cpp MarketData.cpp)
target_compile_options(orderbook PRIVATE -O3 -march=native)
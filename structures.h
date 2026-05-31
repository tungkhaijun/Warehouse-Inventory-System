// structures.h
#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <iostream>
#include <string>

// 统一的商品结构体
struct Product {
    std::string productId;
    std::string productName;
    int quantity;
    double price;
    std::string location;
    std::string supplier;
};

#endif
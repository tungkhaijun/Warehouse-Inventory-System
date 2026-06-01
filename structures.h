// structures.h
#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <iostream>
#include <string>
using namespace std;

// 统一的商品结构体
struct Product {
    string productId;
    string productName;
    int quantity;
    double price;
    string location;
    string supplier;
};

#endif
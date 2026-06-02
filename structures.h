// structures.h
#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <iostream>
#include <string>
using namespace std;

//for the product data
struct Product {
    int productId;
    string productName;
    string category;
    int stockQuantity;
    string zone;
    string supplier;
    double productPrice;
    
    Product* next;
};

//for the order data
struct Order {
    int orderId;
    int productId;
    int dispatchQuantity;
    string operatorName;
    string orderDate;
    
    Order* next;
};

//for the user's data
struct User {
    string username;
    string password; 
    string role; // User,Admin,Manager
    
    User* next; 
};

#endif

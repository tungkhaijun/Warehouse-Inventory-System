// structures.h
#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <iostream>
#include <string>

using namespace std;


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

struct Order {
    int orderId;
    int productId;
    int categoryId;
    int dispatchQuantity;
    string operatorName;
    string orderDate;
    
    Order* next;
};


class User {
public:
    string username, password, role;
    User* next;
    friend void printUserProfile(const User& u);

    User(string un, string pw, string r) : username(un), password(pw), role(r), next(NULL) {}
    virtual ~User() {}

    virtual void displayMenu() = 0; 
};

#endif
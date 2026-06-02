#ifndef INVENTORY_CORE_H
#define INVENTORY_CORE_H

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
    int dispatchQuantity;
    string operatorName;
    string orderDate;
    
    Order* next;
};

class User {
protected:
    string username;
    string password; 
    string role; // User,Admin,Manager
    
    User* next; 
public:
    User(string un, string pw, string r);
    virtual ~User(); 
    virtual void displayMenu() = 0; 
};

class Admin : public User {
public:
    Admin(string un);
    ~Admin();
    void displayMenu() override;
};

class Manager : public User {
public:
    Manager(string un);
    ~Manager();
    void displayMenu() override;
};

class ProductLinkedList {
private:
    Product* head;
    int count;

    void Merge(Product** arr, int lpos, int rpos, int rend, int sortBy);
    void MergeSortRec(Product** arr, int left, int right, int sortBy);

public:
    ProductLinkedList();
    ~ProductLinkedList();

    void insertNode(int id, string name, string cat, int qty, string z, string sup, double price);
    void deleteNode(int id);
    void sortList(int sortBy);
    Product* binarySearch(int targetId);
    void display();
    
    Product* getHead() const { return head; } 
    int getCount() const { return count; }    
};

#endif 
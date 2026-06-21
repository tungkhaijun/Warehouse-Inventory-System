#ifndef INVENTORY_CORE_H
#define INVENTORY_CORE_H

#include <iostream>
#include <string>
#include "structures.h"

using namespace std;


class ProductLinkedList {
private:
    Product* head;
    int currentSort;
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
    friend void displayInventoryForCustomer(const ProductLinkedList& inv);
    
    Product* getHead() const { return head; } 
    int getCount() const { return count; }    
    friend void generateStockSummary(const ProductLinkedList& inv);
};

#endif 
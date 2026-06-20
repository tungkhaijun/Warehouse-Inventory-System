#ifndef ADMIN_H
#define ADMIN_H

#include <iostream>
#include <string>
#include "structures.h"
#include "backend.h"

using namespace std;

// =========================================================
// File I/O Engine Declarations
// =========================================================
void saveInventoryToFile(ProductLinkedList& inventory);
void saveAdminsToFile();
void loadInventoryFromFile(ProductLinkedList& inventory);
void loadAdminsFromFile();

// =========================================================
// 1. Admin Class
// =========================================================
class Admin : public User {
public:
    Admin(string un, string pw);
    ~Admin();
    
    void displayMenu();
    
    void addProduct(ProductLinkedList& inventory);
    void updateStock(ProductLinkedList& inventory);
    void sortAndDisplay(ProductLinkedList& inventory);
    void searchProduct(ProductLinkedList& inventory);
    void generateLowStockReport(ProductLinkedList& inventory);
};

// =========================================================
// 2. SuperAdmin Class
// =========================================================
class SuperAdmin : public User {
public:
    SuperAdmin(string un, string pw);
    ~SuperAdmin();
    
    void displayMenu();
    
    void addProduct(ProductLinkedList& inventory);
    void updateStock(ProductLinkedList& inventory);
    void deleteProduct(ProductLinkedList& inventory);
    void sortAndDisplay(ProductLinkedList& inventory);
    void searchProduct(ProductLinkedList& inventory);
    void addAdmin();
    void generateLowStockReport(ProductLinkedList& inventory);
};

#endif
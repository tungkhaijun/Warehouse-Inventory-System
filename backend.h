#ifndef INVENTORY_CORE_H
#define INVENTORY_CORE_H

#include <iostream>
#include <string>

using namespace std;

// ==========================================
// 1. 底层核心结构体定义
// ==========================================
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

// ==========================================
// 2. OOP 用户架构声明
// ==========================================
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

// ==========================================
// 3. 动态核心链表类声明
// ==========================================
class ProductLinkedList {
private:
    Product* head;
    int count;

    // 私有辅助算法声明
    void Merge(Product** arr, int lpos, int rpos, int rend, int sortBy);
    void MergeSortRec(Product** arr, int left, int right, int sortBy);

public:
    ProductLinkedList();
    ~ProductLinkedList();

    // 暴露给其他组员调用的公共接口
    void insertNode(int id, string name, string cat, int qty, string z, string sup, double price);
    void deleteNode(int id);
    void sortList(int sortBy);
    Product* binarySearch(int targetId);
    void display();
    
    // 建议加上这两个，方便组员获取链表状态
    Product* getHead() const { return head; } 
    int getCount() const { return count; }    
};

#endif // 结束头文件保护
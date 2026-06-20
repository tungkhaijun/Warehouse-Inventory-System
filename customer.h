#ifndef CUSTOMER_H
#define CUSTOMER_H

#include<iostream>
#include<string>
//#include"backend.h"
#include"structures.h"

using namespace std;

struct Order;

// =========================================================
// File I/O Engine Declarations -- Customer accounts only.
// (Admin/SuperAdmin accounts are declared in admin.h)
// =========================================================
void saveCustomersToFile();
void loadCustomersFromFile();

class Customer : public User{
	private:
		Order* head;
		Order* tail;
		
	public:
		Customer(string username = "", string password =""); 
		~Customer();
		
		

void addOrder();
void displayOrders();
void searchOrder();
void editOrder();
void deleteOrder();
void generateReport();
void sortOrders(int criteria);
void displayMenu();
};

#endif
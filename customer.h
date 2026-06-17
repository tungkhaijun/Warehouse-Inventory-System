#ifndef CUSTOMER_H
#define CUSTOMER_H

#include<iostream>
#include<string>
//#include"backend.h"
#include"structures.h"

using namespace std;

struct Order;

class Customer : public User{
	private:
		Order* head;
		Order* tail;
		
		string username;
		string password;
		
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

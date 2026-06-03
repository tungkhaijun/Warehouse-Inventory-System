#ifndef CUSTOMER_H
#define CUSTOMER_H

#include<iostream>
#include<string>
#include"backend.h"

using namespace std;

class Customer : public User{
	private:
		Order* head;
		Order* tail;
	public:
		Customer(string u, string p) : User(u, p, "Manager") {
		head = nullptr;
		tail = nullptr; }
		
		~Customer(){
			Order* current = head;
			while (current != nullptr){
				Order* nextNode = current->next;
				delete current;
				current = nextNode;
			}
		}
		
void displayMenu() override;

void addOrder();
void editOrder();
void deleteOrder();
void displayOrders();
void searchOrder();
void generateReport();
void sortOrders(int criteria);
};

#endif
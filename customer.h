#ifndef CUSTOMER_H
#define CUSTOMER_H

#include<iostream>
#include<string>

using namespace std;

struct booking {
	int bookingid;
	int productid;
	int quantity;
	string orderdate;
	
	booking* next;
};

class user{
	private:
		string username;
		string password;
		
	public:
		user(string u, string p) : username(u),password(p){}
		virtual void displayMenu() = 0;
		virtual ~user(){}
};

class customer : public user{
	private:
		booking* head;
		booking* tail;
	
	public:
		customer(string u, string p) : user(u, p) {
		head = nullptr;
		tail = nullptr; }
		
		~customer(){
			booking* current = head;
			while (current != nullptr){
				booking* nextNode = current->next;
				delete current;
				current = nextNode;
			}
		}
		
void displayMenu() override;

void addbooking();
void editbooking();
void displaybookings();
void searchbooking();
void generatereport();
void sortbookings(int criteria);
};

#endif
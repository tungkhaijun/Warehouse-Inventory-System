#include<iostream>
#include<string>
#include <fstream>
#include"customer.h"

using namespace std;
void Customer::addOrder(){
	Order* newOrder = new Order;
	
	cout<<"\n-- Create New Order --\n";
	cout<<"Enter Order ID (e.g: 1000): ";
	cin >> newOrder->orderId;
	
	cout<<"Enter Product ID you want to order?: ";
	cin >> newOrder->productId;
	
	cout<<"Enter Quantity: ";
	cin >> newOrder->dispatchQuantity;
	
	cout<<"Enter Order Date: ";
	cin >> newOrder->orderDate;
	
	newOrder->next = nullptr;
	
	if(head == nullptr){
		head = newOrder;
		tail = newOrder;
	} else{
		tail->next = newOrder;
		tail = newOrder;
	}
	
	cout << "\n[SUCCESS] Order ID"<<newOrder->orderId<<"has been successfully added to the system!\n";
}

void Customer::displayOrders(){
	cout<<"\n-- My Order Records --\n";
	
	if(head == nullptr){
		cout<<"[INFO] You have no order. Go to order!\n";
		return;
	}
	
	Order* current = head;
	int recordcount = 1;
	
	while(current != nullptr){
		cout<<"Record #"<< recordcount<<"\n";
		cout << "  Order ID : " << current->orderId << "\n";
        cout << "  Product ID : " << current->productId << "\n";
        cout << "  Quantity   : " << current->dispatchQuantity << "\n";
        cout << "  Order Date : " << current->orderDate << "\n";
        cout << "---------------------------------\n";
        
        current = current->next;
        recordcount++;
	}
	cout <<"[SYSTEM] End of records.\n";
}

void Customer::searchOrder() {
	cout<<"\n-- Search Order Record --\n";
	
	if(head == nullptr){
		cout << "[INFO] System id empty. No orders to search.\n";
		return;
	}
	
	int targetId;
	cout<< "Enter the Order ID you want to search: ";
	cin>> targetId;
	
	Order*current = head;
	bool found = false;
	
	while(current != nullptr){
		if (current->orderId == targetId){
			cout << "\n[SUCCESS] Record Found!\n";
            cout << "---------------------------------\n";
            cout << "  Order ID : " << current->orderId << "\n";
            cout << "  Product ID : " << current->productId << "\n";
            cout << "  Quantity   : " << current->dispatchQuantity << "\n";
            cout << "  Order Date : " << current->orderDate << "\n";
            cout << "---------------------------------\n";
            found = true;
            break;
		}
		current = current->next;
	}
	if(!found){
		cout << "\n[ERROR] Order ID " << targetId << " not found in the system.\n";
	}
}

void Customer::editOrder(){
	cout << "\n--- Edit Booking Details ---\n";
	if (head == nullptr) {
   	cout<<"[INFO] System is empty. No bookings to edit.\n";
	return;
}

int targetId;
cout<< "Enter the Booking ID you want to edit: ";
cin>>targetId;

Order* current = head;
bool found = false;

while(current != nullptr){
	if(current->orderId == targetId){
		cout<<"\n[RECORD FOUND]\n";
		cout<<"Current Quantity for Product " << current->productId << " is: " << current->dispatchQuantity << "\n";
		
		int newQuantity;
		cout<<"Enter NEW Quantity: ";
		cin>>newQuantity;
		
		if (newQuantity<0){
			cout<<"[ERROR] Quantity cannot be negative. Update aborted.\n";
		} else{
			current->dispatchQuantity = newQuantity;
			cout<<"[SUCCESS] Order ID " << targetId << " has been updated successfully!\n";
		}
		found = true;
		break;
	}
	current = current->next;
}
if(!found){
	cout << "\n[ERROR] Order ID " << targetId << " not found in the system.\n";
}
}

void Customer::generateReport(){
	cout<< "\n--- Generating Order Summary Report ---\n";
	
	if(head == nullptr){
		cout<< "[INFO] System is empty. No orders to report.\n";
		return;
	}
	
	cout << "[SYSTEM] Sorting records by Quantity (Highest to Lowest)...\n";
    sortOrders(2);
	
	ofstream outFile("OrderSummary.txt");
	
	if(!outFile){
		cout<<"[ERROR] Unable to create report file!\n";
		return;
	}
	
	outFile << "=========================================\n";
    outFile << "        Order SUMMARY REPORT           \n";
    outFile << "=========================================\n";
    
    Order* current = head;
    while (current!= nullptr){
    	outFile << "Order ID: " << current->orderId
                << " | Product ID: " << current->productId
                << " | Quantity: " << current->dispatchQuantity
                << " | Date: " << current->orderDate << "\n";
                
        current = current->next;
	}
	
	outFile.close();
	cout << "[SUCCESS] Report successfully saved to 'OrderSummary.txt'!\n";
	cout << "\n[SYSTEM] Retrieving data from text file...\n\n";
    
    ifstream inFile("OrderSummary.txt");
    string line;
    
    while(getline(inFile,line)){
    	cout << line << "\n";
	}
	
	inFile.close();
}

void Customer::sortOrders(int criteria) {
    if (head == nullptr || head->next == nullptr) {
        return;
    }

    bool swapped;
    Order* current;
    Order* lastPtr = nullptr;

    do {
        swapped = false;
        current = head;

        while (current->next != lastPtr) {
            bool needswap = false;

            if (criteria == 1) {
                if (current->orderId > current->next->orderId) {
                    needswap = true;
                }
            }
            else if (criteria == 2) {
                if (current->dispatchQuantity < current->next->dispatchQuantity) {
                    needswap = true;
                }
            }

            if (needswap) {
                int tempId = current->orderId;
                current->orderId = current->next->orderId;
                current->next->orderId = tempId;

                int tempprodId = current->productId;
                current->productId = current->next->productId;
                current->next->productId = tempprodId;

                int tempQty = current->dispatchQuantity;
                current->dispatchQuantity = current->next->dispatchQuantity;
                current->next->dispatchQuantity = tempQty;

                string tempDate = current->orderDate;
                current->orderDate = current->next->orderDate;
                current->next->orderDate = tempDate;

                swapped = true;
            }
            current = current->next;
        }
        lastPtr = current;
    } while (swapped);
    
    cout << "\n[SUCCESS] Records sorted successfully!\n";
}



void Customer::displayMenu(){
	int choice = 0;
	bool keepRunning = true;
	
	while(keepRunning){
		cout<<"\n==============================\n";
		cout<<"\n       Customer Dashboard     \n";
		cout<<"\n==============================\n";
		cout<<"1. Make a New Order\n";
		cout<<"2. Edit Order Details\n";
		cout<<"3. Delete the Orders\n";
		cout<<"4. View the Orders\n";
		cout<<"5. Search Orders\n";
		cout<<"6. Generate Order Report\n";
		cout<<"7. Logout\n";
		cout<<"\n==============================\n";
		cout<<"Enter your Choice (1-6): ";
		
		try{
			cin>>choice;
			if(cin.fail()){
				throw string("Invalid input detected! Please enter the valid number!");
			}
		if (choice < 1 || choice>6){
			throw choice;
		}
		switch (choice){
			case 1:
				cout<<"\n[SYSTEM] Loading Add Order Module...\n";
				addOrder();
				break;
			case 2:
				cout<<"\n[SYSTEM] Loadng Edit Order Module...\n";
				editOrder();
				break;
			case 3:
                cout << "\n[SYSTEM] Loading Delete Order Module...\n";
                deleteOrder(); 
                break;
			case 4:
				cout<<"\n[SYSTEM] Displaying All Records...\n";
				displayOrders();
				break;
			case 5:
				cout<<"\n[SYSTEM] Displaying All Records...\n";
				break;
			case 6:
				cout<<"\n[SYSTEM] Loading Search Module...\n";
				searchOrder();
				break;
			case 7:
				cout<<"\n[SYSTEM] Generating Booking's Report...\n";
				generateReport();
				break;
		}
	}

catch(string errorMsg){
	cout<<"\n[ERROR]"<< errorMsg <<endl;
	cin.clear();
	cin.ignore(10000, '\n');
	
}

catch(int invalidChoice){
	cout<<"\n[ERROR]"<<invalidChoice<<"is OUT of Range! Please Enter 1-7...\n";
}
		}
		
		
	}
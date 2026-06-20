#include<iostream>
#include<string>
#include <fstream>
#include <sstream>  // Required for string splitting (username|password|role)
#include"structures.h"
#include"customer.h"

using namespace std;

// External global variable defined in main.cpp -- the single linked list
// holding every logged-in-capable account (Admin, SuperAdmin, Customer).
extern User* userHead;

// =========================================================
// File I/O Engine -- Customer accounts only.
// Admin & SuperAdmin accounts are handled separately by
// loadAdminsFromFile() / saveAdminsToFile() in admin.cpp.
// =========================================================

void saveCustomersToFile() {
    // Format: username|password|role  (role will always be "Customer" here)
    ofstream outFile("data/Customer.txt", ios::out | ios::trunc);
    if (!outFile) {
        cout << "[System Warning] Cannot write to Customer.txt!" << endl;
        return;
    }
    User* temp = userHead;
    while (temp != NULL) {
        if (temp->role == "Customer") {
            outFile << temp->username << "|"
                    << temp->password << "|"
                    << temp->role << "\n";
        }
        temp = temp->next;
    }
    outFile.close();
}

void loadCustomersFromFile() {
    ifstream inFile("data/Customer.txt");
    if (!inFile) return;

    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;

        // Defensive programming: strip trailing '\r' left by Windows line endings
        if (!line.empty() && line[line.length() - 1] == '\r') {
            line.erase(line.length() - 1);
        }
        if (line.empty()) continue;

        stringstream ss(line);
        string un, pw, role;
        getline(ss, un,   '|');
        getline(ss, pw,   '|');
        getline(ss, role, '|');

        if (role != "Customer") continue; // defensive: skip anything unexpected

        User* n = new Customer(un, pw);

        if (userHead == NULL) {
            userHead = n;
        } else {
            User* temp = userHead;
            while (temp->next != NULL) temp = temp->next;
            temp->next = n;
        }
    }
    inFile.close();
}

Customer::Customer(string username , string password) : User(username, password, "Customer"){
	head = NULL;
	tail = NULL;
}

Customer::~Customer(){
	Order* current = head;
	
	while(current != NULL){
		Order* temp = current;
		current = current->next;
		delete temp;
	}
	
	head = NULL;
	tail = NULL;
}

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
	
	newOrder->operatorName = username;
	newOrder->next = NULL;
	
	if(head == NULL){
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
	
	if(head == NULL){
		cout<<"[INFO] You have no order. Go to order!\n";
		return;
	}
	
	Order* current = head;
	int recordcount = 1;
	
	while(current != NULL){
		cout<<"Record #"<< recordcount<<"\n";
		cout << "  Order ID : " << current->orderId << "\n";
        cout << "  Product ID : " << current->productId << "\n";
        cout << "  Category ID: " << current->categoryId << "\n";
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
	
	if(head == NULL){
		cout << "[INFO] System id empty. No orders to search.\n";
		return;
	}
	
	int targetId;
	cout<< "Enter the Order ID you want to search: ";
	cin>> targetId;
	
	sortOrders(1);
	
	int count = 0;
	Order* current = head;
	
	while(current !=NULL){
		count++;
		current = current->next;
	}
	
	Order** tempArray = new Order*[count];
	
	current = head;
	int i=0;
	
	while (current !=NULL){
		tempArray[i] = current;
		i++;
		current = current->next;
	}
	
	int first = 0;
	int last = count - 1;
	int mid;
	Order* found = NULL;
	
	while (first <= last){
		mid = (first + last) / 2;
		
		if(tempArray[mid]->orderId == targetId){
			found = tempArray[mid];
			break;
		}
		else if(tempArray[mid]->orderId > targetId){
			last = mid -1;
		}
		else{
			first = mid + 1;
		}
	}
	
	if(found != NULL){
		cout << "\n[SUCCESS] Record Found using Binary Search!\n";
        cout << "---------------------------------\n";
        cout << "Order ID    : " << found->orderId << "\n";
        cout << "Product ID  : " << found->productId << "\n";
        cout << "Category ID : " << found->categoryId << "\n";
        cout << "Quantity    : " << found->dispatchQuantity << "\n";
        cout << "Order Date  : " << found->orderDate << "\n";
        cout << "---------------------------------\n";
	}
	
	else{
		cout << "\n[ERROR] Order ID " << targetId << " not found in the system.\n";
	}
	
	delete[] tempArray;

}

void Customer::editOrder(){
	cout << "\n--- Edit Booking Details ---\n";
	if (head == NULL) {
   	cout<<"[INFO] System is empty. No bookings to edit.\n";
	return;
}

int targetId;
cout<< "Enter the Order ID you want to edit: ";
cin>>targetId;

Order* current = head;
bool found = false;

while(current != NULL){
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

void Customer::deleteOrder(){
	if (head == NULL){
		cout<<"No Orders to delete!\n";
		return;
	}
	
	int targetId;
	cout<<"Enter Order ID to delete: ";
	cin>>targetId;
	
	Order* current = head;
	Order* previous = NULL;
	
	while (current !=NULL && current->orderId !=targetId){
		previous = current;
		current = current->next;
	}
	
	if (current == NULL){
		cout << "Error Order ID"<<targetId<<"is not found.\n";
		return;
	}
	
	if (previous == NULL){
		head = head->next;
	} else {
		previous->next = current->next;
	}
	
	if (current == tail){
		tail = previous;
	}
	
	delete current;
	
	
	cout<<"The Order deleted Successfully!\n";
}

void Customer::generateReport(){
	cout<< "\n--- Generating Order Summary Report ---\n";
	
	if(head == NULL){
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
    while (current!= NULL){
    	outFile << "Order ID: " << current->orderId
                << " | Product ID: " << current->productId
                << " | Category Id: " << current->categoryId << "\n"
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
    if (head == NULL || head->next == NULL) {
        return;
    }

    bool swapped;
    Order* current;
    Order* lastPtr = NULL;

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
		cout<<"6. Sort Orders\n";
		cout<<"7. Generate Order Report\n";
		cout<<"8. Logout\n";
		cout<<"\n==============================\n";
		cout<<"Enter your Choice (1-8): ";
		
		try{
			cin>>choice;
			if(cin.fail()){
				throw string("Invalid input detected! Please enter the valid number!");
			}
		if (choice < 1 || choice>8){
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
				cout<<"\n[SYSTEM] Search The Records...\n";
				searchOrder();
				break;
			case 6:
				cout<<"\n[SYSTEM] Sorting the Orders...\n";{
					
				int sortChoice;
				cout<< "\nSort by:\n";
				cout<<"1. Order ID ascending\n";
				cout<<"2. Quantity descending\n";
				cout<<"Enter Choice: ";
				cin>>sortChoice;
				
				if(sortChoice == 1 || sortChoice == 2){
					sortOrders(sortChoice);
					displayOrders();
				}else{
					cout<<"[ERROR] Invalid sorting criteria.\n";
				}
				break;
			}
				
			case 7:
				cout<<"\n[SYSTEM] Generating Booking's Report...\n";
				generateReport();
				break;
				
			case 8:
				cout << "\n[SYSTEM] Logging out...\n";
				keepRunning = false;
				break;
		}
	}

catch(string errorMsg){
	cout<<"\n[ERROR]"<< errorMsg <<endl;
	cin.clear();
	cin.ignore(10000, '\n');
	
}

catch(int invalidChoice){
	cout<<"\n[ERROR]"<<invalidChoice<<"is OUT of Range! Please Enter 1-8...\n";
}
		}
		

	}
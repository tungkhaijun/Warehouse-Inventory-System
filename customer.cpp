#include<iostream>
#include<string>
#include <fstream>
#include <sstream>  // Required for string splitting (username|password|role)
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "structures.h"
#include "customer.h"

using namespace std;

const char* ORDER_SUMMARY_FILE = "data\\OrderSummary.txt";

string getCurrentDateTime() {
    time_t now = time(0);
    tm* localTime = localtime(&now);

    char buffer[30];
    strftime(buffer, 30, "%Y-%m-%d_%H:%M:%S", localTime);

    return string(buffer);
}

int getNextOrderId(Order* head) {
    int maxId = 1000;
    Order* current = head;

    while (current != NULL) {
        if (current->orderId > maxId) {
            maxId = current->orderId;
        }

        current = current->next;
    }

    return maxId + 1;
}

Product* findProductById(int productId) {
    ifstream inFile("data\\Inventory.txt");

    if (!inFile) {
        cout << "[ERROR] Cannot open data\\Inventory.txt.\n";
        return NULL;
    }

    string line;

    while (getline(inFile, line)) {
        if (line == "") {
            continue;
        }

        stringstream ss(line);
        string idStr, name, category, stockStr, zone, supplier, priceStr;

        getline(ss, idStr, '|');
        getline(ss, name, '|');
        getline(ss, category, '|');
        getline(ss, stockStr, '|');
        getline(ss, zone, '|');
        getline(ss, supplier, '|');
        getline(ss, priceStr, '|');

        int currentId = atoi(idStr.c_str());

        if (currentId == productId) {
            Product* foundProduct = new Product;

            foundProduct->productId = currentId;
            foundProduct->productName = name;
            foundProduct->category = category;
            foundProduct->stockQuantity = atoi(stockStr.c_str());
            foundProduct->zone = zone;
            foundProduct->supplier = supplier;
            foundProduct->productPrice = atof(priceStr.c_str());
            foundProduct->next = NULL;

            inFile.close();
            return foundProduct;
        }
    }

    inFile.close();
    return NULL;
}

void displayAvailableInventory() {
    ifstream inFile("data\\Inventory.txt");

    if (!inFile) {
        cout << "[ERROR] Cannot open data\\Inventory.txt.\n";
        return;
    }

    cout << "\n================ AVAILABLE INVENTORY ================\n";
    cout << "Product ID | Product Name | Category | Stock | Price\n";
    cout << "-----------------------------------------------------\n";

    string line;

    while (getline(inFile, line)) {
        if (line == "") {
            continue;
        }

        stringstream ss(line);
        string idStr, name, category, stockStr, zone, supplier, priceStr;

        getline(ss, idStr, '|');
        getline(ss, name, '|');
        getline(ss, category, '|');
        getline(ss, stockStr, '|');
        getline(ss, zone, '|');
        getline(ss, supplier, '|');
        getline(ss, priceStr, '|');

        cout << idStr << " | "
             << name << " | "
             << category << " | "
             << stockStr << " | RM "
             << priceStr << "\n";
    }

    cout << "=====================================================\n";

    inFile.close();
}

bool reduceInventoryStock(int productId, int orderQty) {
    ifstream inFile("data\\Inventory.txt");
    ofstream tempFile("data\\Inventory_temp.txt");

    if (!inFile || !tempFile) {
        cout << "[ERROR] Unable to open data\\Inventory.txt for stock update.\n";
        return false;
    }

    string line;
    bool productFound = false;
    bool stockEnough = false;

    while (getline(inFile, line)) {
        if (line == "") {
            continue;
        }

        stringstream ss(line);
        string idStr, name, category, stockStr, zone, supplier, priceStr;

        getline(ss, idStr, '|');
        getline(ss, name, '|');
        getline(ss, category, '|');
        getline(ss, stockStr, '|');
        getline(ss, zone, '|');
        getline(ss, supplier, '|');
        getline(ss, priceStr, '|');

        int currentId = atoi(idStr.c_str());
        int currentStock = atoi(stockStr.c_str());

        if (currentId == productId) {
            productFound = true;

            if (currentStock >= orderQty) {
                cout << "[SYSTEM] Stock before order: " << currentStock << "\n";
                currentStock = currentStock - orderQty;
                cout << "[SYSTEM] Stock after order : " << currentStock << "\n";
                stockEnough = true;
            } else {
                cout << "[ERROR] Not enough stock in Inventory.txt.\n";
            }
        }

        tempFile << idStr << "|"
                 << name << "|"
                 << category << "|"
                 << currentStock << "|"
                 << zone << "|"
                 << supplier << "|"
                 << priceStr << "\n";
    }

    inFile.close();
    tempFile.close();

    if (!productFound || !stockEnough) {
        remove("data\\Inventory_temp.txt");
        return false;
    }

    remove("data\\Inventory.txt");
    rename("data\\Inventory_temp.txt", "data\\Inventory.txt");

    return true;
}

void saveOrderSummaryFile(Order* head) {
    ofstream outFile("data\\OrderSummary.txt");

    if (!outFile) {
        cout << "[ERROR] Unable to create OrderSummary.txt!\n";
        return;
    }

    outFile << "=========================================\n";
    outFile << "        ORDER SUMMARY REPORT             \n";
    outFile << "=========================================\n\n";

    if (head == NULL) {
        outFile << "No order records available.\n";
        outFile.close();
        return;
    }

    Order* current = head;
    int count = 1;

    while (current != NULL) {
        Product* product = findProductById(current->productId);

        outFile << "Record #" << count << "\n";
        outFile << "Order ID   : " << current->orderId << "\n";
        outFile << "Product ID : " << current->productId << "\n";

        if (product != NULL) {
            outFile << "Product    : " << product->productName << "\n";
            outFile << "Category   : " << product->category << "\n";
            delete product;
        } else {
            outFile << "Product    : Unknown Product\n";
            outFile << "Category   : Unknown Category\n";
        }

        outFile << "Quantity   : " << current->dispatchQuantity << "\n";
        outFile << "Order Date : " << current->orderDate << "\n";
        outFile << "-----------------------------------------\n";

        current = current->next;
        count++;
    }

    outFile.close();
}
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

    cout << "\n-- Create New Order --\n";
    
    displayAvailableInventory();

    // System auto-generate Order ID and Date/Time
    newOrder->orderId = getNextOrderId(head);
    newOrder->orderDate = getCurrentDateTime();

    cout << "Generated Order ID: " << newOrder->orderId << "\n";
    cout << "Current Date/Time : " << newOrder->orderDate << "\n";

    cout << "Enter Product ID you want to order: ";
    cin >> newOrder->productId;
    
    Product* selectedProduct = findProductById(newOrder->productId);

    if (selectedProduct == NULL) {
        cout << "[ERROR] Product ID " << newOrder->productId << " not found. Order cancelled.\n";
        delete newOrder;
        return;
    }

    cout << "[SYSTEM] Product Found : " << selectedProduct->productName << "\n";
    cout << "[SYSTEM] Category      : " << selectedProduct->category << "\n";

    cout << "Enter Quantity: ";
    cin >> newOrder->dispatchQuantity;

    if (newOrder->dispatchQuantity <= 0) {
    cout << "[ERROR] Quantity must be more than 0. Order cancelled.\n";
    delete newOrder;
    delete selectedProduct;
    return;
	}

    newOrder->categoryId = 0;
    newOrder->operatorName = username;
    newOrder->next = NULL;

    cout << "\nPlease confirm your order:\n";
    cout << "---------------------------------\n";
    cout << "Order ID   : " << newOrder->orderId << "\n";
    cout << "Product ID : " << newOrder->productId << "\n";
    cout << "Quantity   : " << newOrder->dispatchQuantity << "\n";
    cout << "Date/Time  : " << newOrder->orderDate << "\n";
    cout << "---------------------------------\n";

    char confirm;
    cout << "Confirm this order? (Y/N): ";
    cin >> confirm;

    if (confirm != 'Y' && confirm != 'y') {
        cout << "[SYSTEM] Order cancelled by customer.\n";
        delete newOrder;
        delete selectedProduct;
        return;
    }
    
    if (!reduceInventoryStock(newOrder->productId, newOrder->dispatchQuantity)) {
    cout << "[ERROR] Stock update failed. Order cancelled.\n";
    delete newOrder;
    delete selectedProduct;
    return;
	}

    if (head == NULL) {
        head = newOrder;
        tail = newOrder;
    } else {
        tail->next = newOrder;
        tail = newOrder;
    }
    
    saveOrderSummaryFile(head);

    cout << "\n[SUCCESS] Order ID " << newOrder->orderId
         << " has been successfully added to the system!\n";
         
    cout << "[SUCCESS] Inventory stock has been updated.\n";
	cout << "[SUCCESS] OrderSummary.txt has been updated.\n";
	
	delete selectedProduct;
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
			saveOrderSummaryFile(head);
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
	saveOrderSummaryFile(head);
	
	cout<<"The Order deleted Successfully!\n";
}

void Customer::generateReport() {
    cout << "\n--- Generating Order Summary Report ---\n";

    if (head == NULL) {
        cout << "[INFO] System is empty. No orders to report.\n";
        return;
    }

    cout << "[SYSTEM] Sorting records by Quantity (Highest to Lowest)...\n";
    sortOrders(2);

    saveOrderSummaryFile(head);

    cout << "[SUCCESS] Report successfully saved to 'data\\OrderSummary.txt'!\n";
    cout << "\n[SYSTEM] Retrieving data from text file...\n\n";

    ifstream inFile("data\\OrderSummary.txt");

    if (!inFile) {
        cout << "[ERROR] Unable to read data\\OrderSummary.txt!\n";
        return;
    }

    string line;

    while (getline(inFile, line)) {
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
		cout<<"1. View Available Inventory\n";
		cout<<"2. Make a New Order\n";
		cout<<"3. Edit Order Details\n";
		cout<<"4. Delete the Orders\n";
		cout<<"5. View the Orders\n";
		cout<<"6. Search Orders\n";
		cout<<"7. Sort Orders\n";
		cout<<"8. Generate Order Report\n";
		cout<<"9. Logout\n";
		cout<<"\n==============================\n";
		cout<<"Enter your Choice (1-9): ";
		
		try{
			cin>>choice;
			if(cin.fail()){
				throw string("Invalid input detected! Please enter the valid number!");
			}
		if (choice < 1 || choice>9){
			throw choice;
		}
		switch (choice){
			case 1:
			    cout << "\n[SYSTEM] Loading Available Inventory...\n";
			    displayAvailableInventory();
		     	break;
			case 2:
				cout<<"\n[SYSTEM] Loading Add Order Module...\n";
				addOrder();
				break;
			case 3:
				cout<<"\n[SYSTEM] Loadng Edit Order Module...\n";
				editOrder();
				break;
			case 4:
                cout << "\n[SYSTEM] Loading Delete Order Module...\n";
                deleteOrder(); 
                break;
			case 5:
				cout<<"\n[SYSTEM] Displaying All Records...\n";
				displayOrders();
				break;
			case 6:
				cout<<"\n[SYSTEM] Search The Records...\n";
				searchOrder();
				break;
			case 7:
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
				
			case 8:
				cout<<"\n[SYSTEM] Generating Order's Report...\n";
				generateReport();
				break;
				
			case 9:
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
	cout<<"\n[ERROR]"<<invalidChoice<<"is OUT of Range! Please Enter 1-9...\n";
}
		}
		

	}
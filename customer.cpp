#include<iostream>
#include<string>
#include <fstream>
#include <sstream>
#include <ctime>
#include"structures.h"
#include"customer.h"

using namespace std;

extern Product* inventoryHead;

void forceReloadInventory();

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
        cout << "[ERROR] Cannot open Inventory.txt. Please put Inventory.txt in the same folder as main.exe.\n";
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

            foundProduct->productId = atoi(idStr.c_str());
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
    return nullptr;
}

void saveOrderSummaryFile(Order* head) {
    ofstream outFile("data\\OrderSummary.txt");

    if (!outFile) {
        cout << "[ERROR] Unable to create OrderSummary.txt!\n";
        return;
    }

    outFile << "=========================================\n";
    outFile << "        ORDER SUMMARY REPORT             \n";
    outFile << "=========================================\n";

    Order* current = head;

    while (current != NULL) {
        Product* product = findProductById(current->productId);

        outFile << "Order ID: " << current->orderId
                << " | Product ID: " << current->productId;

        if (product != NULL) {
            outFile << " | Product: " << product->productName
                    << " | Category: " << product->category;
        }

        outFile << " | Quantity: " << current->dispatchQuantity
                << " | Date: " << current->orderDate << "\n";

        current = current->next;
    }

    outFile.close();
}

Customer::Customer(string username , string password) : User(username, password, "Customer"){
	this->username = username;
	this->password = password;
	head = nullptr;
	tail = nullptr;
}

Customer::~Customer(){
	Order* current = head;
	
	while(current != nullptr){
		Order* temp = current;
		current = current->next;
		delete temp;
	}
	
	head = nullptr;
	tail = nullptr;
}

void Customer::addOrder(){
	forceReloadInventory();
	Order* newOrder = new Order;
	
	cout<<"\n-- Create New Order --\n";
	
	newOrder->orderId = getNextOrderId(head);
	newOrder->orderDate = getCurrentDateTime();
	
	cout << "Generated Order ID: "<< newOrder->orderId << "\n";
	cout << "Current Date/Time : " << newOrder->orderDate << "\n";
	
	cout<<"Enter Product ID you want to order?: ";
	cin >> newOrder->productId;
	
	Product* selectedProduct = findProductById(newOrder->productId);
	
	if(selectedProduct == nullptr){
		cout << "[ERROR] Product ID " << newOrder->productId << " not found. Order cancelled.\n";
		delete newOrder;
		return;
	}
	
	cout << "[SYSTEM] Product Found : " << selectedProduct->productName << "\n";
    cout << "[SYSTEM] Category      : " << selectedProduct->category << "\n";
	
	cout<<"Enter Quantity: ";
	cin >> newOrder->dispatchQuantity;
	
	if(newOrder->dispatchQuantity <= 0){
		cout << "[ERROR] Quantity must be more than 0. Order cancelled.\n";
		delete newOrder;
		return;
	}
	
	if (newOrder->dispatchQuantity > selectedProduct->stockQuantity){
        cout << "[ERROR] Not enough stock available. Order cancelled.\n";
        cout << "Available stock: " << selectedProduct->stockQuantity << "\n";
        delete newOrder;
        return;
	}
	
	newOrder->categoryId = 0;
	newOrder->operatorName = username;
	newOrder->next = nullptr;
	
	cout << "\nPlease confirm your order:\n";
    cout << "---------------------------------\n";
    cout << "Order ID   : " << newOrder->orderId << "\n";
    cout << "Product ID : " << newOrder->productId << "\n";
    cout << "Product    : " << selectedProduct->productName << "\n";
    cout << "Category   : " << selectedProduct->category << "\n";
    cout << "Quantity   : " << newOrder->dispatchQuantity << "\n";
    cout << "Date/Time  : " << newOrder->orderDate << "\n";
    cout << "---------------------------------\n";
    
    char confirm;
    cout << "Confirm this order? (Y/N): ";
    cin >> confirm;
    
    if (confirm != 'Y' && confirm != 'y'){
    	cout << "[SYSTEM] Order cancelled by customer.\n";
    	delete newOrder;
    	delete selectedProduct;
    	return;
	}
	
	selectedProduct->stockQuantity = selectedProduct->stockQuantity - newOrder->dispatchQuantity;
	
	if(head == nullptr){
		head = newOrder;
		tail = newOrder;
	} else{
		tail->next = newOrder;
		tail = newOrder;
	}
	
    saveOrderSummaryFile(head);
    
    cout << "\n[SUCCESS] Order ID " << newOrder->orderId
         << " has been successfully added to the system!\n";
    cout << "[SUCCESS] OrderSummary.txt have been updated.\n";
    
    delete selectedProduct;
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
	
	if(head == nullptr){
		cout << "[INFO] System id empty. No orders to search.\n";
		return;
	}
	
	int targetId;
	cout<< "Enter the Order ID you want to search: ";
	cin>> targetId;
	
	sortOrders(1);
	
	int count = 0;
	Order* current = head;
	
	while(current !=nullptr){
		count++;
		current = current->next;
	}
	
	Order** tempArray = new Order*[count];
	
	current = head;
	int i=0;
	
	while (current !=nullptr){
		tempArray[i] = current;
		i++;
		current = current->next;
	}
	
	int first = 0;
	int last = count - 1;
	int mid;
	Order* found = nullptr;
	
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
	
	if(found != nullptr){
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
	if (head == nullptr) {
   	cout<<"[INFO] System is empty. No bookings to edit.\n";
	return;
}

int targetId;
cout<< "Enter the Order ID you want to edit: ";
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

void Customer::deleteOrder(){
	if (head == nullptr){
		cout<<"No Orders to delete!\n";
		return;
	}
	
	int targetId;
	cout<<"Enter Order ID to delete: ";
	cin>>targetId;
	
	Order* current = head;
	Order* previous = nullptr;
	
	while (current !=nullptr && current->orderId !=targetId){
		previous = current;
		current = current->next;
	}
	
	if (current == nullptr){
		cout << "Error Order ID"<<targetId<<"is not found.\n";
		return;
	}
	
	if (previous == nullptr){
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
	
	if(head == nullptr){
		cout<< "[INFO] System is empty. No orders to report.\n";
		return;
	}
	
	cout << "[SYSTEM] Sorting records by Quantity (Highest to Lowest)...\n";
    sortOrders(2);
	
	saveOrderSummaryFile(head);
	
	cout << "[SUCCESS] Report successfully saved to 'OrderSummary.txt'!\n";
	cout << "\n[SYSTEM] Retrieving data from text file...\n\n";
    
    ifstream inFile("data\\OrderSummary.txt");
    
    if (!inFile){
    	cout << "[ERROR] Unable to read OrderSummary.txt!\n";
    	return;
	}
	
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
                
                int tempCatId = current->categoryId;
                current->categoryId = current->next->categoryId;
                current->next->categoryId = tempCatId;

                int tempQty = current->dispatchQuantity;
                current->dispatchQuantity = current->next->dispatchQuantity;
                current->next->dispatchQuantity = tempQty;

                string tempDate = current->orderDate;
                current->orderDate = current->next->orderDate;
                current->next->orderDate = tempDate;
                
                string tempOp = current->operatorName;
                current->operatorName = current->next->operatorName;
                current->next->operatorName = tempOp;

                swapped = true;
            }
            current = current->next;
        }
        lastPtr = current;
    } while (swapped);
    
    cout << "\n[SUCCESS] Records sorted successfully!\n";
}

void forceReloadInventory() {
    while (inventoryHead != nullptr) {
        Product* temp = inventoryHead;
        inventoryHead = inventoryHead->next;
        delete temp;
    }

    ifstream inFile("Inventory.txt");
    if (!inFile) return;

    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;

        Product* newNode = new Product;
        size_t pos = 0;
        string token;
        int field = 0;
        
        try {
            while ((pos = line.find('|')) != string::npos) {
                token = line.substr(0, pos);
                if (field == 0) newNode->productId = stoi(token);
                else if (field == 1) newNode->productName = token;
                else if (field == 2) newNode->category = token;
                else if (field == 3) newNode->stockQuantity = stoi(token);
                else if (field == 4) newNode->zone = token;
                else if (field == 5) newNode->supplier = token;
                
                line.erase(0, pos + 1);
                field++;
            }
            newNode->productPrice = stod(line);
            newNode->next = nullptr;
            
            if (inventoryHead == nullptr) {
                inventoryHead = newNode;
            } else {
                Product* temp = inventoryHead;
                while (temp->next != nullptr) temp = temp->next;
                temp->next = newNode;
            }
        } catch (...) {
            delete newNode;
        }
    }
    inFile.close();
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
	
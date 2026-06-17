// main.cpp
#include <iostream>
#include <fstream>
#include <string>
#include "structures.h"
#include "customer.h"
//#include "backend.h"
//#include "admin.h"

using namespace std;

Product* inventoryHead = NULL;
Order* orderHead = NULL;
User* userHead = NULL;

//1. Safe Input(Prevent Crash using Try-Catch)
int getSafeInput(){
	int input;
	while (true){
		try{
			cin >> input;
			if(cin.fail()){
				throw "System Error: Invalid input! Please enter numbers only.";
			}
			if (input < 0){
				throw "System Error: Negative numbers are not allowed! Please try again.";
			}
			return input; 
		}
		catch (const char* errorMessage){
			cout << errorMessage << endl;
			cin.clear();
			cin.ignore(1000, '\n');
			cout << ">>";
		}
	}
} 

//2. Load Inventory Data
void loadDataFromFile(){
	ifstream inFile("Inventory.txt");
	
	if (!inFile){
		cout << "System Warning: Inventory.txt not found. Starting with empty inventory." << endl;
		return;
	}
	
	int productId, stockQuantity;
	string productName, category, zone, supplier;
	double productPrice;
	
	while (inFile >> productId >> productName >> category >> stockQuantity >> zone >> supplier >> productPrice){
		
		Product* newNode = new Product;
		newNode->productId = productId;
		newNode->productName = productName;
		newNode->category = category;
		newNode->stockQuantity = stockQuantity;
		newNode->zone = zone;
		newNode->supplier = supplier;
		newNode->productPrice = productPrice;
		newNode->next = NULL;
		
		if (inventoryHead == NULL){
			inventoryHead = newNode;
		}else{
			Product* temp = inventoryHead;
			while (temp->next != NULL){
				temp = temp->next;
			}
			temp->next = newNode;
		}
	}
	inFile.close();
	cout<<"System: Inventory data loaded succesfully."<<endl;
}

//3. Load Order Data
void loadOrdersFromFile(){
	ifstream inFile("Orders.txt");
	
	if(!inFile){
		cout <<"System Warning: Orders.txt not found. Starting with empty order records." << endl;
		return;
	}
	
	int orderId, productId, dispatchQuantity;
	string operatorName, orderDate;
	
	while (inFile >> orderId >> productId >> dispatchQuantity >> operatorName >> orderDate){
	   Order* newNode = new Order;
	   newNode->orderId = orderId;
	   newNode->productId = productId;
	   newNode->dispatchQuantity = dispatchQuantity;
	   newNode->operatorName = operatorName;
	   newNode->orderDate = orderDate;
	   newNode->next = NULL;
	   
	   if(orderHead == NULL){
	   	orderHead = newNode;
	   } else{
	   	Order* temp = orderHead;
	   	while (temp->next !=NULL){
	   		temp = temp->next;
		   }
		   temp->next = newNode;
	   }
   }

  inFile.close();
  cout << "System: Order data loaded successfully." << endl;
}


//4. Load Admin Data
void loadUsersFromFile(){
	ifstream inFile("Admin.txt");
	
	if(!inFile){
		cout <<"System Error: Admin.txt not found! Login will fail." << endl;
		return;
	}
	
	string user, pass, role;
	
	// Use getline to parse data separated by the pipe '|' character
    while (getline(inFile, user, '|')) {
        getline(inFile, pass, '|');
        getline(inFile, role); 

        // Defensive programming: remove trailing '\r' if running on Windows
        if (!role.empty() && role[role.length() - 1] == '\r') {
            role.erase(role.length() - 1);
        }

        User* newNode = new User;
        newNode->username = user;
        newNode->password = pass;
        newNode->role = role;
        newNode->next = NULL;

        if (userHead == NULL) {
            userHead = newNode;
        } else {
            User* temp = userHead;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
     }
     inFile.close();
	 cout <<"System: User credentials loaded succesfully." <<endl;	
}


//5. Save Inventory Data
void saveDataToFile(){
	ofstream outFile("Inventory.txt", ios::trunc);
	
	if(!outFile){
		cout<<"System Error: Could not open Inventory.txt to save data."<<endl;
		return;
	}
	
	Product* temp = inventoryHead;
	
	while(temp != NULL){
		outFile << temp->productId << " "
		        << temp->productName<< " "
		        << temp->category<< " "
		        << temp->stockQuantity<< " "
		        << temp->zone<< " "
		        << temp->supplier <<" "
		        << temp->productPrice <<"\n";
	
	temp = temp->next;
	}
	
	outFile.close();
	cout << "System: All inventory data succesfully saved to file." << endl;
}


//6. Save Order Data
void saveOrdersToFile(){
	ofstream outFile("Orders.txt", ios::trunc);
	
	if(!outFile){
		cout << "System Error: Could not open Orders.txt to save data." << endl;
		return;
	}
	
	Order* temp = orderHead;
	
	while (temp != NULL){
		outFile << temp->orderId << " "
                << temp->productId << " "
				<< temp->dispatchQuantity << " "
				<< temp->operatorName << " "
				<< temp->orderDate <<"\n";
				
	  temp = temp->next;		
	}
	
	outFile.close();
	cout << "System: Order data successfully saved to file." << endl;
}

//7. Check Required for User
void checkSupportFile(const char* fileName){
	ifstream inFile(fileName);
	
	if (!inFile){
		ofstream createFile(fileName);
		createFile.close();
		cout <<"System Warning: " << fileName << " not found. Empty file created. " << endl;
	  }else{
	  	inFile.close();
	  	cout <<"System: " << fileName << " is ready."<< endl;
	  }
	}
	
	void checkRequiredFiles(){
		checkSupportFile("Suppliers.txt");
		checkSupportFile("Logistics.txt");
	}
	
//8.Check whether username exist
bool isUsernameExist(string username){
	User* temp = userHead;
	
	while (temp != NULL){
		if(temp->username == username){
			return true;
		}
		temp = temp->next;
	}
	return false;
}

//9. Add usernode to linked list
void addUserToList(string username, string password, string role){
	User* newNode = new User;
	newNode->username= username;
	newNode->password= password;
	newNode->role = role;
	newNode->next = NULL;
	
	if(userHead == NULL){
		userHead = newNode;
	}
	else{
		User* temp = userHead;
		
		while (temp->next != NULL){
			temp = temp->next;
		}
		temp->next = newNode;
	}
}

//10.Save user to Admin.txt
void saveUsersToFile(){
	ofstream outFile("Admin.txt", ios::trunc);
	
	if(!outFile){
		cout << "System Error: Could not open Admin.txt to save user data." << endl;
		return;
	}
	
	User* temp = userHead;
	
	while (temp != NULL){
		outFile << temp->username << "|"
		        << temp->password << "|"
		        << temp->role <<"\n";
		        
		temp = temp->next;
	}
	
	outFile.close();
	cout <<"System: User data succesfully saved to file." << endl;
}

//11. Customer registration function
void registerCustomer(){
	string username, password, confirmPassword;
	
	cout <<"\n~+~Customer Registration~+~" <<endl;
	
	cout <<"Enter username: ";
	cin >> username;
	
	if (isUsernameExist(username)){
		cout <<"System Error: Username already exists. Please try another username." << endl;
		return;
	}
	
	cout << "Enter password: ";
	cin >> password;
	
	cout << "Confirm password: ";
	cin >> confirmPassword;
	
	if (password != confirmPassword){
		cout << "System Error: Password confirmation does not match." <<endl;
		return;
	}
	
	addUserToList(username, password, "Customer");
	saveUsersToFile();
	
	cout <<"System: Customer account registered successfully." << endl;
}

//12. Authentication for User
bool authenticateUser(string inputUser, string inputPass, string expectedRole) {
    User* temp = userHead;
    
    while (temp != NULL) {
        if (temp->username == inputUser && temp->password == inputPass && temp->role == expectedRole) {
            return true;
        }
        temp = temp->next; 
    }
    return false; 
}


//13. Clear Memory to prevent Memory Leak
void clearMemory(){
	Product* productTemp;
	
	while (inventoryHead != NULL){
		productTemp = inventoryHead;
		inventoryHead = inventoryHead ->next;
		delete productTemp;
	}
	
	Order* orderTemp;
	
	while (orderHead != NULL){
		orderTemp = orderHead;
		orderHead = orderHead->next;
		delete orderTemp;
	}
	
	User* userTemp;
	
	while(userHead != NULL){
		userTemp = userHead;
		userHead = userHead->next;
		delete userTemp;
	}
	
	cout <<"System: Memory cleared safely." << endl;
}


//14. Main Menu
int main() {
    cout <<"==========================================="<<endl;
    cout <<"        Warehouse Inventory System         "<<endl;
    cout <<"==========================================="<<endl;
    
    loadDataFromFile();
    loadUsersFromFile();
    loadOrdersFromFile();
    checkRequiredFiles();
    
    bool isRunning = true;
    string inputUser, inputPass;
    while (isRunning){
    	cout <<"\n~+~ Main Gateway ~+~"<<endl;
    	cout <<"\n>>> Login Menu <<<" <<endl;
    	cout << "1. Login as Admin " <<endl;
    	cout << "2. Login as Customer "<<endl;
    	cout << "3. Register as Customer" <<endl;
        cout << "4. Exit System" <<endl;
        cout << "Select your role: ";
        
        int choice = getSafeInput();
        
        switch (choice) {
            case 1: {
                cout << "\n--- Admin Login ---" << endl;
                cout << "Username: ";
                cin >> inputUser;
                cout << "Password: ";
                cin >> inputPass;

                // Verify credentials against the User linked list
                if (authenticateUser(inputUser, inputPass, "Admin")) {
                    cout << "\nAccess Granted. Welcome, Admin " << inputUser << "!" << endl;
                    
                    // TODO: Call Member 2's admin module here
                    // Admin adminObj;
                    // adminObj.displayMenu(); 
                } else {
                    cout << "\nAccess Denied. Incorrect username or password." << endl;
                }
                break;
            }
            case 2: {
                cout << "\n--- Customer Login ---" << endl;
                cout << "Username: ";
                cin >> inputUser;
                cout << "Password: ";
                cin >> inputPass;

                if (authenticateUser(inputUser, inputPass, "Customer")) {
                    cout << "\nAccess Granted. Welcome, Customer " << inputUser << "!" << endl;
                    
                    Customer customerObj(inputUser, inputPass);
                    customerObj.displayMenu();
                } else {
                    cout << "\nAccess Denied. Incorrect username or password." << endl;
                }
                break;
            }
            case 3:{
            	registerCustomer();
				break;
			}
            case 4: {
                cout << "\nSystem: Preparing to shutdown..." << endl;
                isRunning = false;
                break;
            }
            default:
                cout << "System Error: Invalid option. Please select 1-3." << endl;
        }
    }

    // Save only inventory data upon exit (User data does not change)
    saveDataToFile();
    saveOrdersToFile();
    saveUsersToFile();
    clearMemory();
    
    cout << "System Terminated." << endl;
    return 0;
}

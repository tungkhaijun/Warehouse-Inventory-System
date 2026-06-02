// main.cpp
#include <iostream>
#include <fstream>
#include <string>
#include "structures.h"
#include "customer.h"
//#include "admin.h"
//#include "manager.h"

using namespace std;

Product* inventoryHead = NULL;
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
	
	string id, name, category, location, supplier;
	int qty;
	double price;
	
	while (inFile >> id >> name >> category >> qty >> location >> supplier >> price){
		
		Product* newNode = new Product;
		newNode->productId = id;
		newNode->productName = name;
		newNode->category = category;
		newNode->quantity = qty;
		newNode->location = location;
		newNode->supplier = supplier;
		newNode->price = price;
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

//3. Load Admin Data
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

//4. Save Inventory Data
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
		        << temp->quantity<< " "
		        << temp->location<< " "
		        << temp->supplier <<" "
		        << temp->price <<"\n";
	
	temp = temp->next;
	}
	
	outFile.close();
	cout << "System: All inventory data succesfully saved to file." << endl;
}

//5. Authentication for User
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

//6. Main Menu
int main() {
    cout <<"==========================================="<<endl;
    cout <<"  Warehouse Inventory System Initializing  "<<endl;
    cout <<"==========================================="<<endl;
    
    loadDataFromFile();
    loadUsersFromFile();
    
    bool isRunning = true;
    string inputUser, inputPass;
    while (isRunning){
    	cout <<"\n~+~ Main Gateway ~+~"<<endl;
    	cout <<"\n>>> Login Menu <<<" <<endl;
    	cout << "1. Login as Admin " << endl;
        cout << "2. Login as Manager " << endl;
        cout << "3. Exit System" << endl;
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
                cout << "\n--- Manager Login ---" << endl;
                cout << "Username: ";
                cin >> inputUser;
                cout << "Password: ";
                cin >> inputPass;

                if (authenticateUser(inputUser, inputPass, "Manager")) {
                    cout << "\nAccess Granted. Welcome, Manager " << inputUser << "!" << endl;
                    
                    // TODO: Call Member 3's manager module here
                    // Manager managerObj;
                    // managerObj.displayMenu();
                } else {
                    cout << "\nAccess Denied. Incorrect username or password." << endl;
                }
                break;
            }
            case 3: {
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
    
    cout << "System Terminated." << endl;
    return 0;
}

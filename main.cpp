// main.cpp
#include <iostream>
#include <fstream>
#include <string>
#include "structures.h"
#include "customer.h"
#include "admin.h"
#include "backend.h"


using namespace std;


User* userHead = NULL;
ProductLinkedList globalInventory;

//1. Safe Input to ensure user enter number only for menu choice
int getSafeInput(){
	int input;
	
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
			
			return -1;
		
	}
} 

	
//2.Check whether username exist in the file
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

//3. Register validation for username and password
bool isValidUsername(string username){
	if (username.length() < 3 || username.length() > 20){
		return false;
	}
	
	for (int i = 0; i < username.length(); i++){
		char c = username[i];
		
		bool isLetter = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
		bool isDigit = (c >= '0' && c <= '9');
		bool isUnderscore = (c == '_');
		
		if (!isLetter && !isDigit && !isUnderscore){
		return false;
	}
   }
  	
  return true;
}

bool isValidPassword(string password){
	if (password.length() < 4 || password.length() > 20){
		return false;
	}
	
	for (int i = 0; i < password.length(); i++){
		if (password[i] == '|'){
			return false;
		}
	}
	
	return true;
}


//4. Add usernode to linked list in-memory only; caller decides when to persist to file. Used for both Admin/SuperAdmin and Customer Registration.
void addUserToList(string username, string password, string role){
    User* newNode = NULL;

    if (role == "Admin") {
        newNode = new Admin(username, password);
    }
    else if (role == "SuperAdmin") {
        newNode = new SuperAdmin(username, password);
    }
    else if (role == "Customer") {
        newNode = new Customer(username, password);
    }
    else {
        cout << "System Error: Invalid role. User not added." << endl;
        return;
    }

    newNode->next = NULL;

    if (userHead == NULL) {
        userHead = newNode;
    }
    else {
        User* temp = userHead;

        while (temp->next != NULL) {
            temp = temp->next;
        }

        temp->next = newNode;
    }
}

//5. Customer registration function
void registerCustomer(){
	string username, password, confirmPassword;
	bool validUsername = false;
	bool validPassword = false;
	
	cout <<"\n~+~Customer Registration~+~" <<endl;
	cout <<"Enter 0 anytime for cancel registration."<<endl;
 //Username Validation looping
	while (!validUsername){		
       cout <<"Enter username: ";
	   cin >> username;
	   
	if (username == "0"){
		cout << "System: Customer registration cancelled." << endl;
		return;
	}
		
	if (!isValidUsername(username)){
		cout <<"System Error: Username must be 3~20 characters and only contain letters, numbers or underscore." << endl;
		cout <<"Please enter username again.\n" << endl;
	}
	
	else if (isUsernameExist(username)){
		cout <<"System Error: Username already exists." << endl;
		cout <<"Please try another username again.\n" << endl;
	}
	
	else{
      validUsername = true;
	}
 }

	// Password validation looping to ensure both validity and matches confirmation
	while (!validPassword){
	 cout << "Enter password: ";
	 cin >> password;
	 
	if (password == "0"){
		cout << "System: Customer registration cancelled." << endl;
		return;
	}
	 
	 if (!isValidPassword(password)){
		cout << "System Error: Password must be 4~20 characters and cannot contain | symbol." << endl;
		cout << "Please enter password again.\n" << endl;
		continue;
	}
	
	cout << "Confirm password: ";
	cin >> confirmPassword;
	
	if (password != confirmPassword){
		cout << "System Error: Password confirmation does not match." <<endl;
		cout << "Please enter password again.\n" << endl;
	}
	else{
		validPassword = true;
	}
		
  }
  
	addUserToList(username, password, "Customer");
	saveCustomersToFile();
	
	cout <<"System: Customer account registered successfully." << endl;
}

//6. Authentication for User
User* authenticateUser(string inputUser, string inputPass, string expectedRole) {
    User* temp = userHead;
    
    while (temp != NULL) {
        if (temp->username == inputUser &&
		    temp->password == inputPass &&
			temp->role == expectedRole) {
			return temp;
        }
        temp = temp->next; 
    }
    return NULL; 
}

//7. Clear Memory to prevent Memory Leak
void clearMemory(){
	User* userTemp;
	
	while(userHead != NULL){
		userTemp = userHead;
		userHead = userHead->next;
		delete userTemp;
	}
	
	cout <<"System: Memory cleared safely." << endl;
}


//8. Main Menu
int main() {
    cout <<"==========================================="<<endl;
    cout <<"        Warehouse Inventory System         "<<endl;
    cout <<"==========================================="<<endl;
    
    // Load everything from data/ — single source of truth for both
    // inventory (globalInventory) and accounts (userHead).
    // Admin/SuperAdmin accounts and Customer accounts are loaded from
    // separate files but end up on the same userHead linked list.
    loadInventoryFromFile(globalInventory);
    loadAdminsFromFile();
    loadCustomersFromFile();
    
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
        
        if(choice == -1){
        	continue;
		}
        
        switch (choice) {
            case 1: {
                cout << "\n--- Admin Login ---" << endl;
                cout << "Username: ";
                cin >> inputUser;
                cout << "Password: ";
                cin >> inputPass;

                // Verify credentials against the User linked list (Admin /SuperAdmin)
                User* currentUser = authenticateUser(inputUser, inputPass, "Admin");
                
                if (currentUser != NULL) {
                    cout << "\nAccess Granted. Welcome, Admin " << inputUser << "!" << endl;
                    currentUser->displayMenu(); 
                }
                else{
                    currentUser = authenticateUser(inputUser, inputPass, "SuperAdmin");

                    if(currentUser != NULL) {
                        cout << "\nAccess Granted. Welcome, SuperAdmin " << inputUser << "!" << endl;
                        currentUser->displayMenu();
                    }
                    else {
                        cout << "\nAccess Denied. Incorrect username or password." << endl;
                    }
                }

                break;
            }

            case 2: {
                cout << "\n--- Customer Login ---" << endl;
                cout << "Username: ";
                cin >> inputUser;
                cout << "Password: ";
                cin >> inputPass;
                
                User* currentUser = authenticateUser(inputUser, inputPass, "Customer");
				
				if(currentUser != NULL){
                    cout << "\nAccess Granted. Welcome, Customer " << inputUser << "!" << endl;
                    currentUser->displayMenu();
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
                cout << "System Error: Invalid option. Please select 1-4." << endl;
        }
    }

    // Save all updated data before exit — same data/ files used at startup.
    saveInventoryToFile(globalInventory);
    saveAdminsToFile();
    saveCustomersToFile();
    clearMemory();
    
    cout << "System Terminated." << endl;
    return 0;
}
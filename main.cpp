// main.cpp
#include <iostream>
#include <fstream>
#include <string>
#include "structures.h"
#include "customer.h"
#include "backend.h"
#include "admin.h"

using namespace std;

Product* inventoryHead = NULL;
Order* orderHead = NULL;
User* userHead = NULL;
ProductLinkedList globalInventory;

//1. Safe Input: Make sure user enters number for menu choice
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


//4. Load User Data
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

        // Defensive programming: remove extra windows new line characters
        if (!role.empty() && role[role.length() - 1] == '\r') {
            role.erase(role.length() - 1);
        }

        User* newNode = NULL;

        if (role == "Admin") {
        newNode = new Admin(user, pass);
        }
        else if (role == "SuperAdmin") {
        newNode = new SuperAdmin(user, pass);
        }
        else if (role == "Customer") {
        newNode = new Customer(user, pass);
        }
        else {
        cout << "System Warning: Unknown role skipped for user " << user << endl;
        continue;
       }
		
		newNode->next = NULL;
		
        if(userHead == NULL){
        	userHead = newNode;
		}
		else{
			User* temp = userHead;
			
			while(temp->next != NULL){
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

//9. Register Validation
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


//10. Add usernode to linked list
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

//11.Save user to Admin.txt
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

//12. Customer registration function
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

	// Password validation looping
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
	saveUsersToFile();
	
	cout <<"System: Customer account registered successfully." << endl;
}
//13. Authentication for User
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

//14. Clear Memory to prevent Memory Leak
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


//15. Main Menu
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

    // Save all updated data before exit
    saveDataToFile();
    saveOrdersToFile();
    saveUsersToFile();
    clearMemory();
    
    cout << "System Terminated." << endl;
    return 0;
}
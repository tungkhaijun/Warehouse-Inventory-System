#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <limits>
#include <algorithm>
#include <cctype>

using namespace std;

// Helper functions
// remove unwanted characters from input
string cleanString(string str) {
	    while (!str.empty() && (str.back() == '\r' || str.back() == '\n' || 
	                            str.back() == ' ' || isspace(str.back()) || 
	                            (unsigned char)str.back() == 12289 || str.back() == '?')) {
	        str.pop_back();
	    }
	    return str;
	}

// PART 1: CORE STRUCT DEFINITIONS (structures.h)
struct Product {
	    int productId;
	    string productName;
	    string category;
	    int stockQuantity;
	    string zone;
	    string supplier;
	    double productPrice;
	    
	    Product* next;
};

struct Order {
	    int orderId;
	    int productId;
	    int dispatchQuantity;
	    string operatorName;
	    string orderDate;
	    
	    Order* next;
};

struct User {
	    string username;
	    string password; 
	    string role; // User,Admin,Manager,Customer
	    
	    User* next; 
};

// PART 2: CLASS DECLARATIONS & OOP INHERITANCE (backend.h)
class UserClass {
	protected:
	    string username;
	    string password; 
	    string role; 
	public:
	    UserClass(string un, string pw, string r) : username(un), password(pw), role(r) {}
	    virtual ~UserClass() {} 
	    virtual void displayMenu() = 0; 
	    
	    string getUsername() { return username; }
	    string getPassword() { return password; }
	    string getRole() { return role; }
};

class ProductLinkedList {
	private:
	    Product* head;
	    int count;
	public:
	    ProductLinkedList();
	    ~ProductLinkedList();
	    void Merge(Product** arr, int lpos, int rpos, int rend, int sortBy);
	    void MergeSortRec(Product** arr, int left, int right, int sortBy);
	    void insertNode(int id, string name, string cat, int qty, string z, string sup, double price);
	    void deleteNode(int id);
	    void sortList(int sortBy);
	    Product* binarySearch(int targetId);
	    void display();
	    Product* getHead() { return head; }
	    void clearAll(); 
};

// Global variables
	ProductLinkedList inventory; 
	Order* orderHead = nullptr;  
	User* userHead = nullptr;    

// File handling
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

void loadDataFromFile(){
    ifstream inFile("Inventory.txt");
    if (!inFile){
        cout << "System Warning: Inventory.txt not found. Starting with empty inventory." << endl;
        return;
    }
    
    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string idStr, name, cat, qtyStr, z, sup, priceStr;
        
        if (getline(ss, idStr, '|') && getline(ss, name, '|') && getline(ss, cat, '|') &&
            getline(ss, qtyStr, '|') && getline(ss, z, '|') && getline(ss, sup, '|') && getline(ss, priceStr)) {
            
            inventory.insertNode(stoi(idStr), cleanString(name), cleanString(cat), stoi(qtyStr), cleanString(z), cleanString(sup), stod(priceStr));
        }
    }
    inFile.close();
    cout << "System: Inventory data loaded successfully via '|' delimiter." << endl;
}

void loadOrdersFromFile(){
    ifstream inFile("Orders.txt");
    if(!inFile){
        cout << "System Warning: Orders.txt not found. Starting with empty order records." << endl;
        return;
    }
    
    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string oId, pId, qty, opName, oDate;
        
        if (getline(ss, oId, '|') && getline(ss, pId, '|') && getline(ss, qty, '|') && 
            getline(ss, opName, '|') && getline(ss, oDate)) {
            Order* newNode = new Order;
            newNode->orderId = stoi(oId);
            newNode->productId = stoi(pId);
            newNode->dispatchQuantity = stoi(qty);
            newNode->operatorName = cleanString(opName);
            newNode->orderDate = cleanString(oDate); // ?????????? \r ?????
            newNode->next = NULL;
            
            if(orderHead == NULL){
                orderHead = newNode;
            } else{
                Order* temp = orderHead;
                while (temp->next != NULL){ temp = temp->next; }
                temp->next = newNode;
            }
        }
    }
    inFile.close();
    cout << "System: Order data loaded successfully." << endl;
}

void loadUsersFromFile(){
    ifstream inFile("Admin.txt");
    if(!inFile){
        cout << "System Error: Admin.txt not found! Login will fail." << endl;
        return;
    }
    
    string user, pass, role;
    while (getline(inFile, user, '|')) {
        getline(inFile, pass, '|');
        getline(inFile, role); 

        user = cleanString(user);
        pass = cleanString(pass);
        role = cleanString(role);

        User* newNode = new User;
        newNode->username = user;
        newNode->password = pass;
        newNode->role = role;
        newNode->next = NULL;

        if (userHead == NULL) {
            userHead = newNode;
        } else {
            User* temp = userHead;
            while (temp->next != NULL) { temp = temp->next; }
            temp->next = newNode;
        }
     }
     inFile.close();
     cout << "System: User credentials loaded successfully." << endl;  
}

void saveDataToFile(){
    ofstream outFile("Inventory.txt", ios::trunc);
    if(!outFile){
        cout<<"System Error: Could not open Inventory.txt to save data."<<endl;
        return;
    }
    
    Product* temp = inventory.getHead(); 
    while(temp != NULL){
        outFile << temp->productId << "|" << temp->productName << "|" << temp->category << "|"
                << temp->stockQuantity << "|" << temp->zone << "|" << temp->supplier << "|"
                << temp->productPrice << "\n";
        temp = temp->next;
    }
    outFile.close();
    cout << "System: All inventory data successfully saved to file." << endl;
}

void saveOrdersToFile(){
    ofstream outFile("Orders.txt", ios::trunc);
    if(!outFile){
        cout << "System Error: Could not open Orders.txt to save data." << endl;
        return;
    }
    
    Order* temp = orderHead;
    while (temp != NULL){
        outFile << temp->orderId << "|" << temp->productId << "|" << temp->dispatchQuantity << "|"
                << temp->operatorName << "|" << temp->orderDate << "\n";
        temp = temp->next;        
    }
    outFile.close();
    cout << "System: Order data successfully saved to file." << endl;
}

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
    
bool isUsernameExist(string username){
    User* temp = userHead;
    while (temp != NULL){
        if(temp->username == username){ return true; }
        temp = temp->next;
    }
    return false;
}

void addUserToList(string username, string password, string role){
    User* newNode = new User;
    newNode->username= username;
    newNode->password= password;
    newNode->role = role;
    newNode->next = NULL;
    
    if(userHead == NULL){
        userHead = newNode;
    } else{
        User* temp = userHead;
        while (temp->next != NULL){ temp = temp->next; }
        temp->next = newNode;
    }
}

void saveUsersToFile(){
    ofstream outFile("Admin.txt", ios::trunc);
    if(!outFile){
        cout << "System Error: Could not open Admin.txt to save user data." << endl;
        return;
    }
    User* temp = userHead;
    while (temp != NULL){
        outFile << temp->username << "|" << temp->password << "|" << temp->role <<"\n";
        temp = temp->next;
    }
    outFile.close();
    cout <<"System: User data successfully saved to file." << endl;
}

void registerCustomer(){
    string username, password, confirmPassword;
    cout <<"\n~+~Customer Registration~+~" <<endl;
    cout <<"Enter username: "; cin >> username;
    
    if (isUsernameExist(username)){
        cout <<"System Error: Username already exists. Please try another username." << endl;
        return;
    }
    cout << "Enter password: "; cin >> password;
    cout << "Confirm password: "; cin >> confirmPassword;
    
    if (password != confirmPassword){
        cout << "System Error: Password confirmation does not match." <<endl;
        return;
    }
    
    addUserToList(username, password, "Customer");
    saveUsersToFile();
    cout <<"System: Customer account registered successfully." << endl;
}

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

// Linked list and sorting functions
ProductLinkedList::ProductLinkedList() { head = NULL; count = 0; }
ProductLinkedList::~ProductLinkedList() { clearAll(); }
	void ProductLinkedList::clearAll() {
	    Product* curr = head;
	    while (curr != NULL) {
	        Product* nextNode = curr->next;
	        delete curr;
	        curr = nextNode;
	    }
	    head = NULL; count = 0;
}

void ProductLinkedList::Merge(Product** arr, int lpos, int rpos, int rend, int sortBy) {
    int lend = rpos - 1; int tmppos = 0; int numelements = rend - lpos + 1;
    Product** TmpArray = new Product*[numelements];
    int l_cursor = lpos; int r_cursor = rpos;

    while (l_cursor <= lend && r_cursor <= rend) {
        bool condition = false;
        if (sortBy == 1) condition = arr[l_cursor]->stockQuantity <= arr[r_cursor]->stockQuantity;
        else if (sortBy == 2) condition = arr[l_cursor]->productPrice <= arr[r_cursor]->productPrice;
        else condition = arr[l_cursor]->productId <= arr[r_cursor]->productId;

        if (condition) TmpArray[tmppos++] = arr[l_cursor++];
        else TmpArray[tmppos++] = arr[r_cursor++];
    }
    while (l_cursor <= lend) TmpArray[tmppos++] = arr[l_cursor++];
    while (r_cursor <= rend) TmpArray[tmppos++] = arr[r_cursor++];
    for (int i = 0; i < numelements; i++) arr[rend - i] = TmpArray[numelements - 1 - i];
    delete[] TmpArray;
}

void ProductLinkedList::MergeSortRec(Product** arr, int left, int right, int sortBy) {
    if (left < right) {
        int center = (left + right) / 2;
        MergeSortRec(arr, left, center, sortBy);
        MergeSortRec(arr, center + 1, right, sortBy);
        Merge(arr, left, center + 1, right, sortBy);
    }
}

void ProductLinkedList::insertNode(int id, string name, string cat, int qty, string z, string sup, double price) {
    Product* temp = new Product;
    temp->productId = id; temp->productName = name; temp->category = cat;
    temp->stockQuantity = qty; temp->zone = z; temp->supplier = sup; temp->productPrice = price;
    temp->next = NULL;

    if (head == NULL) head = temp;
    else {
        Product* curr = head;
        while (curr->next != NULL) curr = curr->next;
        curr->next = temp;
    }
    count++;
}

void ProductLinkedList::deleteNode(int id) {
    if (head == NULL) return;
    Product* temp = head; Product* prev = NULL;
    if (head->productId == id) { head = head->next; delete temp; count--; }
    else {
        while (temp != NULL && temp->productId != id) { prev = temp; temp = temp->next; }
        if (temp) { prev->next = temp->next; delete temp; count--; }
    }
}

void ProductLinkedList::sortList(int sortBy) {
    if (count <= 1) return;
    Product** TmpArray = new Product*[count];
    Product* curr = head; int i = 0;
    while (curr != NULL) { TmpArray[i++] = curr; curr = curr->next; }
    MergeSortRec(TmpArray, 0, count - 1, sortBy);
    head = TmpArray[0]; curr = head; 
    for (i = 1; i < count; i++) { curr->next = TmpArray[i]; curr = curr->next; }
    curr->next = NULL;
    delete[] TmpArray;
}

Product* ProductLinkedList::binarySearch(int targetId) {
    if (count == 0) return NULL;
    sortList(0);
    Product** TmpArray = new Product*[count];
    Product* curr = head; int i = 0;
    while (curr != NULL) { TmpArray[i++] = curr; curr = curr->next; }
    int first = 0; int last = count - 1; int mid; Product* found = NULL;
    while (first <= last) {
        mid = (first + last) / 2;
        if (TmpArray[mid]->productId == targetId) { found = TmpArray[mid]; break; }
        else if (TmpArray[mid]->productId > targetId) last = mid - 1;
        else first = mid + 1;
    }
    delete[] TmpArray;
    return found;
}

void ProductLinkedList::display() {
    Product* temp = head;
    while (temp != NULL) {
        cout << "[ID: " << temp->productId << " | Name: " << temp->productName 
             << " | Qty: " << temp->stockQuantity << " | Price: RM" << temp->productPrice << "]\n";
        temp = temp->next;
    }
}

// Menu functions
class Admin : public UserClass {
	public:
	    Admin(string un) : UserClass(un, "", "Admin") {}
	    
	    void displayMenu() override {
	        cout << "\n--- Admin Dashboard ---" << endl;
	        cout << "[INFO] Admin menu functions are currently under construction by Member 2." << endl;
	        cout << "1. Temp View Current Inventory\n2. Logout\nSelect: ";
	        int c = getSafeInput();
	        if (c == 1) {
	            inventory.display();
	        }
	    }
};

class Customer : public UserClass {
	private:
	    Order* head; Order* tail;
	public:
	    Customer(string un, string pw) : UserClass(un, pw, "Customer"), head(nullptr), tail(nullptr) {}
	    ~Customer();
	    void displayMenu() override;
	    void addOrder(); void displayOrders(); void syncFromGlobalList();
	};

Customer::~Customer() {
    Order* current = head;
    while (current != nullptr) { Order* temp = current; current = current->next; delete temp; }
}

void Customer::syncFromGlobalList() {
    Order* curr = head;
    while (curr != nullptr) { Order* t = curr; curr = curr->next; delete t; }
    head = nullptr; tail = nullptr;

    Order* current = orderHead;
    while (current != nullptr) {
        if (current->operatorName == this->username) { 
            Order* newOrder = new Order(*current); newOrder->next = nullptr;
            if (head == nullptr) { head = newOrder; tail = newOrder; }
            else { tail->next = newOrder; tail = newOrder; }
        }
        current = current->next;
    }
}

void Customer::addOrder() {
    Order* newOrder = new Order;
    cout << "\n-- Create New Order --\n";
    cout << "Enter Order ID: "; newOrder->orderId = getSafeInput();
    cout << "Enter Product ID to purchase: "; newOrder->productId = getSafeInput();
    
    Product* checkProd = inventory.binarySearch(newOrder->productId);
    if (!checkProd) {
        cout << "[WARNING] Product ID not found in inventory! Order aborted.\n";
        delete newOrder; return;
    }
    cout << "Enter Quantity: "; newOrder->dispatchQuantity = getSafeInput();
    if (newOrder->dispatchQuantity > checkProd->stockQuantity) {
        cout << "[WARNING] Stock insufficient! Only " << checkProd->stockQuantity << " available.\n";
        delete newOrder; return;
    }
    
    cout << "Enter Order Date (YYYY-MM-DD): "; 
    string rawDate;
    cin >> rawDate;
    newOrder->orderDate = cleanString(rawDate); // clean user input
    
    newOrder->operatorName = this->username; newOrder->next = nullptr;
    
    if (head == nullptr) { head = newOrder; tail = newOrder; }
    else { tail->next = newOrder; tail = newOrder; }
    
    Order* globalNode = new Order(*newOrder);
    if (orderHead == nullptr) { orderHead = globalNode; }
    else {
        Order* t = orderHead;
        while (t->next != nullptr) t = t->next;
        t->next = globalNode;
    }
    cout << "\n[SUCCESS] Order created successfully!\n";
}

void Customer::displayOrders() {
    cout << "\n-- My Order Records --\n";
    if (head == nullptr) { cout << "[INFO] No order history for user: " << this->username << "\n"; return; }
    Order* current = head;
    while (current != nullptr) {
        cout << "Order ID: " << current->orderId << " | Product ID: " << current->productId
             << " | Qty: " << current->dispatchQuantity << " | Date: " << current->orderDate << "\n";
        current = current->next;
    }
}

void Customer::displayMenu() {
    int choice = 0; bool keepRunning = true;
    while (keepRunning) {
        syncFromGlobalList();
        cout << "\n==============================\n       Customer Menu\n==============================\n"
             << "1. Make New Order\n2. View My Orders\n3. Logout\nEnter choice: ";
        choice = getSafeInput();
        if (choice == 1) addOrder();
        else if (choice == 2) displayOrders();
        else if (choice == 3) keepRunning = false;
    }
}

// Clear Memory (main.cpp)
void clearMemory(){
    inventory.clearAll(); 

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

// // Main function
int main() {
    cout <<"==========================================="<<endl;
    cout <<"         Warehouse Inventory System        "<<endl;
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
        
        // clear buffer before getting user input
        if (cin.peek() == '\n') cin.ignore(); 

        switch (choice) {
            case 1: {
                cout << "\n--- Admin Login ---" << endl;
                cout << "Username: "; 
                getline(cin, inputUser); // getline
                inputUser = cleanString(inputUser);

                cout << "Password: "; 
                getline(cin, inputPass);
                inputPass = cleanString(inputPass);

                if (authenticateUser(inputUser, inputPass, "Admin")) {
                    cout << "\nAccess Granted. Welcome, Admin " << inputUser << "!" << endl;
                    Admin adminObj(inputUser);
                    adminObj.displayMenu(); 
                } else {
                    cout << "\nAccess Denied. Incorrect username or password." << endl;
                }
                break;
            }
            case 2: {
                cout << "\n--- Customer Login ---" << endl;
                cout << "Username: "; 
                getline(cin, inputUser); // getline
                inputUser = cleanString(inputUser);

                cout << "Password: "; 
                getline(cin, inputPass);
                inputPass = cleanString(inputPass);

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
                cout << "System Error: Invalid option. Please try again." << endl;
        }
    }

    saveDataToFile();
    saveOrdersToFile();
    saveUsersToFile();
    clearMemory();
    
    cout << "System Terminated." << endl;
    return 0;
}
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <limits>
#include <algorithm>
#include <cctype>
#include <cstdlib>

using namespace std;

// Helper function: remove unwanted characters from input
string cleanString(string str) {
    while (!str.empty() && (str.back() == '\r' || str.back() == '\n' || 
                            str.back() == ' ' || isspace((unsigned char)str.back()) || 
                            (unsigned char)str.back() == 12289 || str.back() == '?')) {
        str.pop_back();
    }
    return str;
}

// CORE STRUCT DEFINITIONS
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
    string role; // Admin, SuperAdmin, Customer
    
    User* next; 
};

// PART 2: CLASS DECLARATIONS & OOP INHERITANCE
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
    void insertNode(int id, string name, string cat, int qty, string z, string sup, double price);
    void deleteNode(int id);
    void sortList(int sortBy);
    Product* binarySearch(int targetId);
    void display();
    Product* getHead() { return head; }
    void clearAll(); 
};

// Global variables (To match extern statements in backend/admin systems)
ProductLinkedList inventory; 
Order* orderHead = nullptr;  
User* userHead = nullptr;    

// File handling safety check
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


// File I/O Engine (Aligned flawlessly with admin.cpp specs)

void saveInventoryToFile(ProductLinkedList& inv) {
    ofstream outFile("Inventory.txt", ios::trunc); 
    if (!outFile) {
        cout << "[System Warning] Cannot write to Inventory.txt!" << endl;
        return;
    }
    Product* temp = inv.getHead();
    while (temp != NULL) {
        outFile << temp->productId << "|" 
                << temp->productName << "|" 
                << temp->category << "|" 
                << temp->stockQuantity << "|" 
                << temp->zone << "|" 
                << temp->supplier << "|" 
                << temp->productPrice << "\n";
        temp = temp->next;
    }
    outFile.close();
    cout << "[System] Inventory.txt auto-saved." << endl;
}

void saveUsersToFile() {
    ofstream outFile("Admin.txt", ios::trunc);
    if (!outFile) {
        cout << "[System Warning] Cannot write to Admin.txt!" << endl;
        return;
    }
    User* temp = userHead;
    while (temp != NULL) {
        outFile << temp->username << "|" 
                << temp->password << "|" 
                << temp->role << "\n";
        temp = temp->next;
    }
    outFile.close();
    cout << "[System] Admin.txt auto-saved." << endl;
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

void loadDataFromFile(){
    ifstream inFile("Inventory.txt");
    if (!inFile){
        cout << "System Notice: Inventory.txt not found. Starting with empty inventory." << endl;
        return;
    }
    
    string line;
    // Safe sequential read to avoid duplicate bottom lines
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string idStr, name, cat, qtyStr, z, sup, priceStr;
        
        if (getline(ss, idStr, '|') && getline(ss, name, '|') && getline(ss, cat, '|') &&
            getline(ss, qtyStr, '|') && getline(ss, z, '|') && getline(ss, sup, '|')) {
            getline(ss, priceStr);
            
            int id = atoi(idStr.c_str());
            int qty = atoi(qtyStr.c_str());
            double price = atof(priceStr.c_str());
            
            inventory.insertNode(id, cleanString(name), cleanString(cat), qty, cleanString(z), cleanString(sup), price);
        }
    }
    inFile.close();
    cout << "System: Inventory data loaded successfully via '|' delimiter." << endl;
}

void loadUsersFromFile(){
    ifstream inFile("Admin.txt");
    if(!inFile){
        cout << "System Error: Admin.txt not found! Login will fail." << endl;
        return;
    }
    
    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string user, pass, role;

        if (getline(ss, user, '|') && getline(ss, pass, '|')) {
            getline(ss, role);
            
            user = cleanString(user);
            pass = cleanString(pass);
            role = cleanString(role);

            if(user.empty()) continue;

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
    }
    inFile.close();
    cout << "System: User credentials loaded successfully." << endl;  
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
            getline(ss, opName, '|')) {
            getline(ss, oDate);
            Order* newNode = new Order;
            newNode->orderId = stoi(oId);
            newNode->productId = stoi(pId);
            newNode->dispatchQuantity = stoi(qty);
            newNode->operatorName = cleanString(opName);
            newNode->orderDate = cleanString(oDate); 
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

void checkSupportFile(const char* fileName){
    ifstream inFile(fileName);
    if (!inFile) {
        ofstream createFile(fileName);
        createFile.close();
        cout <<"System Warning: " << fileName << " not found. Empty file created. " << endl;
    } else {
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
    newNode->username = username;
    newNode->password = password;
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


// Linked List Sorting & Searching Engine 
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

Product* mergeLinkedLists(Product* first, Product* second, int sortBy) {
    if (!first) return second;
    if (!second) return first;

    bool condition = false;
    if (sortBy == 1) condition = first->stockQuantity <= second->stockQuantity;
    else if (sortBy == 2) condition = first->productPrice <= second->productPrice;
    else condition = first->productId <= second->productId;

    Product* result = nullptr;
    if (condition) {
        result = first;
        result->next = mergeLinkedLists(first->next, second, sortBy);
    } else {
        result = second;
        result->next = mergeLinkedLists(first, second->next, sortBy);
    }
    return result;
}

void splitLinkedList(Product* source, Product** frontRef, Product** backRef) {
    Product* fast;
    Product* slow;
    slow = source;
    fast = source->next;

    while (fast != nullptr) {
        fast = fast->next;
        if (fast != nullptr) {
            slow = slow->next;
            fast = fast->next;
        }
    }
    *frontRef = source;
    *backRef = slow->next;
    slow->next = nullptr;
}

void mergeSortLinkedList(Product** headRef, int sortBy) {
    Product* head = *headRef;
    if (!head || !head->next) return;

    Product* a;
    Product* b;
    splitLinkedList(head, &a, &b);

    mergeSortLinkedList(&a, sortBy);
    mergeSortLinkedList(&b, sortBy);

    *headRef = mergeLinkedLists(a, b, sortBy);
}

void ProductLinkedList::sortList(int sortBy) {
    if (count <= 1) return;
    mergeSortLinkedList(&head, sortBy);
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

Product* getMiddleNode(Product* start, Product* end) {
    if (start == nullptr) return nullptr;
    Product* slow = start;
    Product* fast = start->next;

    while (fast != end) {
        fast = fast->next;
        if (fast != end) {
            slow = slow->next;
            fast = fast->next;
        }
    }
    return slow;
}

Product* ProductLinkedList::binarySearch(int targetId) {
    if (count == 0) return nullptr;
    sortList(0); // Ensure it's sorted by ID for Binary Search logic

    Product* start = head;
    Product* end = nullptr;

    while (start != end) {
        Product* mid = getMiddleNode(start, end);
        if (mid == nullptr) return nullptr;

        if (mid->productId == targetId) return mid;
        else if (mid->productId > targetId) end = mid;
        else start = mid->next;
    }
    return nullptr;
}

void ProductLinkedList::display() {
    Product* temp = head;
    if(!temp) { cout << "[System Info] Inventory is completely empty." << endl; return;}
    while (temp != NULL) {
        cout << "[ID: " << temp->productId << " | Name: " << temp->productName 
             << " | Cat: " << temp->category << " | Qty: " << temp->stockQuantity 
             << " | Zone: " << temp->zone << " | Supplier: " << temp->supplier 
             << " | Price: RM" << temp->productPrice << "]\n";
        temp = temp->next;
    }
}


// ROLE CLASSES IMPLEMENTATION (Admin Specs Compliant)
class AdminMenu : public UserClass {
public:
    AdminMenu(string un, string pw) : UserClass(un, pw, "Admin") {}
    
    void addProduct(ProductLinkedList& inv) {
        int id, qty; string name, cat, zone, supplier; double price;
        cout << "\n--- Add Product ---" << endl;
        cout << "ID: "; id = getSafeInput();
        cout << "Name: "; cin >> name;
        cout << "Category: "; cin >> cat;
        cout << "Quantity: "; qty = getSafeInput();
        cout << "Zone: "; cin >> zone;
        cout << "Supplier: "; cin >> supplier;
        cout << "Price: RM "; cin >> price;

        inv.insertNode(id, name, cat, qty, zone, supplier, price);
        cout << "[Success] Product added to linked list!" << endl;
        saveInventoryToFile(inv); // Dynamic saving
    }

    void updateStock(ProductLinkedList& inv) {
        int id, newQty;
        cout << "\n--- Update Stock ---" << endl;
        cout << "Enter Product ID: "; id = getSafeInput();

        Product* p = inv.binarySearch(id);
        if (p != NULL) { 
            cout << "[Found] " << p->productName << " | Current Qty: " << p->stockQuantity << endl;
            cout << "Enter New Qty: "; newQty = getSafeInput(); 
            p->stockQuantity = newQty; 
            cout << "[Success] Stock updated!" << endl;
            saveInventoryToFile(inv);
        } else {
            cout << "[Error] Product not found." << endl;
        }
    }

    void sortAndDisplay(ProductLinkedList& inv) {
        int s; 
        cout << "\n--- Sort Inventory ---" << endl;
        cout << "Sort by (1:Qty, 2:Price): "; s = getSafeInput();
        if (s == 1 || s == 2) {
            inv.sortList(s); 
            inv.display();
        } else {
            cout << "[Error] Invalid sorting option." << endl;
        }
    }

    void searchProduct(ProductLinkedList& inv) {
        int id; 
        cout << "\n--- Search Product ---" << endl;
        cout << "Enter Product ID: "; id = getSafeInput();

        Product* p = inv.binarySearch(id);
        if (p != NULL) { 
            cout << "\n[Found] ID: " << p->productId << " | Name: " << p->productName << endl;
            cout << "Category: " << p->category << " | Qty: " << p->stockQuantity << endl;
            cout << "Zone: " << p->zone << " | Supplier: " << p->supplier << endl;
            cout << "Price: RM " << p->productPrice << endl;
        } else {
            cout << "[Error] Product ID " << id << " not found." << endl;
        }
    }

    void displayMenu() override {
        int choice;
        bool inMenu = true;
        while (inMenu) {
            cout << "\n=== Admin Control Panel | User: " << username << " ===" << endl;
            cout << "1. View Inventory\n2. Sort Inventory (Merge Sort)\n3. Search Product (Binary Search)\n4. Add Product\n5. Update Stock\n6. Logout\nChoice: ";
            choice = getSafeInput();

            if (choice == 1) inventory.display();
            else if (choice == 2) sortAndDisplay(inventory);
            else if (choice == 3) searchProduct(inventory);
            else if (choice == 4) addProduct(inventory);
            else if (choice == 5) updateStock(inventory);
            else if (choice == 6) inMenu = false;
            else cout << "[Error] Invalid option." << endl;
        }
    }
};

class SuperAdminMenu : public UserClass {
public:
    SuperAdminMenu(string un, string pw) : UserClass(un, pw, "SuperAdmin") {}

    void deleteProduct(ProductLinkedList& inv) {
        int id; 
        cout << "\n--- Delete Product ---" << endl;
        cout << "Enter Product ID to delete: "; id = getSafeInput();

        Product* p = inv.binarySearch(id);
        if (p != NULL) {
            inv.deleteNode(id);
            cout << "[Success] Product deleted." << endl;
            saveInventoryToFile(inv);
        } else {
            cout << "[Error] Product ID not found." << endl;
        }
    }

    void addAdmin() {
        string un, pw; 
        cout << "\n--- Register New Admin ---" << endl;
        cout << "New Username: "; cin >> un; 
        cout << "New Password: "; cin >> pw;
    
        if (isUsernameExist(un)) {
            cout << "[Error] Username already exists!" << endl;
            return;
        }
        addUserToList(un, pw, "Admin");
        cout << "[Success] New admin added to system." << endl;
        saveUsersToFile();
    }

    void displayMenu() override {
        int choice;
        bool inMenu = true;
        AdminMenu tempProxy(username, password); // Proxy Design pattern reuse
        while (inMenu) {
            cout << "\n=== [MASTER] SuperAdmin Panel | User: " << username << " ===" << endl;
            cout << "1. View Inventory\n2. Sort Inventory\n3. Search Product\n4. Add Product\n5. Update Stock\n";
            cout << "6. Delete Product [High Privilege]\n7. Add Admin [High Privilege]\n8. Logout\nChoice: ";
            choice = getSafeInput();

            if (choice == 1) inventory.display();
            else if (choice == 2) tempProxy.sortAndDisplay(inventory);
            else if (choice == 3) tempProxy.searchProduct(inventory);
            else if (choice == 4) tempProxy.addProduct(inventory);
            else if (choice == 5) tempProxy.updateStock(inventory);
            else if (choice == 6) deleteProduct(inventory);
            else if (choice == 7) addAdmin();
            else if (choice == 8) inMenu = false;
            else cout << "[Error] Invalid option." << endl;
        }
    }
};

// Customer Implementation
class Customer : public UserClass {
private:
    Order* head; 
    Order* tail;
public:
    Customer(string un, string pw) : UserClass(un, pw, "Customer"), head(nullptr), tail(nullptr) {}
    ~Customer();
    void displayMenu() override;
    void addOrder(); 
    void displayOrders(); 
    void syncFromGlobalList();
};

Customer::~Customer() {
    Order* current = head;
    while (current != nullptr) { 
        Order* temp = current; 
        current = current->next; 
        delete temp; 
    }
}

void Customer::syncFromGlobalList() {
    Order* curr = head;
    while (curr != nullptr) { 
        Order* t = curr; 
        curr = curr->next; 
        delete t; 
    }
    head = nullptr; tail = nullptr;

    Order* current = orderHead;
    while (current != nullptr) {
        if (current->operatorName == this->username) { 
            Order* newOrder = new Order(*current); 
            newOrder->next = nullptr;
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
    newOrder->orderDate = cleanString(rawDate); 
    
    newOrder->operatorName = this->username; 
    newOrder->next = nullptr;
    
    // 1. Link into local context
    if (head == nullptr) { head = newOrder; tail = newOrder; }
    else { tail->next = newOrder; tail = newOrder; }
    
    // 2. Link into global database log context
    Order* globalNode = new Order(*newOrder);
    if (orderHead == nullptr) { orderHead = globalNode; }
    else {
        Order* t = orderHead;
        while (t->next != nullptr) t = t->next;
        t->next = globalNode;
    }

    // 3. Deduct actual stocks instantly & write directly to disk files
    checkProd->stockQuantity -= newOrder->dispatchQuantity;
    saveInventoryToFile(inventory);
    saveOrdersToFile(); 
    
    cout << "\n[SUCCESS] Order created successfully! Stock updated dynamically.\n";
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

// Clear Memory Safely
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

// Main Gateway
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
        cout << "2. Login as SuperAdmin "<<endl;
        cout << "3. Login as Customer "<<endl;
        cout << "4. Register as Customer" <<endl;
        cout << "5. Exit System" <<endl;
        cout << "Select your role: ";
        
        int choice = getSafeInput();
        if (cin.peek() == '\n') cin.ignore(); 

        switch (choice) {
            case 1: {
                cout << "\n--- Admin Login ---" << endl;
                cout << "Username: "; getline(cin, inputUser); inputUser = cleanString(inputUser);
                cout << "Password: "; getline(cin, inputPass); inputPass = cleanString(inputPass);

                if (authenticateUser(inputUser, inputPass, "Admin")) {
                    cout << "\nAccess Granted. Welcome, Admin " << inputUser << "!" << endl;
                    AdminMenu adminObj(inputUser, inputPass);
                    adminObj.displayMenu(); 
                } else {
                    cout << "\nAccess Denied. Incorrect credentials or role mismatch." << endl;
                }
                break;
            }
            case 2: {
                cout << "\n--- SuperAdmin Login ---" << endl;
                cout << "Username: "; getline(cin, inputUser); inputUser = cleanString(inputUser);
                cout << "Password: "; getline(cin, inputPass); inputPass = cleanString(inputPass);

                if (authenticateUser(inputUser, inputPass, "SuperAdmin")) {
                    cout << "\n[SUCCESS] Access Granted. Welcome, " << inputUser << "!" << endl;
                    SuperAdminMenu superObj(inputUser, inputPass);
                    superObj.displayMenu(); 
                } else {
                    cout << "\nAccess Denied. Incorrect credentials or role mismatch." << endl;
                }
                break;
            }
            case 3: {
                cout << "\n--- Customer Login ---" << endl;
                cout << "Username: "; getline(cin, inputUser); inputUser = cleanString(inputUser);
                cout << "Password: "; getline(cin, inputPass); inputPass = cleanString(inputPass);

                if (authenticateUser(inputUser, inputPass, "Customer")) {
                    cout << "\nAccess Granted. Welcome, " << inputUser << "!" << endl;
                    Customer customerObj(inputUser, inputPass);
                    customerObj.displayMenu();
                } else {
                    cout << "\nAccess Denied. Incorrect username or password." << endl;
                }
                break;
            }
            case 4:{
                registerCustomer();
                break;
            }
            case 5: {
                cout << "\nSystem: Preparing to shutdown..." << endl;
                isRunning = false;
                break;
            }
            default:
                cout << "System Error: Invalid option. Please try again." << endl;
        }
    }

    // Defensive Save Actions On Termination Gateway
    saveInventoryToFile(inventory);
    saveOrdersToFile();
    saveUsersToFile();
    clearMemory();
    
    cout << "System Terminated." << endl;
    return 0;
}
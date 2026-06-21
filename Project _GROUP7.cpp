#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <ctime>
#include <cstdio>

using namespace std;

// ==========================================
// 1. Structures & Base Class Definitions
// ==========================================
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
    int categoryId;
    int dispatchQuantity;
    string operatorName;
    string orderDate;
    
    Order* next;
};

class User {
public:
    string username, password, role;
    User* next;
    friend void printUserProfile(const User& u);

    User(string un, string pw, string r) : username(un), password(pw), role(r), next(NULL) {}
    virtual ~User() {}

    virtual void displayMenu() = 0; 
};

// ==========================================
// 2. Class Declarations
// ==========================================

// ProductLinkedList Declaration
class ProductLinkedList {
private:
    Product* head;
    int currentSort;
    int count;

    void Merge(Product** arr, int lpos, int rpos, int rend, int sortBy);
    void MergeSortRec(Product** arr, int left, int right, int sortBy);

public:
    ProductLinkedList();
    ~ProductLinkedList();

    void insertNode(int id, string name, string cat, int qty, string z, string sup, double price);
    void deleteNode(int id);
    void sortList(int sortBy);
    Product* binarySearch(int targetId);
    void display();
    friend void displayInventoryForCustomer(const ProductLinkedList& inv);
    
    Product* getHead() const { return head; } 
    int getCount() const { return count; }    
    friend void generateStockSummary(const ProductLinkedList& inv);
};

// Admin & SuperAdmin Declarations
class Admin : public User {
public:
    Admin(string un, string pw);
    ~Admin();
    
    void displayMenu();
    
    void addProduct(ProductLinkedList& inventory);
    void updateStock(ProductLinkedList& inventory);
    void sortAndDisplay(ProductLinkedList& inventory);
    void searchProduct(ProductLinkedList& inventory);
    void generateLowStockReport(ProductLinkedList& inventory);
};

class SuperAdmin : public User {
public:
    SuperAdmin(string un, string pw);
    ~SuperAdmin();
    
    void displayMenu();
    
    void addProduct(ProductLinkedList& inventory);
    void updateStock(ProductLinkedList& inventory);
    void deleteProduct(ProductLinkedList& inventory);
    void sortAndDisplay(ProductLinkedList& inventory);
    void searchProduct(ProductLinkedList& inventory);
    void addAdmin();
    void generateLowStockReport(ProductLinkedList& inventory);
};

// Customer Declaration
class Customer : public User {
private:
    Order* head;
    Order* tail;
    
public:
    Customer(string username = "", string password =""); 
    ~Customer();
    
    void addOrder();
    void displayOrders();
    void searchOrder();
    void editOrder();
    void deleteOrder();
    void generateReport();
    void sortOrders(int criteria);
    void displayMenu();

    friend void adminViewCustomerOrders(const Customer& cust);
};

// ==========================================
// 3. Global Variables & Constants
// ==========================================
User* userHead = NULL;
ProductLinkedList globalInventory;

const char* ORDER_SUMMARY_FILE = "data\\OrderSummary.txt";
const char* ORDERS_FILE = "data\\Orders.txt";

// ==========================================
// 4. Forward Declarations of Global Helpers
// ==========================================
void saveInventoryToFile(ProductLinkedList& inventory);
void saveAdminsToFile();
void loadInventoryFromFile(ProductLinkedList& inventory);
void loadAdminsFromFile();
void saveCustomersToFile();
void loadCustomersFromFile();
void displayInventoryForCustomer(const ProductLinkedList& inv);
void generateStockSummary(const ProductLinkedList& inv);
void adminViewCustomerOrders(const Customer& cust);
void printUserProfile(const User& u);

void clearInputBuffer();
string getValidLineData(string prompt);
bool isValidAdminname(string username);
bool isValidAdminPassword(string password);
int getValidIntData(string prompt);
double getValidDoubleData(string prompt);

string getCurrentDateTime();
int getNextOrderId(Order* head);
Product* findProductById(int productId);
void displayAvailableInventory();
bool reduceInventoryStock(int productId, int orderQty);
void saveOrderSummaryFile(Order* head, string customerName);
void appendOrderToFile(Order* order, string customerName);
void loadOrdersFromFile(Order*& head, Order*& tail, string customerName);

int getSafeInput();
bool isUsernameExist(string username);
bool isValidUsername(string username);
bool isValidPassword(string password);
void addUserToList(string username, string password, string role);
void registerCustomer();
User* authenticateUser(string inputUser, string inputPass, string expectedRole);
void clearMemory();

// ==========================================
// 5. Global Function Implementations
// ==========================================

// Helper: truncate a field (from Lead_backend.cpp)
static string truncateField(const string& text, int width) {
    if ((int)text.length() <= width) return text;
    if (width <= 3) return text.substr(0, width); 
    return text.substr(0, width - 3) + "...";
}

// File I/O Engine Implementations (from admin.cpp)
void saveInventoryToFile(ProductLinkedList& inventory) {
    ofstream outFile("data/Inventory.txt", ios::out | ios::trunc);
    if (!outFile) {
        cout << "[System Warning] Cannot write to Inventory.txt!" << endl;
        return;
    }
    Product* temp = inventory.getHead();
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
}

void saveAdminsToFile() {
    ofstream outFile("data/Admin.txt", ios::out | ios::trunc);
    if (!outFile) {
        cout << "[System Warning] Cannot write to Admin.txt!" << endl;
        return;
    }
    User* temp = userHead;
    while (temp != NULL) {
        if (temp->role == "Admin" || temp->role == "SuperAdmin") {
            outFile << temp->username << "|"
                    << temp->password << "|"
                    << temp->role << "\n";
        }
        temp = temp->next;
    }
    outFile.close();
}

void loadInventoryFromFile(ProductLinkedList& inventory) {
    ifstream inFile("data/Inventory.txt");
    if (!inFile) {
        cout << "[System Notice] Inventory.txt not found. Starting fresh." << endl;
        return; 
    }

    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue; 

        stringstream ss(line);
        string tempId, name, cat, tempQty, zone, sup, tempPrice;

        getline(ss, tempId, '|');
        getline(ss, name, '|');
        getline(ss, cat, '|');
        getline(ss, tempQty, '|');
        getline(ss, zone, '|');
        getline(ss, sup, '|');
        getline(ss, tempPrice, '|');

        int id = atoi(tempId.c_str());
        int qty = atoi(tempQty.c_str());
        double price = atof(tempPrice.c_str());

        inventory.insertNode(id, name, cat, qty, zone, sup, price);
    }
    inFile.close();
}

void loadAdminsFromFile() {
    ifstream inFile("data/Admin.txt");
    if (!inFile) return;

    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;

        if (!line.empty() && line[line.length() - 1] == '\r') {
            line.erase(line.length() - 1);
        }
        if (line.empty()) continue;

        stringstream ss(line);
        string un, pw, role;
        getline(ss, un,   '|');
        getline(ss, pw,   '|');
        getline(ss, role, '|');

        User* n = NULL;
        if (role == "SuperAdmin") n = new SuperAdmin(un, pw);
        else if (role == "Admin") n = new Admin(un, pw);
        else continue; 

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

void viewInventoryWithSortOption(ProductLinkedList& inventory) {
    inventory.display();

    char ans;
    cout << "\nDo you want to sort the inventory now? (Y/N): ";
    cin >> ans;

    if (cin.fail()) {
        cin.clear(); cin.ignore(1000, '\n');
        return;
    }

    if (ans == 'Y' || ans == 'y') {
        int s;
        cout << "Sort by (1: Quantity, 2: Price): ";
        cin >> s;

        if (cin.fail()) {
            cin.clear(); cin.ignore(1000, '\n');
            cout << "[Error] Invalid input." << endl;
            return;
        }

        if (s == 1 || s == 2) {
            inventory.sortList(s);

            string sortLabel = (s == 1) ? "Quantity" : "Price";
            cout << "\n[System] Inventory is now sorted by: " << sortLabel << endl;

            inventory.display();
        } else {
            cout << "[Error] Invalid sorting option." << endl;
        }
    }
}

void clearInputBuffer() {
    cin.clear();
    cin.ignore(10000, '\n');
}

string getValidLineData(string prompt) {
    string value;
    while (true) {
        cout << prompt;
        getline(cin, value);
        
        bool isEmpty = true;
        for (size_t i = 0; i < value.length(); i++) {
            if (value[i] != ' ' && value[i] != '\t') {
                isEmpty = false;
                break;
            }
        }

        bool hasDelimiter = (value.find('|') != string::npos);

        if (!isEmpty && !hasDelimiter) {
            return value;
        }
        cout << " [Error] Input cannot be empty or contain illegal character '|'!\n";
    }
}

bool isValidAdminname(string username) {
    if (username.length() < 3 || username.length() > 20) {
        return false;
    }
    
    for (size_t i = 0; i < username.length(); i++) {
        char c = username[i];
        
        bool isLetter = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
        bool isDigit = (c >= '0' && c <= '9');
        bool isUnderscore = (c == '_');
        
        if (!isLetter && !isDigit && !isUnderscore) {
            return false;
        }
    }
    return true;
}

bool isValidAdminPassword(string password) {
    if (password.length() < 4 || password.length() > 20) {
        return false;
    }
    
    for (size_t i = 0; i < password.length(); i++) {
        if (password[i] == '|') {
            return false;
        }
    }
    return true;
}

int getValidIntData(string prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value > 0) {
            clearInputBuffer(); 
            return value;
        }
        cout << " [Error] Please enter a valid positive integer (> 0)!\n";
        clearInputBuffer();
    }
}

double getValidDoubleData(string prompt) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value && value > 0.0) {
            clearInputBuffer(); 
            return value;
        }
        cout << " [Error] Please enter a valid price (cannot be negative)!\n";
        clearInputBuffer();
    }
}

// Customer helper implementations
string getCurrentDateTime(){
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
        cout << "[ERROR] Cannot open Inventory.txt.\n";
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
        cout << "[ERROR] Cannot open Inventory.txt.\n";
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

    cout << "===================================================\n";
    inFile.close();
}

bool reduceInventoryStock(int productId, int orderQty) {
    ifstream inFile("data\\Inventory.txt");
    ofstream tempFile("data\\Inventory_temp.txt");

    if (!inFile || !tempFile) {
        cout << "[ERROR] Unable to open Inventory.txt for stock update.\n";
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
                cout << "[SYSTEM] Stock before you order: " << currentStock << "\n";
                currentStock = currentStock - orderQty;
                cout << "[SYSTEM] Stock after you order : " << currentStock << "\n";
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

void saveOrderSummaryFile(Order* head, string customerName) {
    ofstream outFile("data\\OrderSummary.txt");

    if (!outFile) {
        cout << "[ERROR] Unable to create OrderSummary.txt!\n";
        return;
    }

    outFile << "=========================================\n";
    outFile << "        ORDER SUMMARY REPORT             \n";
    outFile << "=========================================\n\n";

    if (head == NULL){
        outFile << "No order record avaliable.\n";
        outFile.close();
        return;
    }

    Order* current = head;
    int count = 1;

    while (current != NULL) {
        Product* product = findProductById(current->productId);

        outFile << "Record #" << count << "\n";
        outFile << "Customer Name : " << customerName << "\n";
        outFile << "Order ID      : " << current->orderId << "\n";
        outFile << "Product ID    : " << current->productId << "\n";

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

void appendOrderToFile(Order* order, string customerName) {
    ofstream outFile(ORDERS_FILE, ios::app);

    if (!outFile) {
        cout << "[ERROR] Unable to write to data\\Orders.txt!\n";
        return;
    }

    Product* product = findProductById(order->productId);

    string productName = "Unknown Product";
    string category = "Unknown Category";

    if (product != NULL) {
        productName = product->productName;
        category = product->category;
        delete product;
    }

    outFile << customerName << "|"
            << order->orderId << "|"
            << order->productId << "|"
            << productName << "|"
            << category << "|"
            << order->dispatchQuantity << "|"
            << order->orderDate << "\n";

    outFile.close();
}

void loadOrdersFromFile(Order*& head, Order*& tail, string customerName) {
    ifstream inFile(ORDERS_FILE);
    if (!inFile) {
        return;
    }

    string line;
    while (getline(inFile, line)) {
        if (line == "") {
            continue;
        }

        stringstream ss(line);
        string fileCustomer, orderIdStr, productIdStr;
        string productName, category, qtyStr, dateStr;

        getline(ss, fileCustomer, '|');
        getline(ss, orderIdStr, '|');
        getline(ss, productIdStr, '|');
        getline(ss, productName, '|');
        getline(ss, category, '|');
        getline(ss, qtyStr, '|');
        getline(ss, dateStr, '|');

        if (fileCustomer != customerName) {
            continue;
        }

        Order* newOrder = new Order;
        newOrder->orderId = atoi(orderIdStr.c_str());
        newOrder->productId = atoi(productIdStr.c_str());
        newOrder->dispatchQuantity = atoi(qtyStr.c_str());
        newOrder->orderDate = dateStr;
        newOrder->categoryId = 0;
        newOrder->operatorName = customerName;
        newOrder->next = NULL;

        if (head == NULL) {
            head = newOrder;
            tail = newOrder;
        } else {
            tail->next = newOrder;
            tail = newOrder;
        }
    }
    inFile.close();
}

void saveCustomersToFile() {
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

        if (!line.empty() && line[line.length() - 1] == '\r') {
            line.erase(line.length() - 1);
        }
        if (line.empty()) continue;

        stringstream ss(line);
        string un, pw, role;
        getline(ss, un,   '|');
        getline(ss, pw,   '|');
        getline(ss, role, '|');

        if (role != "Customer") continue; 

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

// Main helper implementations
int getSafeInput(){
    int input;
    try {
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

bool isValidUsername(string username){
    if (username.length() < 3 || username.length() > 20){
        return false;
    }
    
    for (size_t i = 0; i < username.length(); i++){
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
    
    for (size_t i = 0; i < password.length(); i++){
        if (password[i] == '|'){
            return false;
        }
    }
    return true;
}

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

void registerCustomer(){
    string username, password, confirmPassword;
    bool validUsername = false;
    bool validPassword = false;
    
    cout <<"\n~+~Customer Registration~+~" <<endl;
    cout <<"Enter 0 anytime for cancel registration."<<endl;

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

void clearMemory(){
    User* userTemp;
    while(userHead != NULL){
        userTemp = userHead;
        userHead = userHead->next;
        delete userTemp;
    }
    cout <<"System: Memory cleared safely." << endl;
}

// ==========================================
// 6. Class Method Implementations
// ==========================================

// ProductLinkedList Methods
ProductLinkedList::ProductLinkedList() {
    head = NULL;
    count = 0;
    currentSort = -1;
}

ProductLinkedList::~ProductLinkedList() {
    Product* curr = head;
    while (curr != NULL) {
        Product* nextNode = curr->next;
        delete curr;
        curr = nextNode;
    }
}

void ProductLinkedList::Merge(Product** arr, int lpos, int rpos, int rend, int sortBy) {
    int lend = rpos - 1;
    int tmppos = 0;
    int numelements = rend - lpos + 1;
    
    Product** TmpArray = new Product*[numelements];
    int l_cursor = lpos;
    int r_cursor = rpos;

    while (l_cursor <= lend && r_cursor <= rend) {
        bool condition = false;
        if (sortBy == 1) {
            condition = arr[l_cursor]->stockQuantity <= arr[r_cursor]->stockQuantity;
        } else if (sortBy == 2) {
            condition = arr[l_cursor]->productPrice <= arr[r_cursor]->productPrice;
        } else {
            condition = arr[l_cursor]->productId <= arr[r_cursor]->productId;
        }

        if (condition) TmpArray[tmppos++] = arr[l_cursor++];
        else TmpArray[tmppos++] = arr[r_cursor++];
    }

    while (l_cursor <= lend) TmpArray[tmppos++] = arr[l_cursor++];
    while (r_cursor <= rend) TmpArray[tmppos++] = arr[r_cursor++];

    for (int i = 0; i < numelements; i++) {
        arr[rend - i] = TmpArray[numelements - 1 - i];
    }
    
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
    temp->productId = id;
    temp->productName = name;
    temp->category = cat;
    temp->stockQuantity = qty;
    temp->zone = z;
    temp->supplier = sup;
    temp->productPrice = price;
    temp->next = NULL;

    if (head == NULL) {
        head = temp;
    } else {
        Product* curr = head;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = temp;
    }
    count++;
    currentSort = -1;
}

void ProductLinkedList::deleteNode(int id) {
    if (head == NULL) {
        cout << "List is empty. Nothing to delete." << endl;
        return;
    }
    
    Product* temp = head;
    Product* prev = NULL;
    
    if (head->productId == id) {
        head = head->next;
        delete temp;
        count--;
        currentSort = -1;
    } else {
        while (temp != NULL && temp->productId != id) {
            prev = temp;
            temp = temp->next;
        }
        if (temp) {
            prev->next = temp->next;
            delete temp;
            count--;
            currentSort = -1;
        }
    }
}

void ProductLinkedList::sortList(int sortBy) {
    if (count <= 1) return;

    Product** TmpArray = new Product*[count];
    Product* curr = head;
    int i = 0;
    while (curr != NULL) {
        TmpArray[i++] = curr;
        curr = curr->next;
    }

    MergeSortRec(TmpArray, 0, count - 1, sortBy);

    head = TmpArray[0];
    curr = head;
    for (i = 1; i < count; i++) {
        curr->next = TmpArray[i];
        curr = curr->next;
    }
    curr->next = NULL;
    currentSort = sortBy;

    delete[] TmpArray;
}

Product* ProductLinkedList::binarySearch(int targetId) {
    if (count == 0) return NULL;

    if (currentSort != 0) {
        sortList(0); 
    }

    Product** TmpArray = new Product*[count];
    Product* curr = head;
    int i = 0;
    while (curr != NULL) {
        TmpArray[i++] = curr;
        curr = curr->next;
    }

    int first = 0;
    int last = count - 1;
    int mid;
    Product* found = NULL;
    
    while (first <= last) {
        mid = (first + last) / 2;
        if (TmpArray[mid]->productId == targetId) {
            found = TmpArray[mid];
            break;
        } else if (TmpArray[mid]->productId > targetId) {
            last = mid - 1;
        } else {
            first = mid + 1;
        }
    }

    delete[] TmpArray;
    return found;
}

void ProductLinkedList::display() {
    if (head == NULL) {
        cout << "[System] Inventory is empty.\n";
        return;
    }

    cout <<"\n"<< left
         << setw(6)  << "ID"
         << setw(20) << "Name"
         << setw(15) << "Category"
         << setw(10) << "Zone"
         << setw(15) << "Supplier"
         << setw(8)  << "Qty"
         << right << setw(12) << "Price (RM)"
         << "\n";
    cout << string(86, '-') << "\n";

    cout << fixed << setprecision(2);
    Product* temp = head;
    while (temp != NULL) {
        cout << left
             << setw(6)  << temp->productId
             << setw(20) << truncateField(temp->productName, 19)
             << setw(15) << truncateField(temp->category, 14)
             << setw(10) << truncateField(temp->zone, 9)
             << setw(15) << truncateField(temp->supplier, 14)
             << setw(8)  << temp->stockQuantity
             << right << setw(12) << temp->productPrice
             << "\n";
        temp = temp->next;
    }
    cout << string(86, '-') << "\n";
}

// Admin
Admin::Admin(string un, string pw) : User(un, pw, "Admin") { 
    next = NULL; 
}
Admin::~Admin() {}

void Admin::displayMenu() {
    int choice;
    bool inMenu = true;
    while (inMenu) {
        cout<<"\n==============================\n";
        cout<<"\n        Admin Dashboard       \n";
        cout<<"\n==============================\n";
        cout << "1. View Inventory \n";
        cout << "2. Sort Inventory \n";
        cout << "3. Search Product \n";
        cout << "4. Add Product\n";
        cout << "5. Update Stock\n";
        cout << "6. Generate Low Stock Report\n";
        cout << "7. View Customer Orders\n";
        cout << "8. Logout\n";
        cout << "\n================================== \n";
        cout << "Enter a function: ";
        cin >> choice;

        if (cin.fail()) { 
            cin.clear(); cin.ignore(1000, '\n'); 
            cout << "[Error] Please enter a valid number.\n";
            continue; 
        }

        if (choice == 1) viewInventoryWithSortOption(globalInventory);
        else if (choice == 2) sortAndDisplay(globalInventory);
        else if (choice == 3) searchProduct(globalInventory);
        else if (choice == 4) addProduct(globalInventory);
        else if (choice == 5) updateStock(globalInventory);
        else if (choice == 6) generateLowStockReport(globalInventory);
        else if (choice == 7) {
            cout << "\nEnter Customer Username to view orders: ";
            string targetUsername;
            cin >> targetUsername;

            User* temp = userHead;
            bool found = false;

            while (temp != NULL) {
                if (temp->role == "Customer" && temp->username == targetUsername) {
                    Customer* cust = dynamic_cast<Customer*>(temp);
                    if (cust != NULL) {
                        adminViewCustomerOrders(*cust);
                        found = true;
                    }
                    break;
                }
                temp = temp->next;
            }

            if (!found) {
                cout << "[Error] Customer '" << targetUsername << "' not found.\n";
            }
        }
        else if (choice == 8) inMenu = false;
        else cout << "[Error] Invalid option." << endl;
    }
}

void Admin::addProduct(ProductLinkedList& inventory) {
    cout << "\n=========================================\n";
    cout << "        --- Add New Product ---          \n";
    cout << "=========================================\n";
    
    int autoId = 1001; 
    Product* temp = inventory.getHead();
    
    while (temp != NULL) {
        if (temp->productId >= autoId) {
            autoId = temp->productId + 1; 
        }
        temp = temp->next;
    }
    
    cout << " [System] Automatically Generated Product ID: " << autoId << "\n\n";
    clearInputBuffer();
    
    string name     = getValidLineData("Enter Product Name: ");
    string cat      = getValidLineData("Enter Category: ");
    int qty         = getValidIntData("Enter Quantity: ");
    string zone     = getValidLineData("Enter Warehouse Zone: ");
    string supplier = getValidLineData("Enter Supplier: ");
    double price    = getValidDoubleData("Enter Price: RM ");

    cout << "\n=========================================\n";
    cout << "         [Preview] Product Details       \n";
    cout << "-----------------------------------------\n";
    cout << " " << left << setw(16) << "Product ID"     << ": " << autoId << "\n";
    cout << " " << left << setw(16) << "Product Name"   << ": " << name << "\n";
    cout << " " << left << setw(16) << "Category"       << ": " << cat << "\n";
    cout << " " << left << setw(16) << "Quantity"       << ": " << qty << " units\n";
    cout << " " << left << setw(16) << "Warehouse Zone" << ": " << zone << "\n";
    cout << " " << left << setw(16) << "Supplier"       << ": " << supplier << "\n";
    cout << " " << left << setw(16) << "Price"          << ": RM " << fixed << setprecision(2) << price << "\n";
    cout << "=========================================\n";
    
    cout << "Confirm to add this new record? (Y/N): ";
    char confirm;
    cin >> confirm;
    clearInputBuffer(); 

    if (confirm == 'Y' || confirm == 'y') {
        inventory.insertNode(autoId, name, cat, qty, zone, supplier, price);
        cout << "\n [Success] New record appended to memory structure.\n";
        saveInventoryToFile(inventory);
        cout << " [System] Database file auto-saved and synchronized.\n";
    } else {
        cout << "\n [Cancelled] Operation aborted. No data was saved.\n";
    }
}

void Admin::updateStock(ProductLinkedList& inventory) {
    cout << "\n=========================================\n";
    cout << "      --- Edit Full Product Record ---    \n";
    cout << "=========================================\n";
    
    cin.clear();
    int id = getValidIntData("Enter Product ID to Edit: ");

    Product* p = inventory.binarySearch(id);
    
    if (p != NULL) { 
        cout << "\n=========================================\n";
        cout << "         [Current Product Data]          \n";
        cout << "-----------------------------------------\n";
        cout << " " << left << setw(16) << "Product ID"     << ": " << p->productId << "\n";
        cout << " " << left << setw(16) << "Product Name"   << ": " << p->productName << "\n";
        cout << " " << left << setw(16) << "Category"       << ": " << p->category << "\n";
        cout << " " << left << setw(16) << "Quantity"       << ": " << p->stockQuantity << " units\n";
        cout << " " << left << setw(16) << "Warehouse Zone" << ": " << p->zone << "\n";
        cout << " " << left << setw(16) << "Supplier"       << ": " << p->supplier << "\n";
        cout << " " << left << setw(16) << "Price"          << ": RM " << fixed << setprecision(2) << p->productPrice << "\n";
        cout << "=========================================\n";
        cout << "Enter new details below to overwrite this record:\n\n";

        string newName     = getValidLineData("Enter New Product Name: ");
        string newCat      = getValidLineData("Enter New Category: ");
        int newQty         = getValidIntData("Enter New Quantity: ");
        string newZone     = getValidLineData("Enter New Warehouse Zone: ");
        string newSupplier = getValidLineData("Enter New Supplier: ");
        double newPrice    = getValidDoubleData("Enter New Price: RM ");

        cout << "\n=========================================\n";
        cout << "         [Preview Modifed Changes]       \n";
        cout << "-----------------------------------------\n";
        cout << " " << left << setw(16) << "Product ID"     << ": " << id << " (Unchanged)\n";
        cout << " " << left << setw(16) << "Product Name"   << ": " << newName << "\n";
        cout << " " << left << setw(16) << "Category"       << ": " << newCat << "\n";
        cout << " " << left << setw(16) << "Quantity"       << ": " << newQty << " units\n";
        cout << " " << left << setw(16) << "Warehouse Zone" << ": " << newZone << "\n";
        cout << " " << left << setw(16) << "Supplier"       << ": " << newSupplier << "\n";
        cout << " " << left << setw(16) << "Price"          << ": RM " << fixed << setprecision(2) << newPrice << "\n";
        cout << "=========================================\n";
        
        cout << "Are you sure you want to save these modifications? (Y/N): ";
        char confirm;
        cin >> confirm;
        clearInputBuffer();

        if (confirm == 'Y' || confirm == 'y') {
            p->productName   = newName;
            p->category      = newCat;
            p->stockQuantity = newQty;
            p->zone          = newZone;
            p->supplier      = newSupplier;
            p->productPrice  = newPrice;

            cout << "\n [Success] Product records modified perfectly in runtime system!\n";
            saveInventoryToFile(inventory);
            cout << " [System] Changes synchronized to data/Inventory.txt seamlessly.\n";
        } else {
            cout << "\n [Cancelled] Modification sequence aborted. No data was written.\n";
        }
    } else {
        cout << "\n [Error] Product ID " << id << " not discovered inside the database.\n";
    }
}

void Admin::sortAndDisplay(ProductLinkedList& inventory) {
    int s; 
    cout << "\n--- Sort Inventory ---" << endl;
    cout << "Sort by (1:Qty, 2:Price): "; cin >> s;
    if (cin.fail()) { cin.clear(); cin.ignore(1000, '\n'); return; }
    
    if (s == 1 || s == 2) {
        inventory.sortList(s); 
        string sortLabel = (s == 1) ? "Quantity" : "Price";
        cout << "[System] Inventory is now sorted by: " << sortLabel << endl;
        inventory.display();
    } else {
        cout << "[Error] Invalid sorting option." << endl;
    }
}

void Admin::searchProduct(ProductLinkedList& inventory) {
    int id; 
    cout << "\n--- Search Product ---" << endl;
    cout << "Enter Product ID: "; cin >> id;
    if (cin.fail()) { clearInputBuffer(); return; }

    Product* p = inventory.binarySearch(id);
    if (p != NULL) { 
        cout << "\n=========================================\n";
        cout << " [Found] Product Information\n";
        cout << "=========================================\n";
        
        cout << " " << left << setw(15) << "Product ID"   << ": " << p->productId << "\n";
        cout << " " << left << setw(15) << "Product Name" << ": " << p->productName << "\n";
        cout << " " << left << setw(15) << "Category"     << ": " << p->category << "\n";
        cout << " " << left << setw(15) << "Quantity"     << ": " << p->stockQuantity << " units\n";
        cout << " " << left << setw(15) << "Warehouse Zone"<< ": " << p->zone << "\n";
        cout << " " << left << setw(15) << "Supplier"     << ": " << p->supplier << "\n";
        cout << " " << left << setw(15) << "Price"        << ": RM " << fixed << setprecision(2) << p->productPrice << "\n";
        
        cout << "=========================================\n";
    } else {
        cout << "\n[Error] Product ID " << id << " not found." << endl;
    }
}

void Admin::generateLowStockReport(ProductLinkedList& inventory) {
    cout << "\n=========================================\n";
    cout << "        --- Low Stock Report ---          \n";
    cout << "=========================================\n";

    if (inventory.getCount() == 0) {
        cout << " [System] Inventory is empty. No report to generate.\n";
        return;
    }

    inventory.sortList(1); 

    int totalProducts      = 0;
    int totalStockUnits    = 0;
    double totalStockValue = 0.0;
    double totalPriceSum   = 0.0;
    int lowStockCount      = 0;

    Product* temp = inventory.getHead();
    while (temp != NULL) {
        totalProducts++;
        totalStockUnits += temp->stockQuantity;
        totalStockValue += temp->stockQuantity * temp->productPrice;
        totalPriceSum   += temp->productPrice;
        if (temp->stockQuantity <= 10) { 
            lowStockCount++;
        }
        temp = temp->next;
    }
    double averagePrice = totalPriceSum / totalProducts;

    cout << "\n--- Overall Inventory Statistics ---\n";
    cout << fixed << setprecision(2);
    cout << " " << left << setw(22) << "Total Products"        << ": " << totalProducts << "\n";
    cout << " " << left << setw(22) << "Total Stock Units"     << ": " << totalStockUnits << "\n";
    cout << " " << left << setw(22) << "Total Stock Value"     << ": RM " << totalStockValue << "\n";
    cout << " " << left << setw(22) << "Average Product Price" << ": RM " << averagePrice << "\n";
    cout << " " << left << setw(22) << "Low Stock Items"       << ": " << lowStockCount << " (threshold <= 10)\n";

    cout << "\n--- Products At or Below Threshold (10 units) ---\n";

    if (lowStockCount == 0) {
        cout << " [System] No products are currently low on stock. Great job!\n";
    } else {
        cout << left
             << setw(6)  << "ID"
             << setw(20) << "Name"
             << setw(15) << "Category"
             << setw(10) << "Zone"
             << setw(8)  << "Qty"
             << right << setw(12) << "Price (RM)"
             << "\n";
        cout << string(71, '-') << "\n";

        temp = inventory.getHead();
        while (temp != NULL) {
            if (temp->stockQuantity <= 10) {
                cout << left
                     << setw(6)  << temp->productId
                     << setw(20) << temp->productName
                     << setw(15) << temp->category
                     << setw(10) << temp->zone
                     << setw(8)  << temp->stockQuantity
                     << right << setw(12) << temp->productPrice
                     << "\n";
            }
            temp = temp->next;
        }
        cout << string(71, '-') << "\n";
    }

    ofstream outFile("data/LowStockReport.txt", ios::out | ios::trunc);
    if (!outFile) {
       cout << "Cannot open data/LowStockReport.txt for writing.\n";
       return ;
    }

    outFile << "===== LOW STOCK REPORT =====\n";
    outFile << "Low Stock Threshold: <= 10 units\n\n";

    outFile << "--- Overall Inventory Statistics ---\n";
    outFile << fixed << setprecision(2);
    outFile << "Total Products: " << totalProducts << "\n";
    outFile << "Total Stock Units: " << totalStockUnits << "\n";
    outFile << "Total Stock Value: RM " << totalStockValue << "\n";
    outFile << "Average Product Price: RM " << averagePrice << "\n";
    outFile << "Low Stock Items: " << lowStockCount << "\n\n";

    outFile << "--- Low Stock Product List ---\n";
    if (lowStockCount == 0) {
        outFile << "No products are currently low on stock.\n";
    } else {
        temp = inventory.getHead();
        while (temp != NULL) {
            if (temp->stockQuantity <= 10) {
                outFile << temp->productId   << "|"
                        << temp->productName  << "|"
                        << temp->category     << "|"
                        << temp->zone          << "|"
                        << temp->stockQuantity << "|"
                        << temp->productPrice  << "\n";
            }
            temp = temp->next;
        }
    }
    outFile.close();
    cout << "\n [System] Report successfully saved to data/LowStockReport.txt\n";
}

// SuperAdmin Implementation
SuperAdmin::SuperAdmin(string un, string pw) : User(un, pw, "SuperAdmin") { 
    next = NULL; 
}
SuperAdmin::~SuperAdmin() {}

void SuperAdmin::displayMenu() {
    int choice;
    bool inMenu = true;
    while (inMenu) {
        cout<<"\n==============================\n";
        cout<<"\n     SuperAdmin Dashboard     \n";
        cout<<"\n==============================\n";
        cout << "1. View Inventory\n";
        cout << "2. Sort Inventory \n";
        cout << "3. Search Product \n";
        cout << "4. Add Product\n";
        cout << "5. Update Stock\n";
        cout << "6. Delete Product \n";
        cout << "7. Add Admin \n";
        cout << "8. Generate Low Stock Report\n";
        cout << "9. View Stock Summary\n";
        cout << "10. Customer View Mode\n";
        cout << "11. View Customer Orders\n";
        cout << "12. Logout\n";
        cout << "\n==================================\n";
        cout << "Enter a function: ";
        cin >> choice;

        if (cin.fail()) { cin.clear(); cin.ignore(1000, '\n'); continue; }

        if (choice == 1) viewInventoryWithSortOption(globalInventory);
        else if (choice == 2) sortAndDisplay(globalInventory);
        else if (choice == 3) searchProduct(globalInventory);
        else if (choice == 4) addProduct(globalInventory);
        else if (choice == 5) updateStock(globalInventory);
        else if (choice == 6) deleteProduct(globalInventory);
        else if (choice == 7) addAdmin();
        else if (choice == 8) generateLowStockReport(globalInventory);
        else if (choice == 9) generateStockSummary(globalInventory);
        else if (choice == 10) displayInventoryForCustomer(globalInventory);
        else if (choice == 11) {
            cout << "\nEnter Customer Username to view orders: ";
            string targetUsername;
            cin >> targetUsername;

            User* temp = userHead;
            bool found = false;

            while (temp != NULL) {
                if (temp->role == "Customer" && temp->username == targetUsername) {
                    Customer* cust = dynamic_cast<Customer*>(temp);
                    if (cust != NULL) {
                        adminViewCustomerOrders(*cust);
                        found = true;
                    }
                    break;
                }
                temp = temp->next;
            }

            if (!found) {
                cout << "[Error] Customer '" << targetUsername << "' not found.\n";
            }
        }
        else if (choice == 12) inMenu = false;
        else cout << "[Error] Invalid option." << endl;
    }
}

void SuperAdmin::addProduct(ProductLinkedList& inventory) { Admin temp(username, password); temp.addProduct(inventory); }
void SuperAdmin::updateStock(ProductLinkedList& inventory) { Admin temp(username, password); temp.updateStock(inventory); }
void SuperAdmin::sortAndDisplay(ProductLinkedList& inventory) { Admin temp(username, password); temp.sortAndDisplay(inventory); }
void SuperAdmin::searchProduct(ProductLinkedList& inventory) { Admin temp(username, password); temp.searchProduct(inventory); }
void SuperAdmin::generateLowStockReport(ProductLinkedList& inventory) { Admin temp(username, password); temp.generateLowStockReport(inventory); }

void SuperAdmin::deleteProduct(ProductLinkedList& inventory) {
    cout << "\n=========================================\n";
    cout << "          --- Delete Product ---           \n";
    cout << "=========================================\n";
    
    cin.clear();
    int id = getValidIntData("Enter Product ID to Delete: ");

    Product* p = inventory.binarySearch(id);
    
    if (p != NULL) {
        cout << "\n=========================================\n";
        cout << " [CRITICAL WARNING] Record Targeted for Deletion\n";
        cout << "-----------------------------------------\n";
        cout << " " << left << setw(16) << "Product ID"     << ": " << p->productId << "\n";
        cout << " " << left << setw(16) << "Product Name"   << ": " << p->productName << "\n";
        cout << " " << left << setw(16) << "Category"       << ": " << p->category << "\n";
        cout << " " << left << setw(16) << "Quantity"       << ": " << p->stockQuantity << " units\n";
        cout << " " << left << setw(16) << "Warehouse Zone" << ": " << p->zone << "\n";
        cout << " " << left << setw(16) << "Supplier"       << ": " << p->supplier << "\n";
        cout << " " << left << setw(16) << "Price"          << ": RM " << fixed << setprecision(2) << p->productPrice << "\n";
        cout << "=========================================\n";
        cout << " WARNING: This operation is irreversible!\n";
        
        cout << " Are you absolutely sure you want to purge this record? (Y/N): ";
        char confirm;
        cin >> confirm;
        clearInputBuffer(); 

        if (confirm == 'Y' || confirm == 'y') {
            inventory.deleteNode(id);
            cout << "\n [Success] Product record safely and permanently removed from memory.\n";
            saveInventoryToFile(inventory);
            cout << " [System] Local flat-file database successfully updated.\n";
        } else {
            cout << "\n [Cancelled] Deletion process aborted. Data remains intact.\n";
        }
    } else {
        cout << "\n [Error] Target Product ID " << id << " was not found in the inventory system.\n";
    }
}


void SuperAdmin::addAdmin() {
    string un, pw; 
    cout << "\n=========================================\n";
    cout << "       --- Register New Admin ---        \n";
    cout << "=========================================\n";
    cout <<"Enter 0 anytime for cancel registration.\n"<<endl;

    clearInputBuffer();

    while (true) {
        cout << "Enter New Username: ";
        cin >> un;
        
        if (un == "0")	{
        	cout << "[System] Customer registration cancelled." << endl;
			return;
		}

        if (!isValidAdminname(un)) {
            cout << " [Error] Username must be 3-20 characters long and contain only letters, numbers, or '_'.\n\n";
            clearInputBuffer();
            continue;
        }

        User* temp = userHead;
        bool isDuplicate = false;
        while (temp != NULL) {
            if (temp->username == un) {
                isDuplicate = true;
                break;
            }
            temp = temp->next;
        }

        if (isDuplicate) {
            cout << " [Error] Username '" << un << "' already exists in the system database!\n\n";
            clearInputBuffer();
            continue;
        }

        clearInputBuffer();
        break;
    }

    while (true) {
        cout << "Enter New Password: ";
        cin >> pw;
        
        if (pw == "0")	{
        	cout << "[System] Customer registration cancelled." << endl;
			return;
		}

        if (!isValidAdminPassword(pw)) {
            cout << " [Error] Password must be 4-20 characters long and cannot contain the illegal character '|'.\n\n";
            clearInputBuffer();
            continue;
        }

        clearInputBuffer();
        break;
    }

    cout << "\n=========================================\n";
    cout << "         [Preview] Account Details       \n";
    cout << "-----------------------------------------\n";
    cout << " " << left << setw(16) << "Role"     << ": Admin\n";
    cout << " " << left << setw(16) << "Username" << ": " << un << "\n";
    cout << " " << left << setw(16) << "Password" << ": " << pw << "\n";
    cout << "=========================================\n";
    
    cout << "Confirm account provisioning? (Y/N): ";
    char confirm;
    cin >> confirm;
    clearInputBuffer();

    if (confirm == 'Y' || confirm == 'y') {
        User* n = new Admin(un, pw);
        if (userHead == NULL) {
            userHead = n;
        } else {
            User* temp = userHead;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = n;
        }
        cout << "\n [Success] New Admin account generated and saved onto the linked structure.\n";
    } else {
        cout << "\n [Cancelled] Registration sequence stopped. No credentials saved.\n";
    }
}

void displayInventoryForCustomer(const ProductLinkedList& inv) {
    cout << "\n===== INVENTORY (Customer View) =====\n";
    cout << left << setw(6) << "ID" << setw(22) << "Name"
         << setw(15) << "Category" << right << setw(10) << "Price(RM)\n";
    cout << string(53, '-') << "\n";
    
    Product* temp = inv.head;
    if (temp == NULL) {
        cout << " [Info] No products available.\n";
        return;
    }
    while (temp != NULL) {
        cout << left  << setw(6)  << temp->productId
             << setw(22) << temp->productName
             << setw(15) << temp->category
             << right << setw(10) << fixed << setprecision(2) << temp->productPrice << "\n";
        temp = temp->next;
    }
    cout << string(53, '-') << "\n";
    cout << " Total Products: " << inv.count << "\n";
}

void generateStockSummary(const ProductLinkedList& inv) {
    cout << "\n===== STOCK SUMMARY =====\n";
    
    if (inv.count == 0) {
        cout << " [Info] Inventory is empty.\n";
        return;
    }
    
    int totalQty = 0;
    double totalValue = 0.0;
    int lowStockCount = 0;
    
    Product* temp = inv.head;
    while (temp != NULL) {
        totalQty    += temp->stockQuantity;
        totalValue  += temp->stockQuantity * temp->productPrice;
        if (temp->stockQuantity <= 10) lowStockCount++;
        temp = temp->next;
    }
    
    cout << " Total Products    : " << inv.count << "\n";
    cout << " Total Units       : " << totalQty << "\n";
    cout << " Total Value       : RM " << fixed << setprecision(2) << totalValue << "\n";
    cout << " Low Stock Items   : " << lowStockCount << "\n";
    cout << "=========================\n";
}

void adminViewCustomerOrders(const Customer& cust) {
    cout << "\n===== ORDERS FOR: " << cust.username << " =====\n";
    
    Order* current = cust.head;
    if (current == NULL) {
        cout << " [Info] This customer has no orders.\n";
        return;
    }
    
    int count = 1;
    while (current != NULL) {
        cout << " Order #" << count << "\n";
        cout << "   Order ID  : " << current->orderId << "\n";
        cout << "   Product ID: " << current->productId << "\n";
        cout << "   Quantity  : " << current->dispatchQuantity << "\n";
        cout << "   Date      : " << current->orderDate << "\n";
        cout << " -----------------------\n";
        current = current->next;
        count++;
    }
}

void printUserProfile(const User& u) {
    cout << "=============================\n";
    cout << " Username : " << u.username << "\n";
    cout << " Role     : " << u.role     << "\n";
    cout << " Password : " << string(u.password.length(), '*') << "\n";
    cout << "=============================\n";
}

// Customer
Customer::Customer(string username , string password) : User(username, password, "Customer"){
	head = NULL;
	tail = NULL;
	loadOrdersFromFile(head, tail, username);
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
    
    appendOrderToFile(newOrder, username);
    saveOrderSummaryFile(head, username);

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
		Product* product = findProductById(current->productId);
		
		cout << "  Record #" << recordcount << "\n";
		cout << "  Customer Name: " << username << "\n";
        cout << "  Order ID     : " << current->orderId << "\n";
        cout << "  Product ID   : " << current->productId << "\n";
        
        if(product != NULL){
            cout << "  Product Name : " << product->productName << "\n";
            cout << "  Category     : " << product->category << "\n";
            delete product;
        } else {
            cout << "  Product Name : Unknown Product\n";
            cout << "  Category     : Unknown Category\n";
        }
        
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
        
        Product* product = findProductById(found->productId);
		if (product != NULL) {
            cout << "Product Name: " << product->productName << "\n";
            cout << "Category    : " << product->category << "\n";
            delete product;
        } else {
            cout << "Product Name: Unknown Product\n";
            cout << "Category    : Unknown Category\n";
        }
	
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
                saveOrderSummaryFile(head, username);
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
	saveOrderSummaryFile(head, username);
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

    saveOrderSummaryFile(head, username);
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
        cout <<"9. View My Profile\n";
		cout<<"10. Logout\n";
		cout<<"\n==============================\n";
		cout<<"Enter your Choice (1-10): ";
		
		try{
			cin>>choice;
			if(cin.fail()){
				throw string("Invalid input detected! Please enter the valid number!");
			}
            if (choice < 1 || choice>10){
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
                    cout << "\n[SYSTEM] Viewing My Profile...\n";
                    cout << "---------------------------------\n";
                    cout << "Username : " << username << "\n";
                    cout << "Role     : " << role << "\n";
                    cout << "---------------------------------\n";
                    break;
                    
                case 10:
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
            cout<<"\n[ERROR]"<<invalidChoice<<"is OUT of Range! Please Enter 1-10...\n";
        }                                                                   
    }                                                                 
}

// ==========================================
// 7. System Entry Point
// ==========================================
int main() {
    cout <<"==========================================="<<endl;
    cout <<"        Warehouse Inventory System         "<<endl;
    cout <<"==========================================="<<endl;
    
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

    saveInventoryToFile(globalInventory);
    saveAdminsToFile();
    saveCustomersToFile();
    clearMemory();
    
    cout << "System Terminated." << endl;
    return 0;
}

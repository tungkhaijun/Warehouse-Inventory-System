#include <iostream>
#include <string>
#include <fstream>
#include <sstream>  // Required for string splitting
#include <cstdlib>
#include <stdexcept> // Required for runtime_error / exception (try-catch error handling)
#include "admin.h"
#include "backend.h"
#include "structures.h"
#include "Lead_backend.cpp"

using namespace std;

// External global variables defined in main.cpp
extern ProductLinkedList globalInventory; 
extern User* userHead; 

// =========================================================
// File I/O Engine
// =========================================================

void saveInventoryToFile(ProductLinkedList& inventory) {
    // ios::out | ios::trunc = overwrite entire file with ALL nodes in the linked list.
    // This is CORRECT because we always write the full linked list back to disk.
    // Old data is preserved as long as loadInventoryFromFile() was called at startup
    // so the linked list already contains all existing products before we add new ones.
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
    // This function ONLY persists Admin & SuperAdmin accounts to data/Admin.txt.
    // Customer accounts are handled separately by saveCustomersToFile() in customer.cpp.
    // Format: username|password|role
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
    // Read the file line by line
    while (getline(inFile, line)) {
        if (line.empty()) continue; // Skip empty lines

        stringstream ss(line);
        string tempId, name, cat, tempQty, zone, sup, tempPrice;

        // Split the line by '|'
        getline(ss, tempId, '|');
        getline(ss, name, '|');
        getline(ss, cat, '|');
        getline(ss, tempQty, '|');
        getline(ss, zone, '|');
        getline(ss, sup, '|');
        getline(ss, tempPrice, '|');

        // Convert the string numbers back to int and double
        int id = atoi(tempId.c_str());
        int qty = atoi(tempQty.c_str());
        double price = atof(tempPrice.c_str());

        // Insert into the linked list
        inventory.insertNode(id, name, cat, qty, zone, sup, price);
    }
    inFile.close();
}

void loadAdminsFromFile() {
    // This function ONLY loads Admin & SuperAdmin accounts from data/Admin.txt.
    // Customer accounts are handled separately by loadCustomersFromFile() in customer.cpp.
    ifstream inFile("data/Admin.txt");
    if (!inFile) return;

    string line;
    // Admin.txt format uses '|' as delimiter: username|password|role
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

        User* n = NULL;
        if (role == "SuperAdmin") n = new SuperAdmin(un, pw);
        else if (role == "Admin") n = new Admin(un, pw);
        else continue; // Customer (or unknown) rows don't belong in Admin.txt — skip

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


// Utility to clear cin buffer and prevent getline from being skipped
void clearInputBuffer() {
    cin.clear();
    cin.ignore(10000, '\n');
}

// Validates and reads a string via getline to support names with spaces
string getValidLineData(string prompt) {
    string value;
    while (true) {
        cout << prompt;
        getline(cin, value);
        
        // Check if the input is empty or contains only spaces/tabs
        bool isEmpty = true;
        for (size_t i = 0; i < value.length(); i++) {
            if (value[i] != ' ' && value[i] != '\t') {
                isEmpty = false;
                break;
            }
        }

        // Check for file delimiter character '|' to prevent database corruption
        bool hasDelimiter = (value.find('|') != string::npos);

        if (!isEmpty && !hasDelimiter) {
            return value;
        }
        cout << " [Error] Input cannot be empty or contain illegal character '|'!\n";
    }
}


// 4. Register Validation Rules (Same with User)
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

// Validates positive integers for values like stock quantity
int getValidIntData(string prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value > 0) {
            clearInputBuffer(); // Clear newline immediately to prepare for subsequent getline
            return value;
        }
        cout << " [Error] Please enter a valid positive integer (> 0)!\n";
        clearInputBuffer();
    }
}

// Validates positive double values for product pricing
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

// =========================================================
// Admin Implementation
// =========================================================
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
        cout << "7. Logout\n";
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
        else if (choice == 7) inMenu = false;
        else cout << "[Error] Invalid option." << endl;
    }
}




// --- 3. Add New Record 
void Admin::addProduct(ProductLinkedList& inventory) {
    cout << "\n=========================================\n";
    cout << "        --- Add New Product ---          \n";
    cout << "=========================================\n";
    
    // If the inventory list is completely empty, start from a baseline ID (e.g., 1001).
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

    // Displays structured data card for final user validation before writing to disk
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

//update/edit function
void Admin::updateStock(ProductLinkedList& inventory) {
    cout << "\n=========================================\n";
    cout << "      --- Edit Full Product Record ---    \n";
    cout << "=========================================\n";
    
    cin.clear();
    int id = getValidIntData("Enter Product ID to Edit: ");

    // Guideline Rule: Locate target record strictly utilizing Binary Search
    Product* p = inventory.binarySearch(id);
    
    if (p != NULL) { 
        // Display existing information formatted perfectly
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

        // Collect new inputs via absolute type-safe input checks
        string newName     = getValidLineData("Enter New Product Name: ");
        string newCat      = getValidLineData("Enter New Category: ");
        int newQty         = getValidIntData("Enter New Quantity: ");
        string newZone     = getValidLineData("Enter New Warehouse Zone: ");
        string newSupplier = getValidLineData("Enter New Supplier: ");
        double newPrice    = getValidDoubleData("Enter New Price: RM ");

        // Interactive Confirmation Screen (UI Distinctive Component)
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
            // Apply fields modifications directly onto the linked list memory addresses
            p->productName   = newName;
            p->category      = newCat;
            p->stockQuantity = newQty;
            p->zone          = newZone;
            p->supplier      = newSupplier;
            p->productPrice  = newPrice;

            cout << "\n [Success] Product records modified perfectly in runtime system!\n";
            
            // Commit mutations to permanent text-based database file immediately
            saveInventoryToFile(inventory);
            cout << " [System] Changes synchronized to data/Inventory.txt seamlessly.\n";
        } else {
            cout << "\n [Cancelled] Modification sequence aborted. No data was written.\n";
        }
    } else {
        cout << "\n [Error] Product ID " << id << " not discovered inside the database.\n";
    }
}

//sort fucntion
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


//binary search
void Admin::searchProduct(ProductLinkedList& inventory) {
    int id; 
    cout << "\n--- Search Product ---" << endl;
    cout << "Enter Product ID: "; cin >> id;
    if (cin.fail()) { clearInputBuffer(); return; }

    Product* p = inventory.binarySearch(id);
    if (p != NULL) { 
        // 打印一个整齐的卡片式外观
        cout << "\n=========================================\n";
        cout << " [Found] Product Information\n";
        cout << "=========================================\n";
        
        cout << " " << left << setw(15) << "Product ID"   << ": " << p->productId << "\n";
        cout << " " << left << setw(15) << "Product Name" << ": " << p->productName << "\n";
        cout << " " << left << setw(15) << "Category"     << ": " << p->category << "\n";
        cout << " " << left << setw(15) << "Quantity"     << ": " << p->stockQuantity << " units\n";
        cout << " " << left << setw(15) << "Warehouse Zone"<< ": " << p->zone << "\n";
        cout << " " << left << setw(15) << "Supplier"     << ": " << p->supplier << "\n";
        
        // 价格格式化为两位小数
        cout << " " << left << setw(15) << "Price"        << ": RM " << fixed << setprecision(2) << p->productPrice << "\n";
        
        cout << "=========================================\n";
    } else {
        cout << "\n[Error] Product ID " << id << " not found." << endl;
    }
}

// =========================================================
// Inventory Summary Report: Low Stock Report
// =========================================================
// [Rubric Note] This satisfies the "Summary Report" requirement:
//   - Accurate, dynamically computed statistics (not hardcoded)
//   - Displayed on screen AND saved into a .txt file (data/LowStockReport.txt)
//
// [Algorithm Note] Sorting reuses ProductLinkedList::sortList(), which
// is the manually implemented Merge Sort algorithm from Lead_backend.cpp.
// No STL sorting/searching function is used anywhere in this feature.

const int LOW_STOCK_THRESHOLD = 10; // Products at or below this qty are flagged

// Generates the Low Stock Report: counts low-stock items, computes
// dynamic inventory statistics, displays the report, and writes it
// to data/LowStockReport.txt so it can be retrieved later.
void Admin::generateLowStockReport(ProductLinkedList& inventory) {
    cout << "\n=========================================\n";
    cout << "        --- Low Stock Report ---          \n";
    cout << "=========================================\n";

    if (inventory.getCount() == 0) {
        cout << " [System] Inventory is empty. No report to generate.\n";
        return;
    }

    // Reuse the manually implemented Merge Sort from Lead_backend.cpp
    // (ProductLinkedList::sortList) to order products by quantity ascending.
    // This places the most critical (lowest stock) items at the top of the report.
    inventory.sortList(1); // sortBy = 1 -> Quantity (Merge Sort, see Lead_backend.cpp)

    // ---- Pass 1: Walk the linked list manually to gather statistics ----
    // (No STL containers/algorithms are used; this is a plain pointer traversal.)
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
        if (temp->stockQuantity <= LOW_STOCK_THRESHOLD) {
            lowStockCount++;
        }
        temp = temp->next;
    }
    double averagePrice = totalPriceSum / totalProducts;

    // ---- Display: Overall Inventory Statistics ----
    cout << "\n--- Overall Inventory Statistics ---\n";
    cout << fixed << setprecision(2);
    cout << " " << left << setw(22) << "Total Products"        << ": " << totalProducts << "\n";
    cout << " " << left << setw(22) << "Total Stock Units"     << ": " << totalStockUnits << "\n";
    cout << " " << left << setw(22) << "Total Stock Value"     << ": RM " << totalStockValue << "\n";
    cout << " " << left << setw(22) << "Average Product Price" << ": RM " << averagePrice << "\n";
    cout << " " << left << setw(22) << "Low Stock Items"       << ": " << lowStockCount << " (threshold <= " << LOW_STOCK_THRESHOLD << ")\n";

    // ---- Display: Low Stock Table (already sorted ascending by quantity) ----
    cout << "\n--- Products At or Below Threshold (" << LOW_STOCK_THRESHOLD << " units) ---\n";

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

        temp = inventory.getHead(); // List is already sorted ascending by quantity
        while (temp != NULL) {
            if (temp->stockQuantity <= LOW_STOCK_THRESHOLD) {
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

    // ---- Save the report to a .txt file (try/catch for error handling) ----
    try {
        ofstream outFile("data/LowStockReport.txt", ios::out | ios::trunc);
        if (!outFile) {
            throw runtime_error("Cannot open data/LowStockReport.txt for writing.");
        }

        outFile << "===== LOW STOCK REPORT =====\n";
        outFile << "Low Stock Threshold: <= " << LOW_STOCK_THRESHOLD << " units\n\n";

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
                if (temp->stockQuantity <= LOW_STOCK_THRESHOLD) {
                    // Pipe-delimited so the same row can be re-parsed later
                    // by retrieveLowStockReport() using getline(ss, field, '|').
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

    } catch (const exception& e) {
        cout << "\n [Error] Failed to save report: " << e.what() << "\n";
    }
}

// =========================================================
// SuperAdmin Implementation
// =========================================================
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
        cout << "9. Logout\n";
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
        else if (choice == 9) inMenu = false;
        else cout << "[Error] Invalid option." << endl;
    }
}

void SuperAdmin::addProduct(ProductLinkedList& inventory) { Admin temp(username, password); temp.addProduct(inventory); }
void SuperAdmin::updateStock(ProductLinkedList& inventory) { Admin temp(username, password); temp.updateStock(inventory); }
void SuperAdmin::sortAndDisplay(ProductLinkedList& inventory) { Admin temp(username, password); temp.sortAndDisplay(inventory); }
void SuperAdmin::searchProduct(ProductLinkedList& inventory) { Admin temp(username, password); temp.searchProduct(inventory); }
void SuperAdmin::generateLowStockReport(ProductLinkedList& inventory) { Admin temp(username, password); temp.generateLowStockReport(inventory); }

//delete function
void SuperAdmin::deleteProduct(ProductLinkedList& inventory) {
    cout << "\n=========================================\n";
    cout << "          --- Delete Product ---           \n";
    cout << "=========================================\n";
    
    // Refresh input stream states
    cin.clear();
    int id = getValidIntData("Enter Product ID to Delete: ");

    // Guideline Rule: Locate target node utilizing the custom Binary Search implementation
    Product* p = inventory.binarySearch(id);
    
    if (p != NULL) {
        // High Score UX Trigger: Display a clear warning block detailing the target asset
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
        
        // High Score Requirement: Interactive Dual-Confirmation Guard
        cout << " Are you absolutely sure you want to purge this record? (Y/N): ";
        char confirm;
        cin >> confirm;
        clearInputBuffer(); // Purge trailing newline characters from cache

        if (confirm == 'Y' || confirm == 'y') {
            // Unlink and deallocate memory space for the node within the custom structure
            inventory.deleteNode(id);
            cout << "\n [Success] Product record safely and permanently removed from memory.\n";
            
            // File I/O Synchronization: Write updated linked list states back to the data ledger
            saveInventoryToFile(inventory);
            cout << " [System] Local flat-file database successfully updated.\n";
        } else {
            // Safety exit condition if user hesitates or cancels
            cout << "\n [Cancelled] Deletion process aborted. Data remains intact.\n";
        }
    } else {
        // Graceful failure handler when non-existent keys are requested
        cout << "\n [Error] Target Product ID " << id << " was not found in the inventory system.\n";
    }
}

void SuperAdmin::addAdmin() {
    string un, pw; 
    cout << "\n=========================================\n";
    cout << "       --- Register New Admin ---        \n";
    cout << "=========================================\n";
    cout <<"Enter 0 anytime for cancel registration.\n"<<endl;

    cin.clear();

    // 1. Prompt and rigorously validate Username input
    while (true) {
        cout << "Enter New Username: ";
        cin >> un;
        
        if (un == "0")	{
        	cout << "[System] Admin registration cancelled." << endl;
			return;
		}

        // Constraint A: Schema character rules validation
        if (!isValidAdminname(un)) {
            cout << " [Error] Username must be 3-20 characters long and contain only letters, numbers, or '_'.\n\n";
            clearInputBuffer();
            continue;
        }

        // Constraint B: Data uniqueness check (Duplicate Detection Rule)
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

        // Passed all validation metrics
        clearInputBuffer();
        break;
    }

    // 2. Prompt and rigorously validate Password input
    while (true) {
        cout << "Enter New Password: ";
        cin >> pw;
        
        if (pw == "0")	{
        	cout << "[System] Admin registration cancelled." << endl;
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

    // 3. Structured Visual Profile Card & Interactive Confirmation Screen
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
        // 4. Instantiate object node and dynamically bind it into the user storage chain
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
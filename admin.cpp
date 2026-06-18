#include <iostream>
#include <string>
#include <fstream>
#include <sstream>  // Required for string splitting
#include <cstdlib>
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
    // Make sure the path matches your main.cpp design (e.g., "data/Inventory.txt" or just "Inventory.txt")
    // I am using "Inventory.txt" to match your uploaded text file location.
    ofstream outFile("Inventory.txt"); 
    if (!outFile) {
        cout << "[System Warning] Cannot write to Inventory.txt!" << endl;
        return;
    }
    Product* temp = inventory.getHead();
    while (temp != NULL) {
        // Changed from spaces " " to pipes "|" to match your format
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
    ofstream outFile("data/Admin.txt");
    if (!outFile) {
        cout << "[System Warning] Cannot write to data/Admin.txt! Ensure 'data' folder exists." << endl;
        return;
    }
    User* temp = userHead;
    while (temp != NULL) {
        outFile << temp->username << " " 
                << temp->password << " " 
                << temp->role << "\n";
        temp = temp->next;
    }
    outFile.close();
}

void loadInventoryFromFile(ProductLinkedList& inventory) {
    ifstream inFile("Inventory.txt");
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
    ifstream inFile("data/Admin.txt");
    if (!inFile) return; 

    string un, pw, role;
    while (inFile >> un >> pw >> role) {
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
        cout << "\n=== Admin Control Panel | User: " << username << " ===" << endl;
        cout << "1. View Inventory\n";
        cout << "2. Sort Inventory (Merge Sort)\n";
        cout << "3. Search Product (Binary Search)\n";
        cout << "4. Add Product\n";
        cout << "5. Update Stock\n";
        cout << "6. Logout\n";
        cout << "Choice: ";
        cin >> choice;

        if (cin.fail()) { 
            cin.clear(); cin.ignore(1000, '\n'); 
            cout << "[Error] Please enter a valid number.\n";
            continue; 
        }

        if (choice == 1) globalInventory.display();
        else if (choice == 2) sortAndDisplay(globalInventory);
        else if (choice == 3) searchProduct(globalInventory);
        else if (choice == 4) addProduct(globalInventory);
        else if (choice == 5) updateStock(globalInventory);
        else if (choice == 6) inMenu = false;
        else cout << "[Error] Invalid option." << endl;
    }
}

void Admin::addProduct(ProductLinkedList& inventory) {
    int id, qty; string name, cat, zone, supplier; double price;
    
    cout << "\n--- Add Product ---" << endl;
    cout << "ID: "; cin >> id;
    if (cin.fail()) { cin.clear(); cin.ignore(1000, '\n'); return; }
    
    cout << "Name: "; cin >> name;
    cout << "Category: "; cin >> cat;
    cout << "Quantity: "; cin >> qty;
    cout << "Zone: "; cin >> zone;
    cout << "Supplier: "; cin >> supplier;
    cout << "Price: RM "; cin >> price;

    inventory.insertNode(id, name, cat, qty, zone, supplier, price);
    cout << "[Success] Product added to linked list!" << endl;
    
    saveInventoryToFile(inventory);
    cout << "[System] data/Inventory.txt auto-saved." << endl;
}

void Admin::updateStock(ProductLinkedList& inventory) {
    int id, newQty;
    cout << "\n--- Update Stock ---" << endl;
    cout << "Enter Product ID: "; cin >> id;
    if (cin.fail()) { cin.clear(); cin.ignore(1000, '\n'); return; }

    Product* p = inventory.binarySearch(id);
    if (p != NULL) { 
        cout << "[Found] " << p->productName << " | Current Qty: " << p->stockQuantity << endl;
        cout << "Enter New Qty: "; cin >> newQty; 
        if (cin.fail()) { cin.clear(); cin.ignore(1000, '\n'); return; }
        
        p->stockQuantity = newQty; 
        cout << "[Success] Stock updated!" << endl;
        
        saveInventoryToFile(inventory);
        cout << "[System] data/Inventory.txt auto-saved." << endl;
    } else {
        cout << "[Error] Product not found." << endl;
    }
}

void Admin::sortAndDisplay(ProductLinkedList& inventory) {
    int s; 
    cout << "\n--- Sort Inventory ---" << endl;
    cout << "Sort by (1:Qty, 2:Price): "; cin >> s;
    if (cin.fail()) { cin.clear(); cin.ignore(1000, '\n'); return; }
    
    if (s == 1 || s == 2) {
        inventory.sortList(s); 
        inventory.display();
    } else {
        cout << "[Error] Invalid sorting option." << endl;
    }
}

void Admin::searchProduct(ProductLinkedList& inventory) {
    int id; 
    cout << "\n--- Search Product ---" << endl;
    cout << "Enter Product ID: "; cin >> id;
    if (cin.fail()) { cin.clear(); cin.ignore(1000, '\n'); return; }

    Product* p = inventory.binarySearch(id);
    if (p != NULL) { 
        cout << "\n[Found] ID: " << p->productId << " | Name: " << p->productName << endl;
        cout << "Category: " << p->category << " | Qty: " << p->stockQuantity << endl;
        cout << "Zone: " << p->zone << " | Supplier: " << p->supplier << endl;
        cout << "Price: RM " << p->productPrice << endl;
    } else {
        cout << "[Error] Product ID " << id << " not found." << endl;
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
        cout << "\n=== 👑 SuperAdmin Panel | User: " << username << " ===" << endl;
        cout << "1. View Inventory\n2. Sort Inventory\n3. Search Product\n4. Add Product\n5. Update Stock\n";
        cout << "6. Delete Product [High Privilege]\n7. Add Admin [High Privilege]\n8. Logout\nChoice: ";
        cin >> choice;

        if (cin.fail()) { cin.clear(); cin.ignore(1000, '\n'); continue; }

        if (choice == 1) globalInventory.display();
        else if (choice == 2) sortAndDisplay(globalInventory);
        else if (choice == 3) searchProduct(globalInventory);
        else if (choice == 4) addProduct(globalInventory);
        else if (choice == 5) updateStock(globalInventory);
        else if (choice == 6) deleteProduct(globalInventory);
        else if (choice == 7) addAdmin();
        else if (choice == 8) inMenu = false;
        else cout << "[Error] Invalid option." << endl;
    }
}

void SuperAdmin::addProduct(ProductLinkedList& inventory) { Admin temp(username, password); temp.addProduct(inventory); }
void SuperAdmin::updateStock(ProductLinkedList& inventory) { Admin temp(username, password); temp.updateStock(inventory); }
void SuperAdmin::sortAndDisplay(ProductLinkedList& inventory) { Admin temp(username, password); temp.sortAndDisplay(inventory); }
void SuperAdmin::searchProduct(ProductLinkedList& inventory) { Admin temp(username, password); temp.searchProduct(inventory); }

void SuperAdmin::deleteProduct(ProductLinkedList& inventory) {
    int id; 
    cout << "\n--- Delete Product ---" << endl;
    cout << "Enter Product ID to delete: "; cin >> id;
    if (cin.fail()) { cin.clear(); cin.ignore(1000, '\n'); return; }

    Product* p = inventory.binarySearch(id);
    if (p != NULL) {
        inventory.deleteNode(id);
        cout << "[Success] Product deleted." << endl;
        
        saveInventoryToFile(inventory);
        cout << "[System] data/Inventory.txt auto-saved." << endl;
    } else {
        cout << "[Error] Product ID not found." << endl;
    }
}

void SuperAdmin::addAdmin() {
    string un, pw; 
    cout << "\n--- Register New Admin ---" << endl;
    cout << "New Username: "; cin >> un; 
    cout << "New Password: "; cin >> pw;

    User* temp = userHead;
    while (temp != NULL) {
        if (temp->username == un) {
            cout << "[Error] Username already exists!" << endl;
            return;
        }
        temp = temp->next;
    }

    User* n = new Admin(un, pw);
    if (userHead == NULL) {
        userHead = n;
    } else {
        temp = userHead;
        while(temp->next != NULL) temp = temp->next;
        temp->next = n;
    }
    cout << "[Success] New admin added to system." << endl;
    
    saveAdminsToFile();
    cout << "[System] data/Admin.txt auto-saved." << endl;
}
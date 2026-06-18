#include <iostream>
#include <string>
//#include "structures.h"
//#include "backend.h"
//#include "admin.h"
#include "admin.cpp"

using namespace std;

// ========================================================
// 1. Declare global variables
// ========================================================
ProductLinkedList globalInventory; // Global inventory linked list
User* userHead = NULL;             // Global user linked list


int main() {
    // ========================================================
    // 2. Load data from files (Replaces built-in test data)
    // ========================================================
    cout << "[System] Initializing Test Environment..." << endl;
    
    // 调用 admin.cpp 中写好的加载功能，直接从 txt 文件中读取真实数据
    loadInventoryFromFile(globalInventory);
    loadAdminsFromFile();
    
    cout << "[System] Data loaded from text files (if any exist).\\n" << endl;

    // ========================================================
    // 3. Simulate system login gateway
    // ========================================================
    int choice;
    bool isRunning = true;

    while (isRunning) {
        cout << "=========================================" << endl;
        cout << "   Warehouse System - TEST ENVIRONMENT" << endl;
        cout << "=========================================" << endl;
        cout << "1. Login as Admin" << endl;
        cout << "2. Login as SuperAdmin" << endl;
        cout << "3. Exit Test" << endl;
        cout << "Select Login Role: ";
        
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "[Error] Invalid input. Please enter a number.\n";
            continue;
        }

        User* currentUser = NULL;

        if (choice == 1) {
            currentUser = new Admin("TestAdmin_01", "123456");
        } 
        else if (choice == 2) {
            currentUser = new SuperAdmin("TestBoss_99", "admin123");
        } 
        else if (choice == 3) {
            cout << "Exiting test environment..." << endl;
            isRunning = false;
        } 
        else {
            cout << "[Error] Invalid choice. Please try again." << endl;
        }

        // ========================================================
        // 4. Trigger polymorphic menu & release memory
        // ========================================================
        if (currentUser != NULL) {
            currentUser->displayMenu(); 
            
            delete currentUser; 
            currentUser = NULL;
        }
    }

    cout << "[System] Terminated gracefully." << endl;
    return 0;
}
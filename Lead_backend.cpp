#include <iostream>
#include <string>
#include "backend.h"
#include "structures.h"
using namespace std;

// ==========================================
// 1. dynamic non-primitive data structure: user class hierarchy (Admin, Manager, Member)
// ==========================================
    User::User(string un, string pw, string r) : username(un), password(pw), role(r) {}
    User::~User() {}

    Admin::Admin(string un) : User(un, "", "Admin") {}
    Admin::~Admin() {}
    void Admin::displayMenu() { cout << "--- Admin Menu ---" << endl; }

    Manager::Manager(string un) : User(un, "", "Manager") {}
    Manager::~Manager() {}
    void Manager::displayMenu() { cout << "--- Manager Menu ---" << endl; }

// ==========================================
// 2. linked list with merge sort and binary search
// ==========================================
    ProductLinkedList::ProductLinkedList() {
        head = NULL;
        count = 0;
    }

    ProductLinkedList::~ProductLinkedList() {
        Product* curr = head;
        while (curr != NULL) {
            Product* nextNode = curr->next;
            delete curr;
            curr = nextNode;
        }
    }

    // Merge Sort
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

    // Inserting a new node at the end of the linked list
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
    }

    // Deleting a node by product ID
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
        } else {
            while (temp != NULL && temp->productId != id) {
                prev = temp;
                temp = temp->next;
            }
            if (temp) {
                prev->next = temp->next;
                delete temp;
                count--;
            }
        }
    }

    // sortList: sort the linked list based on the specified criteria (1: quantity, 2: price, 0: ID)
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

        delete[] TmpArray;
    }

    // binarySearch: search for a product by ID using binary search (requires the list to be sorted by ID)
    Product* ProductLinkedList::binarySearch(int targetId) {
        if (count == 0) return NULL;

        sortList(0); 

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

    // Display the linked list (for testing purposes)
    void ProductLinkedList::display() {
        Product* temp = head;
        while (temp != NULL) {
            cout << "[ID: " << temp->productId 
                << " | Name: " << temp->productName 
                << " | Category: " << temp->category
                << " | Zone: " << temp->zone
                << " | Supplier: " << temp->supplier
                << " | Qty: " << temp->stockQuantity 
                << " | Price: RM" << temp->productPrice << "]\n";
            temp = temp->next;
        }
        cout << "-----------------------------------\n";
    }

// ==========================================
// main function for testing the ProductLinkedList class
// ==========================================
/*int main() {
    ProductLinkedList inventory;

    // 1. test inserting nodes
    inventory.insertNode(105, "Mouse", "Electronics", 50, "Zone A", "Supplier X", 25.5);
    inventory.insertNode(102, "Keyboard", "Electronics", 15, "Zone B", "Supplier Y", 120.0);
    inventory.insertNode(108, "Monitor", "Electronics", 5, "Zone C", "Supplier Z", 450.0);
    inventory.insertNode(101, "Cable", "Electronics", 200, "Zone A", "Supplier X", 15.0);

    cout << "Original List:\n";
    inventory.display();

    // 2. test sorting by price (sortBy = 2)
    cout << "Sorted by Price (Merge Sort):\n";
    inventory.sortList(2);
    inventory.display();

    // 3. test sorting by quantity (sortBy = 1)
    cout << "Sorted by Quantity (Merge Sort):\n";
    inventory.sortList(1);
    inventory.display();

    // 4. test binary search by product ID (sortBy = 0)
    cout << "Binary Search for ID 102:\n";
    Product* target = inventory.binarySearch(102);
    if (target) cout << "Found: " << target->productName << "\n";
    else cout << "Not Found\n";

    return 0;
}*/
#include <iostream>
#include <string>
using namespace std;

// ==========================================
// 1. structure definitions for products and orders
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
    int dispatchQuantity;
    string operatorName;
    string orderDate;
    
    Order* next;
};

// ==========================================
// 2. dynamic non-primitive data structure: user class hierarchy (Admin, Manager, Member)
// ==========================================
class User {
protected:
    string username;
    string role;
public:
    User(string un, string r) : username(un), role(r) {}
    virtual ~User() {} // virtual destructor for proper cleanup in derived classes
    virtual void displayMenu() = 0; // pure virtual function, waiting for Member 2 & 3 implementation
};

class Admin : public User {
public:
    Admin(string un) : User(un, "Admin") {}
    ~Admin() {}
    void displayMenu() override { cout << "--- Admin Menu ---" << endl; }
};

class Manager : public User {
public:
    Manager(string un) : User(un, "Manager") {}
    ~Manager() {}
    void displayMenu() override { cout << "--- Manager Menu ---" << endl; }
};

// ==========================================
// 3. linked list with merge sort and binary search
// ==========================================
class ProductLinkedList {
private:
    Product* head;
    int count;

    // Merge Sort
    void Merge(Product** arr, int lpos, int rpos, int rend, int sortBy) {
        int lend = rpos - 1;
        int tmppos = 0;
        int numelements = rend - lpos + 1;
        
        // distribute the elements into the temporary array
        Product** TmpArray;
        TmpArray = new Product*[numelements];

        // use condition to decide which element to compare and move into the temporary array
        while (lpos <= lend && rpos <= rend) {
            bool condition = false;
            if (sortBy == 1) { // 1: based on stock quantity
                condition = arr[lpos]->stockQuantity <= arr[rpos]->stockQuantity;
            } else if (sortBy == 2) { // 2: based on product price
                condition = arr[lpos]->productPrice <= arr[rpos]->productPrice;
            } else { // 0: based on product ID (required for binary search)
                condition = arr[lpos]->productId <= arr[rpos]->productId;
            }

            if (condition) TmpArray[tmppos++] = arr[lpos++];
            else TmpArray[tmppos++] = arr[rpos++];
        }

        while (lpos <= lend) TmpArray[tmppos++] = arr[lpos++];
        while (rpos <= rend) TmpArray[tmppos++] = arr[rpos++];

        // copy the sorted temporary array back to the original array
        for (int i = 0; i < numelements; i++) {
            arr[rend - i] = TmpArray[numelements - 1 - i];
        }
        
        delete[] TmpArray; // free temporary array memory
    }

    void MergeSortRec(Product** arr, int left, int right, int sortBy) {
        int center;
        if (left < right) {
            center = (left + right) / 2;
            MergeSortRec(arr, left, center, sortBy);
            MergeSortRec(arr, center + 1, right, sortBy);
            Merge(arr, left, center + 1, right, sortBy);
        }
    }

public:
    ProductLinkedList() {
        head = NULL;
        count = 0;
    }

    ~ProductLinkedList() {
        Product* curr = head;
        while (curr != NULL) {
            Product* nextNode = curr->next;
            delete curr;
            curr = nextNode;
        }
    }

    // Inserting a new node at the end of the linked list
    void insertNode(int id, string name, string cat, int qty, string z, string sup, double price) {
        Product *temp;
        temp = new Product;
        temp->productId = id;
        temp->productName = name;
        temp->category = cat;
        temp->stockQuantity = qty;
        temp->zone = z;
        temp->supplier = sup;
        temp->productPrice = price;
        temp->next = NULL;
        if(head == NULL){
            head = temp;
            temp->next = NULL;
        } else {
            Product *curr;
            curr = head;
            while(curr->next != NULL){
                curr = curr->next;
            }
            curr->next = temp;
        }
        count++;
    }

    // Deleting a node by product ID
    void deleteNode(int id) {
        if (head == NULL){
            cout << "List is empty. Nothing to delete." << endl;
            return; // empty list, nothing to delete
        }
        Product* temp;
        Product* prev = NULL;
        temp = head;
        if (head->productId == id) {
            temp = head->next;
            delete head;
            head = temp;
            count--;
        } else {
            temp = head;
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
    void sortList(int sortBy) {
        if (count <= 1) return;

        // 1. temporary array to hold pointers to the nodes for sorting
        Product** TmpArray = new Product*[count];
        Product* curr = head;
        int i = 0;
        while (curr != NULL) {
            TmpArray[i++] = curr;
            curr = curr->next;
        }

        // 2. MergeSortRec: sort the array of pointers based on the specified criteria
        MergeSortRec(TmpArray, 0, count - 1, sortBy);

        // 3. Reconstruct the linked list based on the sorted array
        head = TmpArray[0];
        curr = head;
        for (i = 1; i < count; i++) {
            curr->next = TmpArray[i];
            curr = curr->next;
        }
        curr->next = NULL; // terminate the linked list

        delete[] TmpArray; // free temporary array memory
    }

    // binarySearch: search for a product by ID using binary search (requires the list to be sorted by ID)
    Product* binarySearch(int targetId) {
        if (count == 0) return NULL;

        // Attention: binary search requires the list to be sorted by product ID, so we sort it first
        sortList(0); 

        // temporary array to hold pointers to the nodes for binary search
        Product** TmpArray = new Product*[count];
        Product* curr = head;
        int i = 0;
        while (curr != NULL) {
            TmpArray[i++] = curr;
            curr = curr->next;
        }

        int first, last, mid;
        // not use int found; because we want to return the pointer to the found product, not just a boolean
        Product* found = NULL;
        first = 0;
        last = count - 1;
        mid = 0;
        
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

        delete[] TmpArray; // free temporary array memory
        return found; // return the found product pointer or NULL if not found
    }

    // Display the linked list (for testing purposes)
    void display() {
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
};

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
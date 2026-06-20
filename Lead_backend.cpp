#include <iostream>
#include <string>
#include <iomanip> 
#include "backend.h"
#include "structures.h"

using namespace std;

// Helper: truncate a field to fit a fixed column width so a long string
// (e.g. a long supplier name) cannot push the rest of the row out of
// alignment when used together with setw().
static string truncateField(const string& text, int width) {
    if ((int)text.length() <= width) return text;
    if (width <= 3) return text.substr(0, width); // too narrow to fit "..."
    return text.substr(0, width - 3) + "...";
}


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

    // Display the linked list as an aligned table using setw()
    void ProductLinkedList::display() {
        if (head == NULL) {
            cout << "[System] Inventory is empty.\n";
            return;
        }

        // Header
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

        // Rows
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

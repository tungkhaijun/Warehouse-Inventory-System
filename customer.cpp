#include<iostream>
#include<string>
#include <fstream>
#include"customer.h"

using namespace std;void customer::addbooking(){
	booking* newbooking = new booking;
	
	cout<<"\n-- Create New Booking --\n";
	cout<<"Enter Booking ID (e.g: 1000): ";
	cin >> newbooking->bookingid;
	
	cout<<"Enter Product ID you want to book: ";
	cin >> newbooking->productid;
	
	cout<<"Enter Quantity: ";
	cin >> newbooking->quantity;
	
	cout<<"EnterOrder Date: ";
	cin >> newbooking->orderdate;
	
	newbooking->next = nullptr;
	
	if(head == nullptr){
		head = newbooking;
		tail = newbooking;
	} else{
		tail->next = newbooking;
		tail = newbooking;
	}
	
	cout << "\n[SUCCESS] Booking ID"<<newbooking->bookingid<<"has been successfully added to the system!\n";
}

void customer::displaybookings(){
	cout<<"\n-- My Booking Records --\n";
	
	if(head == nullptr){
		cout<<"[INFO] You have no bookings. Go to booking!\n";
		return;
	}
	
	booking* current = head;
	int recordcount = 1;
	
	while(current != nullptr){
		cout<<"Record #"<< recordcount<<"\n";
		cout << "  Booking ID : " << current->bookingid << "\n";
        cout << "  Product ID : " << current->productid << "\n";
        cout << "  Quantity   : " << current->quantity << "\n";
        cout << "  Order Date : " << current->orderdate << "\n";
        cout << "---------------------------------\n";
        
        current = current->next;
        recordcount++;
	}
	cout <<"[SYSTEM] End of records.\n";
}

void customer::searchbooking() {
	cout<<"\n-- Search Booking Record --\n";
	
	if(head == nullptr){
		cout << "[INFO] System id empty. No bookings to search.\n";
		return;
	}
	
	int targetid;
	cout<< "Enter the Booking ID you want to search: ";
	cin>> targetid;
	
	booking*current = head;
	bool found = false;
	
	while(current != nullptr){
		if (current->bookingid == targetid){
			cout << "\n[SUCCESS] Record Found!\n";
            cout << "---------------------------------\n";
            cout << "  Booking ID : " << current->bookingid << "\n";
            cout << "  Product ID : " << current->productid << "\n";
            cout << "  Quantity   : " << current->quantity << "\n";
            cout << "  Order Date : " << current->orderdate << "\n";
            cout << "---------------------------------\n";
            found = true;
            break;
		}
		current = current->next;
	}
	if(!found){
		cout << "\n[ERROR] Booking ID " << targetid << " not found in the system.\n";
	}
}

void customer::editbooking(){
	cout << "\n--- Edit Booking Details ---\n";
	if (head == nullptr) {
   	cout<<"[INFO] System is empty. No bookings to edit.\n";
	return;
}

int targetid;
cout<< "Enter the Booking ID you want to edit: ";
cin>>targetid;

booking* current = head;
bool found = false;

while(current != nullptr){
	if(current->bookingid == targetid){
		cout<<"\n[RECORD FOUND]\n";
		cout<<"Current Quantity for Product " << current->productid << " is: " << current->quantity << "\n";
		
		int newquantity;
		cout<<"Enter NEW Quantity: ";
		cin>>newquantity;
		
		if (newquantity<0){
			cout<<"[ERROR] Quantity cannot be negative. Update aborted.\n";
		} else{
			current->quantity = newquantity;
			cout<<"[SUCCESS] Booking ID " << targetid << " has been updated successfully!\n";
		}
		found = true;
		break;
	}
	current = current->next;
}
if(!found){
	cout << "\n[ERROR] Booking ID " << targetid << " not found in the system.\n";
}
}

void customer::generatereport(){
	cout<< "\n--- Generating Booking Summary Report ---\n";
	
	if(head == nullptr){
		cout<< "[INFO] System is empty. No bookings to report.\n";
		return;
	}
	
	cout << "[SYSTEM] Sorting records by Quantity (Highest to Lowest)...\n";
    sortbookings(2);
	
	ofstream outFile("BookingSummary.txt");
	
	if(!outFile){
		cout<<"[ERROR] Unable to create report file!\n";
		return;
	}
	
	outFile << "=========================================\n";
    outFile << "        BOOKING SUMMARY REPORT           \n";
    outFile << "=========================================\n";
    
    booking* current = head;
    while (current!= nullptr){
    	outFile << "Booking ID: " << current->bookingid
                << " | Product ID: " << current->productid
                << " | Quantity: " << current->quantity
                << " | Date: " << current->orderdate << "\n";
                
        current = current->next;
	}
	
	outFile.close();
	cout << "[SUCCESS] Report successfully saved to 'BookingSummary.txt'!\n";
	cout << "\n[SYSTEM] Retrieving data from text file...\n\n";
    
    ifstream inFile("BookingSummary.txt");
    string line;
    
    while(getline(inFile,line)){
    	cout << line << "\n";
	}
	
	inFile.close();
}

void customer::sortbookings(int criteria) {
    if (head == nullptr || head->next == nullptr) {
        return;
    }

    bool swapped;
    booking* current;
    booking* lastPtr = nullptr;

    do {
        swapped = false;
        current = head;

        while (current->next != lastPtr) {
            bool needswap = false;

            if (criteria == 1) {
                if (current->bookingid > current->next->bookingid) {
                    needswap = true;
                }
            }
            else if (criteria == 2) {
                if (current->quantity < current->next->quantity) {
                    needswap = true;
                }
            }

            if (needswap) {
                int tempid = current->bookingid;
                current->bookingid = current->next->bookingid;
                current->next->bookingid = tempid;

                int tempprodid = current->productid;
                current->productid = current->next->productid;
                current->next->productid = tempprodid;

                int tempqty = current->quantity;
                current->quantity = current->next->quantity;
                current->next->quantity = tempqty;

                string tempdate = current->orderdate;
                current->orderdate = current->next->orderdate;
                current->next->orderdate = tempdate;

                swapped = true;
            }
            current = current->next;
        }
        lastPtr = current;
    } while (swapped);
    
    cout << "\n[SUCCESS] Records sorted successfully!\n";
}

void customer::displayMenu(){
	int choice = 0;
	bool keepRunning = true;
	
	while(keepRunning){
		cout<<"\n==============================\n";
		cout<<"\n       Customer Dashboard     \n";
		cout<<"\n==============================\n";
		cout<<"1. Make a New Booking\n";
		cout<<"2. Edit Booking Details\n";
		cout<<"3. View the Bookings\n";
		cout<<"4. Search Bookings\n";
		cout<<"5. Generate Booking Report\n";
		cout<<"6. Logout\n";
		cout<<"\n==============================\n";
		cout<<"Enter your Choice (1-6): ";
		
		try{
			cin>>choice;
			if(cin.fail()){
				throw string("Invalid input detected! Please enter the valid number!");
			}
		if (choice < 1 || choice>6){
			throw choice;
		}
		switch (choice){
			case 1:
				cout<<"\n[SYSTEM] Loading Add Booking Module...\n";
				addbooking();
				break;
			case 2:
				cout<<"\n[SYSTEM] Loadng Edit Booking Module...\n";
				editbooking();
				break;
			case 3:
				cout<<"\n[SYSTEM] Displaying All Records...\n";
				displaybookings();
				break;
			case 4:
				cout<<"\n[SYSTEM] Displaying All Records...\n";
				break;
			case 5:
				cout<<"\n[SYSTEM] Loading Search Module...\n";
				searchbooking();
				break;
			case 6:
				cout<<"\n[SYSTEM] Generating Booking's Report...\n";
				generatereport();
				break;
		}
	}

catch(string errorMsg){
	cout<<"\n[ERROR]"<< errorMsg <<endl;
	cin.clear();
	cin.ignore(10000, '\n');
	
}

catch(int invalidChoice){
	cout<<"\n[ERROR]"<<invalidChoice<<"is OUT of Range! Please Enter 1-6...\n";
}
		}
		
		
	}
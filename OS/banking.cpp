#include <iostream>
#include <string>
#include <iomanip>
#include <sys/shm.h>
using namespace std;

const int MAX_TRANSACTIONS = 100;
const string LINE = "\t\t--------------------------------------------------";

struct Transaction {
    string type;
    double amount;
    string time;  // Added for potential future enhancement
};

struct Account {
    string name;
    double balance;
    int transaction_count;
    Transaction transactions[MAX_TRANSACTIONS];
};

void displayHeader() {
    cout << LINE << endl;
    cout << "\t\t\t  BANKING SYSTEM PROCESS" << endl;
    cout << LINE << endl << endl;
}

void displayMenu() {
    cout << "\nPlease select an option:" << endl;
    cout << LINE << endl;
    cout << "1. Check Account Balance" << endl;
    cout << "2. Deposit Money" << endl;
    cout << "3. Withdraw Money" << endl;
    cout << "4. Transfer Money" << endl;
    cout << "5. View Transaction History" << endl;
    cout << "6. Exit Banking System" << endl;
    cout << LINE << endl;
    cout << "Enter your choice (1-6): ";
}

void updateSharedMemory(int value) {
    int* shared_memory;
    int shmid = shmget((key_t)1122, sizeof(int), 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    shared_memory = (int*)shmat(shmid, NULL, 0);
    if (shared_memory == (void*)-1) {
        perror("shmat failed");
        exit(1);
    }

    *shared_memory += value;

    if (shmdt(shared_memory) == -1) {
        perror("shmdt failed");
        exit(1);
    }
}

double deposit(Account &account, double amount) {
    account.balance += amount;
    Transaction transaction = {"DEPOSIT", amount};
    account.transactions[account.transaction_count++] = transaction;
    cout << "\nSuccessfully deposited $" << fixed << setprecision(2) << amount << endl;
    return account.balance;
}

double withdraw(Account &account, double amount) {
    if (account.balance >= amount) {
        account.balance -= amount;
        Transaction transaction = {"WITHDRAWAL", amount};
        account.transactions[account.transaction_count++] = transaction;
        cout << "\nSuccessfully withdrew $" << fixed << setprecision(2) << amount << endl;
    } else {
        cout << "\nTransaction failed: Insufficient balance!" << endl;
    }
    return account.balance;
}

void transferMoney(Account &sender, Account &receiver, double amount) {
    if (sender.balance >= amount) {
        sender.balance -= amount;
        receiver.balance += amount;
        
        Transaction sender_transaction = {"TRANSFER TO " + receiver.name, amount};
        Transaction receiver_transaction = {"TRANSFER FROM " + sender.name, amount};
        
        sender.transactions[sender.transaction_count++] = sender_transaction;
        receiver.transactions[receiver.transaction_count++] = receiver_transaction;
        
        cout << "\nSuccessfully transferred $" << fixed << setprecision(2) 
             << amount << " to " << receiver.name << endl;
    } else {
        cout << "\nTransfer failed: Insufficient balance!" << endl;
    }
}

void displayTransactionHistory(Account &account) {
    cout << LINE << endl;
    cout << "\tTRANSACTION HISTORY FOR " << account.name << endl;
    cout << LINE << endl;
    
    if (account.transaction_count == 0) {
        cout << "No transactions recorded yet." << endl;
        return;
    }

    cout << left << setw(20) << "TYPE" << right << setw(15) << "AMOUNT ($)" << endl;
    cout << LINE << endl;
    
    for (int i = 0; i < account.transaction_count; i++) {
        cout << left << setw(20) << account.transactions[i].type
             << right << setw(15) << fixed << setprecision(2) 
             << account.transactions[i].amount << endl;
    }
}

int main() {
    displayHeader();
    
    // Initialize shared memory
    updateSharedMemory(-2);
    
    // Initialize accounts
    Account laiba = {"Laiba", 1000.0, 0};
    Account kamla = {"Kamla", 500.0, 0};
    Account ayesha = {"Ayesha", 500.0, 0};
    Account saad = {"Saad", 500.0, 0};
    
    int choice;
    double amount;

    do {
        displayMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "\nCurrent Balance: $" << fixed << setprecision(2) 
                     << laiba.balance << endl;
                break;
                
            case 2:
                cout << "\nEnter deposit amount: $";
                cin >> amount;
                if (amount > 0) {
                    deposit(laiba, amount);
                } else {
                    cout << "\nInvalid amount! Please enter a positive value." << endl;
                }
                break;
                
            case 3:
                cout << "\nEnter withdrawal amount: $";
                cin >> amount;
                if (amount > 0) {
                    withdraw(laiba, amount);
                } else {
                    cout << "\nInvalid amount! Please enter a positive value." << endl;
                }
                break;
                
            case 4:
                cout << "\nEnter amount to transfer: $";
                cin >> amount;
                if (amount > 0) {
                    transferMoney(laiba, kamla, amount);
                } else {
                    cout << "\nInvalid amount! Please enter a positive value." << endl;
                }
                break;
                
            case 5:
                displayTransactionHistory(laiba);
                break;
                
            case 6:
                updateSharedMemory(2);
                cout << LINE << endl;
                cout << "\tThank you for using our Banking System!" << endl;
                cout << LINE << endl;
                return 0;
                
            default:
                cout << "\nInvalid choice! Please select 1-6." << endl;
        }
    } while (choice != 6);

    return 0;
}
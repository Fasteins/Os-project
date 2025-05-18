#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <sys/shm.h>
using namespace std;

const string BORDER = "\t\t================================================";

// Function to manage shared memory operations
void updateSharedMemory(int value) {
    int* shared_memory;
    int shmid = shmget((key_t)1122, sizeof(int), 0666);
    
    if (shmid == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    shared_memory = (int*)shmat(shmid, NULL, 0);
    if (shared_memory == (void*)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    *shared_memory += value;

    if (shmdt(shared_memory) == -1) {
        perror("shmdt failed");
        exit(EXIT_FAILURE);
    }
}

void displayHeader() {
    cout << BORDER << endl;
    cout << "\t\t\tCALCULATOR PROCESS" << endl;
    cout << BORDER << endl << endl;
}

void displayMainMenu() {
    cout << "\nMain Menu:" << endl;
    cout << BORDER << endl;
    cout << "1. Start Calculator" << endl;
    cout << "0. Exit Program" << endl;
    cout << BORDER << endl;
    cout << "Enter your choice: ";
}

void displayOperationMenu() {
    cout << "\nAvailable Operations:" << endl;
    cout << BORDER << endl;
    cout << "1. Addition" << endl;
    cout << "2. Subtraction" << endl;
    cout << "3. Multiplication" << endl;
    cout << "4. Division" << endl;
    cout << BORDER << endl;
    cout << "Enter operation choice: ";
}

double performOperation(int op, double num1, double num2) {
    switch(op) {
        case 1: return num1 + num2;
        case 2: return num1 - num2;
        case 3: return num1 * num2;
        case 4: 
            if (num2 == 0) {
                cout << "Error: Division by zero!" << endl;
                return 0;
            }
            return num1 / num2;
        default:
            cout << "Invalid operation selected!" << endl;
            return 0;
    }
}

string getOperationName(int op) {
    switch(op) {
        case 1: return "Addition";
        case 2: return "Subtraction";
        case 3: return "Multiplication";
        case 4: return "Division";
        default: return "Unknown Operation";
    }
}

int main() {
    displayHeader();
    updateSharedMemory(-10); // Initialize shared memory

    int mainChoice, operationChoice;
    double num1, num2, result;

    do {
        displayMainMenu();
        cin >> mainChoice;

        // Validate input
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nInvalid input! Please enter a number.\n";
            continue;
        }

        if (mainChoice == 1) {
            displayOperationMenu();
            cin >> operationChoice;

            if (operationChoice < 1 || operationChoice > 4) {
                cout << "Invalid operation choice!" << endl;
                continue;
            }

            cout << "Enter first number: ";
            cin >> num1;
            cout << "Enter second number: ";
            cin >> num2;

            // Validate numbers
            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "\nInvalid number input!\n";
                continue;
            }

            result = performOperation(operationChoice, num1, num2);
            
            cout << "\nResult of " << getOperationName(operationChoice) 
                 << ": " << num1 << " and " << num2 << " = " << result << endl;
        }
        else if (mainChoice != 0) {
            cout << "Invalid choice! Please enter 0 or 1." << endl;
        }

    } while (mainChoice != 0);

    updateSharedMemory(10);
    cout << BORDER << endl;
    cout << "\t\tThank you for using Calculator!" << endl;
    cout << BORDER << endl;

    return 0;
}
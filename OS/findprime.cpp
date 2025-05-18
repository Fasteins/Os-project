#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <sys/shm.h>
#include <limits> // For input validation
#include <cmath>  // For sqrt function
using namespace std;

const string BORDER = "\t\t================================================";

// Function to manage shared memory operations
void updateSharedMemory(int value) {
    int* shared_memory;
    int shmid = shmget((key_t)1122, sizeof(int), 0666);
    
    if (shmid == -1) {
        perror("Error: Shared memory allocation failed");
        exit(EXIT_FAILURE);
    }

    shared_memory = (int*)shmat(shmid, NULL, 0);
    if (shared_memory == (void*)-1) {
        perror("Error: Shared memory attachment failed");
        exit(EXIT_FAILURE);
    }

    *shared_memory += value;

    if (shmdt(shared_memory) == -1) {
        perror("Error: Shared memory detachment failed");
        exit(EXIT_FAILURE);
    }
}

void displayHeader() {
    cout << BORDER << endl;
    cout << "\t\t\tPRIME NUMBER CHECKER" << endl;
    cout << BORDER << endl << endl;
}

bool isPrime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    
    // Optimized prime checking - only check up to sqrt(n)
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return false;
        }
    }
    return true;
}

void checkPrimeNumber() {
    int number;
    cout << "Enter a positive integer: ";
    
    // Input validation
    while (!(cin >> number) || number < 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input! Please enter a positive integer: ";
    }

    if (isPrime(number)) {
        cout << number << " is a prime number." << endl;
    } else {
        cout << number << " is not a prime number." << endl;
    }
}

void displayMenu() {
    cout << "\nMain Menu:" << endl;
    cout << BORDER << endl;
    cout << "1. Check if a number is prime" << endl;
    cout << "0. Exit program" << endl;
    cout << BORDER << endl;
    cout << "Enter your choice: ";
}

int main() {
    displayHeader();
    updateSharedMemory(-20); // Initialize shared memory

    int choice;
    
    do {
        displayMenu();
        cin >> choice;

        // Input validation
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter 0 or 1." << endl;
            continue;
        }

        switch (choice) {
            case 1:
                checkPrimeNumber();
                break;
            case 0:
                cout << "Exiting program..." << endl;
                break;
            default:
                cout << "Invalid choice! Please enter 0 or 1." << endl;
        }
    } while (choice != 0);

    updateSharedMemory(20); // Clean up shared memory
    cout << BORDER << endl;
    cout << "\t\tThank you for using Prime Number Checker!" << endl;
    cout << BORDER << endl;

    return 0;
}
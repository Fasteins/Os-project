#include <iostream>
#include <unistd.h>
#include <sys/shm.h>
#include <limits> // For input validation
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
    cout << "\t\t\tFACTORIAL CALCULATOR" << endl;
    cout << BORDER << endl << endl;
}

unsigned long long factorial(int n) {
    if (n < 0) return 0; // Handle negative numbers
    if (n == 0 || n == 1) return 1;
    
    unsigned long long result = 1;
    for (int i = 2; i <= n; ++i) {
        // Check for overflow before multiplying
        if (result > numeric_limits<unsigned long long>::max() / i) {
            cerr << "Error: Factorial exceeds maximum value!" << endl;
            return 0;
        }
        result *= i;
    }
    return result;
}

void displayMenu() {
    cout << "\nMain Menu:" << endl;
    cout << BORDER << endl;
    cout << "1. Calculate Factorial" << endl;
    cout << "0. Exit Program" << endl;
    cout << BORDER << endl;
    cout << "Enter your choice: ";
}

int main() {
    displayHeader();
    updateSharedMemory(-11); // Initialize shared memory

    int choice, number;
    
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
            case 1: {
                cout << "Enter a positive integer (0-20): ";
                cin >> number;
                
                if (cin.fail() || number < 0) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input! Please enter a positive integer." << endl;
                    break;
                }
                
                if (number > 20) {
                    cout << "Warning: Values above 20 may cause overflow!" << endl;
                }
                
                unsigned long long result = factorial(number);
                if (result != 0 || number == 0) {
                    cout << "Factorial of " << number << " is: " << result << endl;
                }
                break;
            }
            case 0:
                cout << "Exiting program..." << endl;
                break;
            default:
                cout << "Invalid choice! Please enter 0 or 1." << endl;
        }
    } while (choice != 0);

    updateSharedMemory(11); // Clean up shared memory
    cout << BORDER << endl;
    cout << "\t\tThank you for using Factorial Calculator!" << endl;
    cout << BORDER << endl;

    return 0;
}
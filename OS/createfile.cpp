#include <iostream>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <sys/shm.h>
#include <limits> // for numeric_limits
using namespace std;

const string BORDER = "\t\t================================================";

// Function to manage shared memory operations
void updateSharedMemory(int value) {
    int* shared_memory;
    int shmid = shmget((key_t)1122, sizeof(int), 0666);
    
    if (shmid == -1) {
        perror("Error: shmget failed");
        exit(EXIT_FAILURE);
    }

    shared_memory = (int*)shmat(shmid, NULL, 0);
    if (shared_memory == (void*)-1) {
        perror("Error: shmat failed");
        exit(EXIT_FAILURE);
    }

    *shared_memory += value;

    if (shmdt(shared_memory) == -1) {
        perror("Error: shmdt failed");
        exit(EXIT_FAILURE);
    }
}

void displayHeader() {
    cout << BORDER << endl;
    cout << "\t\t\tFILE CREATION PROCESS" << endl;
    cout << BORDER << endl << endl;
}

bool createFile(const string& filename) {
    ofstream file(filename.c_str());
    if (!file) {
        cerr << "Error: Could not create file '" << filename << "'" << endl;
        return false;
    }
    file.close();
    cout << "Success: File '" << filename << "' created successfully!" << endl;
    return true;
}

void displayMenu() {
    cout << "\nMain Menu:" << endl;
    cout << BORDER << endl;
    cout << "1. Create a new file" << endl;
    cout << "0. Exit program" << endl;
    cout << BORDER << endl;
    cout << "Enter your choice: ";
}

int main() {
    displayHeader();
    updateSharedMemory(-6); // Initialize shared memory

    int choice;
    string filename;

    do {
        displayMenu();
        cin >> choice;

        // Clear input buffer and handle invalid input
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter 0 or 1." << endl;
            continue;
        }

        switch (choice) {
            case 1:
                cout << "Enter filename to create: ";
                cin >> filename;
                
                // Basic filename validation
                if (filename.empty()) {
                    cout << "Error: Filename cannot be empty!" << endl;
                } else if (filename.find('/') != string::npos || 
                           filename.find('\\') != string::npos) {
                    cout << "Error: Filename cannot contain path separators!" << endl;
                } else {
                    createFile(filename);
                }
                break;

            case 0:
                cout << "Exiting program..." << endl;
                break;

            default:
                cout << "Invalid choice! Please enter 0 or 1." << endl;
        }

    } while (choice != 0);

    // Clean up shared memory before exiting
    updateSharedMemory(6);
    
    cout << BORDER << endl;
    cout << "\t\tThank you for using File Creator!" << endl;
    cout << BORDER << endl;

    return 0;
}
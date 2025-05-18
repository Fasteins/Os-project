#include <iostream>
#include <sys/shm.h>
using namespace std;

const string BORDER = "\t\t================================================";

// Function to perform binary search
int binarySearch(int arr[], int size, int target) {
    int left = 0;
    int right = size - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (arr[mid] == target)
            return mid;

        if (arr[mid] < target)
            left = mid + 1;
        else
            right = mid - 1;
    }

    return -1;
}

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
    cout << "\t\t\tBINARY SEARCH PROCESS" << endl;
    cout << BORDER << endl << endl;
}

void displayMenu() {
    cout << "\nMain Menu:" << endl;
    cout << BORDER << endl;
    cout << "1. Search for a number in the array" << endl;
    cout << "0. Exit the program" << endl;
    cout << BORDER << endl;
    cout << "Enter your choice: ";
}

int main() {
    displayHeader();
    updateSharedMemory(-40); // Initialize shared memory

    const int ARRAY_SIZE = 10;
    int arr[ARRAY_SIZE] = {2, 5, 8, 12, 16, 23, 38, 56, 72, 91};
    int choice, target;

    while (true) {
        displayMenu();
        cin >> choice;

        // Validate input
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nInvalid input! Please enter a number.\n";
            continue;
        }

        switch (choice) {
            case 1: {
                cout << "\nArray contents: ";
                for (int i = 0; i < ARRAY_SIZE; i++) {
                    cout << arr[i] << " ";
                }
                cout << "\nEnter number to find: ";
                cin >> target;

                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "\nInvalid input! Please enter a valid number.\n";
                    break;
                }

                int result = binarySearch(arr, ARRAY_SIZE, target);

                if (result == -1) {
                    cout << "\nNumber " << target << " not found in the array.\n";
                } else {
                    cout << "\nNumber " << target << " found at index " << result << ".\n";
                }
                break;
            }
            case 0:
                updateSharedMemory(40);
                cout << BORDER << endl;
                cout << "\t\tThank you for using Binary Search!" << endl;
                cout << BORDER << endl;
                return 0;
            default:
                cout << "\nInvalid choice! Please enter 0 or 1.\n";
        }
    }

    return 0;
}
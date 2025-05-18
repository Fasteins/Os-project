#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <sys/shm.h>

using namespace std;

// Function to analyze characters in a string
void* analyzeString() {
    string inputLine;
    int alphabets = 0, digits = 0, spaces = 0, others = 0;

    // Clear input buffer before getline
    cin.ignore();
    cout << "\nEnter a sentence to analyze: ";
    getline(cin, inputLine);

    // Analyze each character
    for (char c : inputLine) {
        if (isalpha(c)) {
            alphabets++;
        } else if (isdigit(c)) {
            digits++;
        } else if (isspace(c)) {
            spaces++;
        } else {
            others++;
        }
    }

    // Display results
    cout << "\n=== Character Analysis Report ===\n";
    cout << "Alphabets      : " << alphabets << endl;
    cout << "Digits         : " << digits << endl;
    cout << "Spaces         : " << spaces << endl;
    cout << "Other Characters: " << others << endl << endl;

    return NULL;
}

int main() {
    int choice;

    // Welcome message
    cout << "\n--------------------------------------------------\n";
    cout << "\tWelcome to the String Analyzer Program!";
    cout << "\n--------------------------------------------------\n";

    // Attach shared memory
    int* shared_memory;
    int shmid = shmget((key_t)1122, sizeof(int), 0666);
    if (shmid == -1) {
        perror("Shared memory access failed");
        exit(1);
    }

    shared_memory = (int*)shmat(shmid, NULL, 0);
    if (shared_memory == (void*)-1) {
        perror("Shared memory attach failed");
        exit(1);
    }

    *shared_memory -= 11;

    if (shmdt(shared_memory) == -1) {
        perror("Shared memory detach failed");
        exit(1);
    }

    // Menu loop
    do {
        cout << "\nMenu:\n";
        cout << "1. Analyze a string\n";
        cout << "0. Exit the program\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            analyzeString();
        } else if (choice != 0) {
            cout << "Invalid choice! Exiting...\n";
            choice = 0;
        }
    } while (choice == 1);

    // Re-attach to shared memory
    shmid = shmget((key_t)1122, sizeof(int), 0666);
    shared_memory = (int*)shmat(shmid, NULL, 0);
    *shared_memory += 11;

    if (shmdt(shared_memory) == -1) {
        perror("Shared memory detach failed");
        exit(1);
    }

    cout << "\nThank you for using the String Analyzer. Goodbye!\n";
    return 0;
}

#include <iostream>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <ctime>
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
    cout << "\t\t\tGUESSING GAME WORLD" << endl;
    cout << BORDER << endl << endl;
}

void clearScreen() {
    // More portable screen clearing than system("clear")
    cout << "\033[2J\033[1;1H"; // ANSI escape codes
}

void playGame() {
    srand(time(0));        // Seed random number generator
    int secret = rand() % 10 + 1; // Random number between 1 and 10
    int guess, tries = 0;
    
    clearScreen();
    cout << "I'm thinking of a number between 1 and 10..." << endl;
    
    do {
        cout << "Enter your guess (1-10): ";
        
        // Input validation
        while (!(cin >> guess) || guess < 1 || guess > 10) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter a number between 1 and 10: ";
        }
        
        tries++;
        
        if (guess > secret) {
            cout << "Too high! Try again." << endl;
        } else if (guess < secret) {
            cout << "Too low! Try again." << endl;
        }
    } while (guess != secret);
    
    cout << "\nCongratulations! You guessed the number in " << tries 
         << (tries == 1 ? " try!" : " tries!") << endl;
    
    sleep(3);
    clearScreen();
}

void displayMenu() {
    cout << "\nMain Menu:" << endl;
    cout << BORDER << endl;
    cout << "1. Start New Game" << endl;
    cout << "0. Exit Game" << endl;
    cout << BORDER << endl;
    cout << "Enter your choice: ";
}

int main() {
    displayHeader();
    updateSharedMemory(-2); // Initialize shared memory

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
                playGame();
                break;
            case 0:
                cout << "Thanks for playing!" << endl;
                break;
            default:
                cout << "Invalid choice! Please enter 0 or 1." << endl;
        }
    } while (choice != 0);

    updateSharedMemory(2); // Clean up shared memory
    return 0;
}
#include <iostream>
#include <iomanip>
#include <ctime>
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
    cout << "\t\t\tCALENDAR PROCESS" << endl;
    cout << BORDER << endl << endl;
}

bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int getDaysInMonth(int month, int year) {
    if (month == 2) {
        return isLeapYear(year) ? 29 : 28;
    } else if (month == 4 || month == 6 || month == 9 || month == 11) {
        return 30;
    } else {
        return 31;
    }
}

string getMonthName(int month) {
    const string months[] = {"January", "February", "March", "April", "May", "June",
                            "July", "August", "September", "October", "November", "December"};
    return months[month - 1];
}

void displayCalendar(int month, int year) {
    // Get starting day of the week (0=Sunday, 6=Saturday)
    tm timeinfo = {0};
    timeinfo.tm_year = year - 1900;
    timeinfo.tm_mon = month - 1;
    timeinfo.tm_mday = 1;
    mktime(&timeinfo);
    int startDay = timeinfo.tm_wday;

    int daysInMonth = getDaysInMonth(month, year);

    // Display calendar header
    cout << "\n" << setw(20) << getMonthName(month) << " " << year << endl;
    cout << BORDER << endl;
    cout << " Sun Mon Tue Wed Thu Fri Sat" << endl;
    cout << BORDER << endl;

    // Print leading spaces
    for (int i = 0; i < startDay; i++) {
        cout << "    ";
    }

    // Print days of month
    for (int day = 1; day <= daysInMonth; day++) {
        cout << setw(4) << day;
        if ((day + startDay) % 7 == 0 || day == daysInMonth) {
            cout << endl;
        }
    }
}

int main() {
    displayHeader();
    updateSharedMemory(-10); // Initialize shared memory

    // Get current date
    time_t now = time(0);
    tm *localTime = localtime(&now);
    int currentYear = 1900 + localTime->tm_year;
    int currentMonth = 1 + localTime->tm_mon;

    int choice, year, month;

    do {
        cout << "\nCurrent date: " << getMonthName(currentMonth) << " " << currentYear << endl;
        cout << "\n1. View calendar for specific month/year" << endl;
        cout << "2. View current month" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter a number.\n";
            continue;
        }

        switch (choice) {
            case 1:
                cout << "Enter year: ";
                cin >> year;
                if (year < 1900 || year > 9999) {
                    cout << "Invalid year! Using current year instead.\n";
                    year = currentYear;
                }

                cout << "Enter month (1-12): ";
                cin >> month;
                if (month < 1 || month > 12) {
                    cout << "Invalid month! Using current month instead.\n";
                    month = currentMonth;
                }
                break;

            case 2:
                year = currentYear;
                month = currentMonth;
                break;

            case 0:
                break;

            default:
                cout << "Invalid choice! Please try again.\n";
                continue;
        }

        if (choice == 1 || choice == 2) {
            displayCalendar(month, year);
        }

    } while (choice != 0);

    updateSharedMemory(10);
    cout << BORDER << endl;
    cout << "\t\tThank you for using Calendar!" << endl;
    cout << BORDER << endl;

    return 0;
}
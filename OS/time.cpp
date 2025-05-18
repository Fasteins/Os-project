#include<iostream>
#include<ctime>
#include<cstdlib>
#include<unistd.h>
#include<sys/shm.h>
using namespace std;

int main() {
    int *shared_memory;
    int shmid;
    
    // Retrieving shared memory segment
    shmid = shmget((key_t)1122, sizeof(int), 0666);
    shared_memory = (int*)shmat(shmid, NULL, 0);

    // Adjusting shared memory value
    int updated_data = *shared_memory - 30;
    *shared_memory = updated_data;

    // Detach the shared memory segment
    if (shmdt(shared_memory) == -1) {
        perror("shmdt error");
        exit(1);
    }

    int previous_second = 0;

    while (true) {
        int seconds, minutes, hours;
        string period;
        
        // Getting the current time
        time_t total_seconds = time(0);
        struct tm* current_time = localtime(&total_seconds);

        seconds = current_time->tm_sec;
        minutes = current_time->tm_min;
        hours = current_time->tm_hour;
        
        // Converting to 12-hour format
        period = (hours >= 12) ? "PM" : "AM";
        hours = (hours > 12) ? hours - 12 : hours;

        // Displaying the time
        if (seconds == previous_second + 1 || (previous_second == 59 && seconds == 0)) {
            system("clear");
            cout << "\t\t-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << endl;
            cout << "\t\t\tWELCOME TO DIGITAL CLOCK SYSTEM!" << endl;
            cout << "\t\t-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << endl;
            cout << "\t\t\t\t" 
                 << (hours < 10 ? "0" : "") << hours << ":"
                 << (minutes < 10 ? "0" : "") << minutes << ":"
                 << (seconds < 10 ? "0" : "") << seconds << " " 
                 << period << endl;
        }

        previous_second = seconds;
    }

    return 0;
}

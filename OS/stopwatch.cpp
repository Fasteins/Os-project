#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <unistd.h>
#include <sys/shm.h>

using namespace std;

class Stopwatch {
public:
    Stopwatch() : start_time(std::chrono::high_resolution_clock::now()) {}

    void reset() {
        start_time = std::chrono::high_resolution_clock::now();
    }

    double get_elapsed_time() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start_time).count();
        return duration / 1e6;
    }

private:
    std::chrono::high_resolution_clock::time_point start_time;
};

void display_time(std::atomic<bool>& running, Stopwatch& stopwatch) {
    while (running.load()) {
        cout << "Elapsed time: " << stopwatch.get_elapsed_time() << " seconds" << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }
}

int main() {
    cout << "\t\t-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << endl;
    cout << "\t\t\tWELCOME TO THE STOPWATCH PROCESS!" << endl;
    cout << "\t\t-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << endl;

    // Shared memory access
    int* shared_memory;
    int shmid = shmget((key_t)1122, sizeof(int), 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    shared_memory = (int*)shmat(shmid, NULL, 0);
    if (shared_memory == (int*)-1) {
        perror("shmat failed");
        exit(1);
    }

    // Modify shared memory before stopwatch starts
    int new_data = *shared_memory - 11;
    *shared_memory = new_data;

    if (shmdt(shared_memory) == -1) {
        perror("shmdt error");
        exit(1);
    }

    // Stopwatch and threading logic
    Stopwatch stopwatch;
    atomic<bool> running(true);

    cout << "Stopwatch started. Press Enter to stop." << endl;

    // Launch a thread to display the elapsed time
    thread display_thread(display_time, ref(running), ref(stopwatch));

    // Wait for the user to press Enter to stop
    cin.get();

    running.store(false);
    display_thread.join();

    cout << "Stopwatch stopped. Final elapsed time: " << stopwatch.get_elapsed_time() << " seconds." << endl;
    sleep(2);

    // Modify shared memory after stopwatch stops
    shared_memory = (int*)shmat(shmid, NULL, 0);
    if (shared_memory == (int*)-1) {
        perror("shmat failed");
        exit(1);
    }

    new_data = *shared_memory + 11;
    *shared_memory = new_data;

    if (shmdt(shared_memory) == -1) {
        perror("shmdt error");
        exit(1);
    }

    return 0;
}

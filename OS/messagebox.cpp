#include <iostream>
#include <string>
#include <sys/shm.h>
using namespace std;

const int MAX_CONTACTS = 10;
const int MAX_INBOX = 10;

struct Message {
    string from;
    string body;
};

struct Contact {
    string username;
    Message inbox[MAX_INBOX];
    int inbox_count = 0;
};

Contact contact_list[MAX_CONTACTS];
int total_users = 0;

void registerUser(const string& username) {
    if (total_users < MAX_CONTACTS) {
        contact_list[total_users].username = username;
        contact_list[total_users].inbox_count = 0;
        total_users++;
    }
}

int getUserIndex(const string& username) {
    for (int i = 0; i < total_users; ++i) {
        if (contact_list[i].username == username) {
            return i;
        }
    }
    return -1;
}

void deliverMessage(const string& fromUser, const string& toUser, const string& msgContent) {
    int fromIdx = getUserIndex(fromUser);
    int toIdx = getUserIndex(toUser);

    if (fromIdx == -1 || toIdx == -1) {
        cout << "Error: Sender or recipient does not exist." << endl;
        return;
    }

    if (contact_list[toIdx].inbox_count < MAX_INBOX) {
        contact_list[toIdx].inbox[contact_list[toIdx].inbox_count].from = fromUser;
        contact_list[toIdx].inbox[contact_list[toIdx].inbox_count].body = msgContent;
        contact_list[toIdx].inbox_count++;
    } else {
        cout << "Recipient's inbox is full. Message not delivered." << endl;
    }
}

void showInbox(const string& username) {
    int userIdx = getUserIndex(username);
    if (userIdx == -1) {
        cout << "User not found in contact list." << endl;
        return;
    }

    cout << "\n--- Inbox of " << username << " ---" << endl;
    for (int i = 0; i < contact_list[userIdx].inbox_count; ++i) {
        cout << contact_list[userIdx].inbox[i].from << " says: " 
             << contact_list[userIdx].inbox[i].body << endl;
    }

    contact_list[userIdx].inbox_count = 0;
    cout << "End of messages.\n" << endl;
}

void updateSharedMemory(int delta) {
    int* shared_mem_ptr;
    int shm_id = shmget((key_t)1122, sizeof(int), 0666);
    if (shm_id == -1) {
        perror("Shared memory access failed");
        exit(1);
    }

    shared_mem_ptr = (int*)shmat(shm_id, NULL, 0);
    if (shared_mem_ptr == (void*)-1) {
        perror("Shared memory attach failed");
        exit(1);
    }

    *shared_mem_ptr += delta;

    if (shmdt(shared_mem_ptr) == -1) {
        perror("Shared memory detach failed");
        exit(1);
    }
}

int main() {
    // Decrease shared memory count on start
    updateSharedMemory(-30);

    // Register fixed contacts
    registerUser("kamla");
    registerUser("laiba");
    registerUser("ayesha");
    registerUser("saad");

    string inputCommand;

    while (true) {
        cout << "\nAvailable Contacts: kamla, laiba, ayesha, saad\n";
        cout << "Options: send | read | exit\n";
        cout << "Enter command: ";
        cin >> inputCommand;

        if (inputCommand == "send") {
            string sender, receiver, message;
            cout << "From: ";
            cin >> sender;
            cout << "To: ";
            cin >> receiver;
            cin.ignore();
            cout << "Message: ";
            getline(cin, message);
            deliverMessage(sender, receiver, message);
        }
        else if (inputCommand == "read") {
            string user;
            cout << "Username: ";
            cin >> user;
            showInbox(user);
        }
        else if (inputCommand == "exit") {
            updateSharedMemory(30);
            cout << "Session ended. Shared memory updated.\n";
            break;
        }
        else {
            cout << "Unknown command. Please try again.\n";
        }
    }

    return 0;
}

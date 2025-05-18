#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sys/shm.h>
using namespace std;

char board[9] = { '1', '2', '3', '4', '5', '6', '7', '8', '9' };

void displayBoard(int& p1, int& p2, int& p3, const string& playerName)
{
    cout << "      " << playerName << " =(X)        Computer=(O)" << endl;
    cout << "     _______________________________" << endl;
    cout << "     |         |         |         |" << endl;
    cout << "     |    " << board[0] << "    |    " << board[1] << "    |    " << board[2] << "    |" << endl;
    cout << "     |_________|_________|_________|" << endl;
    cout << "     |         |         |         |" << endl;
    cout << "     |    " << board[3] << "    |    " << board[4] << "    |    " << board[5] << "    |" << endl;
    cout << "     |_________|_________|_________|" << endl;
    cout << "     |         |         |         |" << endl;
    cout << "     |    " << board[6] << "    |    " << board[7] << "    |    " << board[8] << "    |" << endl;
    cout << "     |_________|_________|_________|" << endl;
    cout << "\n     " << playerName << " wins= " << p1 << "\tComputer wins= " << p2 << "\tDraw = " << p3 << endl;
}

int checkGameStatus()
{
    // Check rows, columns, and diagonals for a win
    if (board[0] == board[1] && board[1] == board[2] ||
        board[3] == board[4] && board[4] == board[5] ||
        board[6] == board[7] && board[7] == board[8] ||
        board[0] == board[3] && board[3] == board[6] ||
        board[1] == board[4] && board[4] == board[7] ||
        board[2] == board[5] && board[5] == board[8] ||
        board[0] == board[4] && board[4] == board[8] ||
        board[2] == board[4] && board[4] == board[6])
        return 0;

    // Check for a draw
    for (int i = 0; i < 9; ++i) {
        if (board[i] != 'X' && board[i] != 'O') return 1;
    }

    return -1;
}

int toss()
{
    return rand() % 2 + 1;
}

void updateSharedMemory(int change)
{
    int *shared_memory;
    int shmid = shmget((key_t)1122, sizeof(int), 0666);

    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    shared_memory = (int*)shmat(shmid, nullptr, 0);
    if (shared_memory == (int*)-1) {
        perror("shmat failed");
        exit(1);
    }

    int new_data = *shared_memory + change;
    *shared_memory = new_data;

    if (shmdt(shared_memory) == -1) {
        perror("shmdt error");
        exit(1);
    }
}

int main()
{
    cout << "\t\t-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << endl;
    cout << "\t\t\tWELCOME TO TIC TAC TOE WORLD!" << endl;
    cout << "\t\t-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << endl;

    string playerName;
    srand(time(0));
    char choice, pressed;
    int p1 = 0, p2 = 0, p3 = 0;
    bool continuePlaying = true;

    // Shared memory update before starting the game
    updateSharedMemory(-30);

    cout << "Welcome to the tic tac toe game\n\nIt is a single player game against the computer\n\nEnter your Name: ";
    cin >> playerName;

    while (continuePlaying) {
        system("CLS");
        int turn = toss();
        displayBoard(p1, p2, p3, playerName);

        cout << (turn == 1 ? "     " + playerName + " has won the toss and will mark first" : "     Computer has won the toss and will mark first") << endl;

        char mark = (turn == 1) ? 'X' : 'O';
        int j = 1;
        while (j == 1) {
            if (turn == 1) mark = 'X';
            else mark = 'O';

            char move;
            if (turn == 2) {
                // Computer's move (simplified logic for now)
                move = (rand() % 9) + '1';  // Random choice for now
            } else {
                cout << "\n     Enter the number of box in which you want to put your mark: ";
                cin >> move;
            }

            bool validMove = false;
            for (int i = 0; i < 9; ++i) {
                if (board[i] == move) {
                    board[i] = mark;
                    validMove = true;
                    break;
                }
            }

            if (!validMove) continue;

            system("CLS");
            displayBoard(p1, p2, p3, playerName);

            j = checkGameStatus();
            turn++;
        }

        if (j == 0) {
            if (turn - 1 == 1) {
                cout << "      " << playerName << " Wins" << endl;
                p1++;
            } else {
                cout << "       Computer Wins" << endl;
                p2++;
            }
        } else {
            cout << "     Game is Draw" << endl;
            p3++;
        }

        cout << "     If you want to play again, enter 'Y': ";
        cin >> choice;
        if (choice == 'Y') {
            // Reset board
            for (int i = 0; i < 9; ++i) {
                board[i] = '1' + i;
            }
        } else {
            continuePlaying = false;
        }
    }

    // Update shared memory after the game ends
    updateSharedMemory(30);

    cout << "Enter any key to quit: ";
    cin >> pressed;
    system("pause");

    return 0;
}

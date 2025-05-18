#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <sys/shm.h>
#include <limits> // For input validation
using namespace std;

const string BORDER = "\t\t================================================";
const int MAX_WRONG = 8; // maximum number of incorrect guesses allowed

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

class HangmanGame {
private:
    vector<string> words; // collection of possible words
    string current_word;  // word to guess
    string guessed_word;  // word guessed so far
    string used_letters;  // letters already guessed
    int wrong_guesses;    // number of incorrect guesses
    
    void initializeWords() {
        words = {
            "HELLO", "WORLD", "HANGMAN", "COMPUTER",
            "SCIENCE", "PROGRAMMING", "CPLUSPLUS", "DEVELOPER",
            "ALGORITHM", "FUNCTION", "VARIABLE", "CONSTANT"
        };
    }
    
    void selectRandomWord() {
        current_word = words[rand() % words.size()];
        guessed_word = string(current_word.length(), '-');
        wrong_guesses = 0;
        used_letters.clear();
    }

public:
    HangmanGame() {
        srand(static_cast<unsigned int>(time(0)));
        initializeWords();
    }

    void displayGameStatus() const {
        cout << "\nIncorrect guesses remaining: " << MAX_WRONG - wrong_guesses << endl;
        cout << "Letters used: ";
        for (char c : used_letters) {
            cout << c << ' ';
        }
        cout << "\nWord: " << guessed_word << endl;
    }

    void getPlayerGuess() {
        char guess;
        while (true) {
            cout << "\nEnter your guess: ";
            cin >> guess;
            guess = toupper(guess);
            
            if (!isalpha(guess)) {
                cout << "Please enter a valid letter (A-Z).\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            } else if (used_letters.find(guess) != string::npos) {
                cout << "You've already guessed that letter. Try again.\n";
            } else {
                used_letters += guess;
                processGuess(guess);
                break;
            }
        }
    }

    void processGuess(char guess) {
        if (current_word.find(guess) != string::npos) {
            cout << "Correct! '" << guess << "' is in the word.\n";
            for (size_t i = 0; i < current_word.length(); ++i) {
                if (current_word[i] == guess) {
                    guessed_word[i] = guess;
                }
            }
        } else {
            cout << "Sorry, '" << guess << "' is not in the word.\n";
            ++wrong_guesses;
        }
    }

    bool isGameOver() const {
        return wrong_guesses >= MAX_WRONG || guessed_word == current_word;
    }

    void displayResult() const {
        if (guessed_word == current_word) {
            cout << "\nCongratulations! You guessed the word: " << current_word << endl;
        } else {
            cout << "\nGame over! The word was: " << current_word << endl;
        }
    }

    void play() {
        selectRandomWord();
        while (!isGameOver()) {
            displayGameStatus();
            getPlayerGuess();
        }
        displayResult();
    }
};

void displayHeader() {
    cout << BORDER << endl;
    cout << "\t\t\tHANGMAN GAME WORLD" << endl;
    cout << BORDER << endl << endl;
    cout << "Guess the word letter by letter before you run out of attempts!\n";
    cout << "You can make up to " << MAX_WRONG << " wrong guesses.\n\n";
}

bool askToPlayAgain() {
    char response;
    while (true) {
        cout << "\nWould you like to play again? (y/n): ";
        cin >> response;
        response = tolower(response);
        if (response == 'y' || response == 'n') {
            return response == 'y';
        }
        cout << "Please enter 'y' or 'n'.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

int main() {
    displayHeader();
    updateSharedMemory(-15); // Initialize shared memory

    HangmanGame game;
    bool play_again = true;
    
    while (play_again) {
        game.play();
        play_again = askToPlayAgain();
    }

    updateSharedMemory(15); // Clean up shared memory
    cout << BORDER << endl;
    cout << "\t\tThanks for playing Hangman!" << endl;
    cout << BORDER << endl;
    
    return 0;
}
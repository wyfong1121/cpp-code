#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <cstring>

using namespace std;

// Constants
const int sizeofeasyword = 7;  // Length of easy words
const int sizeofmidword = 9;   // Length of medium words
const int sizeofdiffword = 10; // Length of difficult words
const int sizeoflists = 50;
const int maxUsers = 100;

// Variables
int difficulty = 1;
int found = 0;  // Flag for checking if guess is successful
string easywordslist[sizeoflists];
string midwordslist[sizeoflists];
string diffwordslist[sizeoflists];
string username;
int userCount = 0;

// Struct for user statistics
struct User {
    string username;
    int gamesPlayed;
    int gamesWon;
    int gamesLost;
    double totalCompletionTime;
};

// Array to store users
User users[maxUsers];

// Function to load user data from file
void loadUsers() {
    ifstream infile("users.txt");
    if (!infile) {
        cerr << "Failed to open users.txt" << endl;
        return;
    }

    while (infile >> users[userCount].username 
                  >> users[userCount].gamesPlayed 
                  >> users[userCount].gamesWon 
                  >> users[userCount].gamesLost 
                  >> users[userCount].totalCompletionTime) {
        userCount++;
    }

    infile.close();
}

// Function to save user data to file
void saveUsers() {
    ofstream outfile("users.txt");
    if (!outfile) {
        cerr << "Failed to open users.txt for writing" << endl;
        return;
    }

    for (int i = 0; i < userCount; i++) {
        outfile << users[i].username << " " 
                << users[i].gamesPlayed << " " 
                << users[i].gamesWon << " " 
                << users[i].gamesLost << " " 
                << users[i].totalCompletionTime << endl;
    }

    outfile.close();
}

// Function to find a user by username
int findUser(string uname) {
    for (int i = 0; i < userCount; i++) {
        if (users[i].username == uname) {
            return i;
        }
    }
    return -1; // User not found
}

// Function for user registration and login
bool loginUser() {
    cout << "Enter your username: ";
    cin >> username;

    int userIndex = findUser(username);
    if (userIndex != -1) {
        cout << "Welcome back, " << username << "!" << endl;
        return true;
    }
    else {
        if (userCount >= maxUsers) {
            cout << "User limit reached. Cannot register new user." << endl;
            return false;
        }
        
        cout << "Username not found. Would you like to register? (y/n): ";
        char choice;
        cin >> choice;

        // Validate the choice input
        while (choice != 'y' && choice != 'Y' && choice != 'n' && choice != 'N') {
            cout << "Invalid choice. Please enter 'y' or 'n': ";
            cin >> choice;
        }

        if (choice == 'y' || choice == 'Y') {
            users[userCount].username = username;
            users[userCount].gamesPlayed = 0;
            users[userCount].gamesWon = 0;
            users[userCount].gamesLost = 0;
            users[userCount].totalCompletionTime = 0.0;
            userCount++;
            cout << "User registered successfully. Welcome, " << username << "!" << endl;
            return true;
        }
        else {
            return false;
        }
    }
}

// Function to report game statistics
void report(double completionTime, int chance, int userIndex) {
    cout << "Completion time: " << completionTime << " seconds" << endl;
    cout << "Errors made: " << (5 - chance) << endl;
}

// Function to play the hangman game
void hangmanGame(int userIndex) {
    int chance;  // Number of lives left
    char key[20]; // Key
    char show[20] = "";
    char ans;
    int sizeofword;

    clock_t startTime = clock();
    cout << "Level: " << difficulty << endl;

    // Set variables depending on difficulty
    if (difficulty == 1) {
        strcpy(key, easywordslist[rand() % sizeoflists].c_str());
        sizeofword = sizeofeasyword;
        chance = 10;
    } else if (difficulty == 2) {
        strcpy(key, midwordslist[rand() % sizeoflists].c_str());
        sizeofword = sizeofmidword;
        chance = 7;
    } else if (difficulty == 3) {
        strcpy(key, diffwordslist[rand() % sizeoflists].c_str());
        sizeofword = sizeofdiffword;
        chance = 5;
    }

    // Initialize show array with underscores
    for (int i = 0; i < sizeofword; i++) {
        show[i] = '_';
    }

    // Game logic
    do {
        cout << "Lives: " << chance << endl;
        cout << "Question: ";

        for (int i = 0; i < sizeofword; i++) {
            cout << show[i] << " ";
        }
        cout << endl << endl;

        cout << "Enter the letter you guess: ";
        cin >> ans;
        ans = tolower(ans);  // Convert to lowercase for case insensitivity
        cout << endl;

        bool correctGuess = false;
        for (int i = 0; i < sizeofword; i++) {
            if (ans == key[i]) {
                show[i] = ans;
                correctGuess = true;
            }
        }

        if (correctGuess) {
            found++;
        } else {
            chance--;
        }

        if (strcmp(key, show) == 0) {
            cout << "Congratulations! You win!" << endl << "Correct answer: " << key << endl;
            users[userIndex].gamesWon++;
            if (difficulty < 3) {
                difficulty++;
            } else {
                difficulty = 0;  // End the game if the highest difficulty is reached
            }
            break;
        }

    } while (chance > 0);

    if (chance == 0) {
        cout << "Game over. You lose." << endl;
        users[userIndex].gamesLost++;
    }

    // Show the correct answer
    cout << "Correct answer: " << key << endl;

    clock_t endTime = clock();
    double completionTime = static_cast<double>(endTime - startTime) / CLOCKS_PER_SEC;
    users[userIndex].totalCompletionTime += completionTime;
    users[userIndex].gamesPlayed++;
    report(completionTime, chance, userIndex);
}

// Function to display admin statistics
void adminView() {
    cout << "Admin View: User Statistics" << endl;
    for (int i = 0; i < userCount; i++) {
        cout << "Username: " << users[i].username << endl;
        cout << "Games Played: " << users[i].gamesPlayed << endl;
        cout << "Games Won: " << users[i].gamesWon << endl;
        cout << "Games Lost: " << users[i].gamesLost << endl;
        cout << "Total Completion Time: " << users[i].totalCompletionTime << " seconds" << endl;
        cout << "-----------------------------------" << endl;
    }
}

int main(int argc, char *argv[]) {
    // Load users from the file
    loadUsers();

    // Admin login
    cout << "Are you an admin? (y/n): ";
    char isAdmin;
    cin >> isAdmin;
	// Data validation for admin login
	while (tolower(isAdmin) != 'y' && tolower(isAdmin) != 'n') {
	    cout << "Invalid input. Please enter 'y' or 'n': ";
		cin >> isAdmin;
	}
    if (isAdmin == 'y' || isAdmin == 'Y') {
        adminView();
        return 0;
    }

    // User login
    while (!loginUser()) {
        cout << "Please try again." << endl;
    }

    int userIndex = findUser(username);
    if (userIndex == -1) {
        cout << "Something went wrong with user login." << endl;
        return 1;
    }

    ifstream easywords("easywords.txt");
    ifstream midwords("midwords.txt");
    ifstream diffwords("diffwords.txt");

    if (!easywords) {
        cout << "Failed to open easywords.txt" << endl;
        return 1;
    }

    if (!midwords) {
        cout << "Failed to open midwords.txt" << endl;
        return 1;
    }

    if (!diffwords) {
        cout << "Failed to open diffwords.txt" << endl;
        return 1;
    }

    srand(time(0));
    for (int i = 0; i < sizeoflists; i++) {
        getline(easywords, easywordslist[i]);
        getline(midwords, midwordslist[i]);
        getline(diffwords, diffwordslist[i]);
    }

    // Start the progressive game
    while (difficulty != 0) {
        hangmanGame(userIndex);
    }

    // Save users to the file
    saveUsers();

    return 0;
}

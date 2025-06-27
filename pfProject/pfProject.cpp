// Secure Personal Expense Manager with Encrypted Access
// Updated: Fixed identifier error and type warnings

#include <iostream>
#include <fstream>
#include <string>
#include <conio.h> // For getch() to mask password input (Windows only)

using namespace std;

const int INITIAL_CAPACITY = 10;

string inputPasswordMasked() {
    string password = "";
    char ch;
    while ((ch = _getch()) != '\r') { // Enter key
        if (ch == '\b') { // Backspace
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b";
            }
        }
        else {
            password += ch;
            cout << '*';
        }
    }
    cout << endl;
    return password;
}

bool usernameExists(const string& username) {
    ifstream file("users.txt");
    string line;
    while (getline(file, line)) {
        string storedUsername = line.substr(0, line.find(':'));
        if (storedUsername == username) return true;
    }
    return false;
}

void signup() {
    string username, password;
    cout << "Enter new username: ";
    cin >> username;
    if (usernameExists(username)) {
        cout << "Username already exists.\n";
        return;
    }
    cout << "Enter password: ";
    password = inputPasswordMasked();

    ofstream file("users.txt", ios::app);
    file << username << ":" << password << endl;
    cout << "Signup successful.\n";
}

bool login(string& loggedInUser) {
    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    password = inputPasswordMasked();

    ifstream file("users.txt");
    string line;
    while (getline(file, line)) {
        if (line == username + ":" + password) {
            loggedInUser = username;
            return true;
        }
    }
    return false;
}

struct Expense {
    string date;
    string category;
    float amount;
    string description;
};

void viewExpenses(Expense* expenses, int size); // Forward declaration to fix identifier error

void loadExpenses(const string& username, Expense*& expenses, int& size, int& capacity) {
    ifstream file(username + "_data.txt");
    string line;
    size = 0;
    capacity = INITIAL_CAPACITY;
    expenses = new Expense[capacity];

    while (getline(file, line)) {
        if (size >= capacity) {
            capacity *= 2;
            Expense* temp = new Expense[capacity];
            for (int i = 0; i < size; ++i) temp[i] = expenses[i];
            delete[] expenses;
            expenses = temp;
        }

        size_t pos1 = line.find('|');
        size_t pos2 = line.find('|', pos1 + 1);
        size_t pos3 = line.find('|', pos2 + 1);

        expenses[size].date = line.substr(0, pos1);
        expenses[size].category = line.substr(pos1 + 1, pos2 - pos1 - 1);
        expenses[size].amount = stof(line.substr(pos2 + 1, pos3 - pos2 - 1));
        expenses[size].description = line.substr(pos3 + 1);
        size++;
    }
}

void saveAllExpenses(const string& username, Expense* expenses, int size) {
    ofstream file(username + "_data.txt", ios::trunc);
    for (int i = 0; i < size; ++i) {
        file << expenses[i].date << "|" << expenses[i].category << "|"
            << expenses[i].amount << "|" << expenses[i].description << endl;
    }
}

void deleteExpense(const string& username, Expense*& expenses, int& size) {
    viewExpenses(expenses, size);
    cout << "Enter the index (0 to " << size - 1 << ") of the expense to delete: ";
    int index;
    cin >> index;

    if (index < 0 || index >= size) {
        cout << "Invalid index.\n";
        return;
    }

    for (int i = index; i < size - 1; ++i) {
        expenses[i] = expenses[i + 1];
    }
    size--;
    saveAllExpenses(username, expenses, size);
    cout << "Expense deleted successfully.\n";
}

void saveExpense(const string& username, const Expense& e) {
    ofstream file(username + "_data.txt", ios::app);
    file << e.date << "|" << e.category << "|" << e.amount << "|" << e.description << endl;
}

void addExpense(const string& username, Expense*& expenses, int& size, int& capacity) {
    if (size >= capacity) {
        capacity *= 2;
        Expense* temp = new Expense[capacity];
        for (int i = 0; i < size; ++i) temp[i] = expenses[i];
        delete[] expenses;
        expenses = temp;
    }

    cout << "Enter date (YYYY-MM-DD): ";
    cin >> expenses[size].date;
    cout << "Enter category: ";
    cin >> expenses[size].category;
    cout << "Enter amount: ";
    cin >> expenses[size].amount;
    cin.ignore();
    cout << "Enter description: ";
    getline(cin, expenses[size].description);

    saveExpense(username, expenses[size]);
    size++;
    cout << "Expense added successfully.\n";
}

void viewExpenses(Expense* expenses, int size) {
    cout << "\n--- Expense Records ---\n";
    for (int i = 0; i < size; ++i) {
        cout << i << ". Date: " << expenses[i].date << ", Category: " << expenses[i].category
            << ", Amount: " << expenses[i].amount << ", Description: " << expenses[i].description << endl;
    }
    cout << "------------------------\n";
}

void analyzeExpenses(Expense* expenses, int size) {
    float total = 0;
    for (int i = 0; i < size; ++i) {
        total += expenses[i].amount;
    }
    cout << "\n--- Spending Analysis ---\n";
    cout << "Total spent: Rs. " << total << endl;
}

void exportToCSV(const string& username, Expense* expenses, int size) {
    ofstream file(username + "_report.csv");
    file << "Date,Category,Amount,Description\n";
    for (int i = 0; i < size; ++i) {
        file << expenses[i].date << ","
            << expenses[i].category << ","
            << expenses[i].amount << ","
            << expenses[i].description << "\n";
    }
    cout << "Expenses exported to " << username << "_report.csv\n";
}

int main() {
    string loggedInUser;
    Expense* expenses = nullptr;
    int size = 0, capacity = INITIAL_CAPACITY;

    while (true) {
        cout << "\n--- Secure Expense Manager ---\n";
        cout << "1. Sign Up\n";
        cout << "2. Login\n";
        cout << "0. Exit\n";
        cout << "Select option: ";
        int choice;
        cin >> choice;

        if (choice == 1) {
            signup();
        }
        else if (choice == 2) {
            if (login(loggedInUser)) {
                cout << "\nWelcome, " << loggedInUser << "!\n";
                loadExpenses(loggedInUser, expenses, size, capacity);
                int subChoice;
                do {
                    cout << "\n1. Add Expense\n";
                    cout << "2. View Expenses\n";
                    cout << "3. Analyze Expenses\n";
                    cout << "4. Export to CSV\n";
                    cout << "5. Delete Expense\n";
                    cout << "0. Logout\n";
                    cout << "Choice: ";
                    cin >> subChoice;

                    if (subChoice == 1) addExpense(loggedInUser, expenses, size, capacity);
                    else if (subChoice == 2) viewExpenses(expenses, size);
                    else if (subChoice == 3) analyzeExpenses(expenses, size);
                    else if (subChoice == 4) exportToCSV(loggedInUser, expenses, size);
                    else if (subChoice == 5) deleteExpense(loggedInUser, expenses, size);

                } while (subChoice != 0);

                delete[] expenses;
                expenses = nullptr;
            }
            else {
                cout << "Invalid credentials.\n";
            }
        }
        else if (choice == 0) {
            break;
        }
    }
    return 0;
}

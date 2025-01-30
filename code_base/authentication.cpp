#include <vector>
#include <limits>
#include <string>
#include <iostream>
#include "sqlite3.h"
#include <cstdint>
#include <conio.h>
#include <chrono>
#include <ctime>
#include <thread>
#include <map>
#include <cstdint>
#include "c_time.h"
#include "authentication.h"

using namespace std;
int m = 1;
// Function to hash a password using SHA-256
uint64_t customHash(const string& str) {
    const uint64_t seed = 0xdeadbeef;
    const uint64_t fnv_prime = 1099511628211ULL;
    uint64_t hash = seed;

    for (char c : str) {
        hash ^= c;
        hash *= fnv_prime;
    }

    return hash;
}

string hashPassword(const string& password_to_be_hashed) {
    uint64_t hash = customHash(password_to_be_hashed);
    return to_string(hash);
}

string getPassword() {
    string pass;
    char ch;
    while ((ch = _getch()) != '\r') { // '\r' is the Enter key
        if (ch == '\b') { // '\b' is the Backspace key
            if (!pass.empty()) {
                cout << "\b \b"; // Move cursor back, erase character, move cursor back again
                pass.pop_back(); // Remove last character from password
            }
        }
        else {
            pass += ch;
            cout << '*'; // Display star instead of character
        }
    }
    cout << endl; // Move to next line
    return pass;
}
std::string signup(sqlite3* db) {
    std::string email, name, password, city;
    int age;
    std::cout << "Enter email: ";
    std::cin >> email;
    std::cout << "Enter password: ";
    password = hashPassword(getPassword()); // Hash the password
    std::cout << "Enter name: ";
    std::cin >> name;
    std::cout << "Enter age: ";
    std::cin >> age;
    std::cout << "Enter city: ";
    std::cin >> city;

    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::string currentDateStr = timeToString(now);

    std::string insertSQL = "INSERT INTO players (email, password, name, age, city, current_date) "
        "VALUES ('" + email + "', '" + password + "', '" + name + "', "
        + std::to_string(age) + ", '" + city + "', '" + currentDateStr + "')";
    char* errMsg = nullptr;
    int status = sqlite3_exec(db, insertSQL.c_str(), nullptr, nullptr, &errMsg);

    if (status != SQLITE_OK) {
        std::cerr << "Error inserting data: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return "";
    }

    std::cout << "Signup successful!" << std::endl;
    return email; // Return the email upon successful signup
}
std::string login(sqlite3* db) {
    std::string email, password;
    std::cout << "Enter email: ";
    std::cin >> email;
    pass_fail: std::cout << "Enter password: ";
    password = hashPassword(getPassword());

    std::string sql = "SELECT password, last_login_date FROM players WHERE email='" + email + "';";
    sqlite3_stmt* stmt;
    char* errMsg = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
        return "";
    }

    std::string resultEmail = ""; // Default to empty string in case of failure

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* dbPassword = sqlite3_column_text(stmt, 0);
        std::string dbPasswordStr(reinterpret_cast<const char*>(dbPassword));

        if (password == dbPasswordStr) { // Password matches
            std::cout << "Login successful" << std::endl;
            resultEmail = email; // Set the returned email

            const unsigned char* lastLoginDate = sqlite3_column_text(stmt, 1);
            if (lastLoginDate != nullptr) {
                std::cout << "Last login date: " << reinterpret_cast<const char*>(lastLoginDate) << std::endl;
            }
            else {
                std::cout << "This is your first login!" << std::endl;
            }

            // Update the last login date
            std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
            std::string lastLoginDateStr = timeToString(now);

            std::string updateSQL = "UPDATE players SET last_login_date = '" + lastLoginDateStr + "' WHERE email = '" + email + "';";
            int updateStatus = sqlite3_exec(db, updateSQL.c_str(), nullptr, nullptr, &errMsg);

            if (updateStatus != SQLITE_OK) {
                std::cerr << "Error updating last login date: " << errMsg << std::endl;
                sqlite3_free(errMsg);
            }
        }
        else {
            std::cerr << "Incorrect password" << std::endl;
            while (1)
            {
                m++;
                if (m > 3)
                    return "problem";
                goto pass_fail;
                
            }
        }
    }
    else {
        std::cerr << "User not found" << std::endl;
    }

    sqlite3_finalize(stmt);
    return resultEmail; // Return the email of the logged-in user or empty string on failure
}
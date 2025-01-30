#include <iostream>
#include <string>
#include "sqlite3.h"

using namespace std;

//int main()
//{
//    // Open SQLite database connection
//    sqlite3* db;
//    int status = sqlite3_open("tictactoe26.db", &db);
//    if (status != SQLITE_OK) {
//        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
//        return 1;
//    }
//    char* errorMessage = nullptr;
//
//    //create players table
//    const char* createTableSQL = "CREATE TABLE players (id INTEGER PRIMARY KEY, email TEXT UNIQUE, password TEXT, name TEXT, city TEXT, age INTEGER, pvp_win_count INTEGER DEFAULT 0, pvp_lose_count INTEGER DEFAULT 0,pvp_total_games INTEGER DEFAULT 0, pve_win_count INTEGER DEFAULT 0, pve_lose_count INTEGER DEFAULT 0, pve_total_games INTEGER DEFAULT 0, total_games INTEGER DEFAULT 0, current_date TEXT, last_login_date TEXT)";
//    
//    status = sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errorMessage);
//    if (status != SQLITE_OK) {
//        std::cerr << "Error creating table: " << errorMessage << std::endl;
//        sqlite3_free(errorMessage);
//        sqlite3_close(db);
//        return 1;
//    }
//    cout << "Table created successfully\n";
//
//     //Create a table to store game data if not exists
//    const char* sql = "CREATE TABLE IF NOT EXISTS games (id INTEGER PRIMARY KEY AUTOINCREMENT, player1_email TEXT NOT NULL, player2_email TEXT NOT NULL, date TEXT NOT NULL,game_mode INTEGER NOT NULL)";
//    char* errMsg;
//    status = sqlite3_exec(db, sql, 0, 0, &errMsg);
//    if (status != SQLITE_OK) {
//        cerr << "SQL error: " << errMsg << endl;
//        sqlite3_free(errMsg);
//    }
//    cout << "Table created successfully\n";
//
//    const char* createTableSQL1 = "CREATE TABLE IF NOT EXISTS moves (id INTEGER PRIMARY KEY AUTOINCREMENT, game_id INTEGER NOT NULL, board TEXT NOT NULL, player_turn TEXT NOT NULL, move_number INTEGER NOT NULL, FOREIGN KEY(game_id) REFERENCES games(id))";
//    char* errorMessage1 = nullptr;
//    status = sqlite3_exec(db, createTableSQL1, nullptr, nullptr, &errorMessage1);
//    if (status != SQLITE_OK) {
//        std::cerr << "Error creating table: " << errorMessage1 << std::endl;
//        sqlite3_free(errorMessage1);
//        sqlite3_close(db);
//        return 1;
//    }
//    cout << "Table created successfully\n"; 
//
//    return 0;
//}

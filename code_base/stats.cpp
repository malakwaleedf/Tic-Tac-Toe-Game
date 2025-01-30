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
#include "stats.h"
using namespace std;


// Retrieve player stats including PvP and PvE, and ensure correct total games count
// Retrieve player stats, ensuring proper handling of both PvP and PvE stats
bool getPlayerStats(sqlite3* db, const std::string& email,
    int& pvp_win_count, int& pvp_lose_count, int& pvp_total_games,
    int& pve_win_count, int& pve_lose_count, int& pve_total_games, int& total_games) {
    std::string sql = "SELECT pvp_win_count, pvp_lose_count, pvp_total_games, "
        "pve_win_count, pve_lose_count, pve_total_games, total_games "
        "FROM players WHERE email='" + email + "';";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing SQL query: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    bool recordFound = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) { // If a record is found
        recordFound = true;
        pvp_win_count = sqlite3_column_int(stmt, 0);
        pvp_lose_count = sqlite3_column_int(stmt, 1);
        pvp_total_games = sqlite3_column_int(stmt, 2);
        pve_win_count = sqlite3_column_int(stmt, 3);
        pve_lose_count = sqlite3_column_int(stmt, 4);
        pve_total_games = sqlite3_column_int(stmt, 5);
        total_games = sqlite3_column_int(stmt, 6);
    }

    if (!recordFound) { // If no record is found, raise a warning
        std::cerr << "No record found for email: " << email << std::endl;
        pvp_win_count = 0;
        pvp_lose_count = 0;
        pvp_total_games = 0;
        pve_win_count = 0;
        pve_lose_count = 0;
        pve_total_games = 0;
    }

    sqlite3_finalize(stmt); // Finalize the prepared statement
    return recordFound;
}

// Update player stats, ensuring proper total games count
void updatePlayerStats(sqlite3* db, const std::string& email,
    int pvp_win_count, int pvp_lose_count, int pvp_total_games,
    int pve_win_count, int pve_lose_count, int pve_total_games) {
    if (db == nullptr) {
        std::cerr << "Database connection is not established." << std::endl;
        return;
    }

    // Calculate total games as the sum of PvP and PvE total games
    int total_games = pvp_total_games + pve_total_games;

    std::string updateSQL =
        "UPDATE players SET "
        "pvp_win_count = " + std::to_string(pvp_win_count) +
        ", pvp_lose_count = " + std::to_string(pvp_lose_count) +
        ", pvp_total_games = " + std::to_string(pvp_total_games) +
        ", pve_win_count = " + std::to_string(pve_win_count) +
        ", pve_lose_count = " + std::to_string(pve_lose_count) +
        ", pve_total_games = " + std::to_string(pve_total_games) +
        ", total_games = " + std::to_string(total_games) +
        " WHERE email = '" + email + "';";

    char* errMsg = nullptr;
    int status = sqlite3_exec(db, updateSQL.c_str(), nullptr, nullptr, &errMsg);

    if (status != SQLITE_OK) {
        std::cerr << "Error updating player stats: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
    else {
        std::cout << "Player stats updated successfully for " << email << std::endl;
    }
}
void handleGameOutcome(sqlite3* db, const std::string& player1Email,
    const std::string& player2Email, int game_result, int gameMode) {
    int pvp_win_count_1, pvp_lose_count_1, pvp_total_games_1;
    int pvp_win_count_2, pvp_lose_count_2, pvp_total_games_2;
    int pve_win_count_1, pve_lose_count_1, pve_total_games_1;
    int pve_win_count_2, pve_lose_count_2, pve_total_games_2;
    int t;
    // Retrieve existing stats for Player 1
    if (!getPlayerStats(db, player1Email, pvp_win_count_1, pvp_lose_count_1, pvp_total_games_1, pve_win_count_1, pve_lose_count_1, pve_total_games_1, t)) {
        std::cerr << "Failed to get player stats for " << player1Email << std::endl;
        return;
    }

    if (gameMode == 1) { // Player vs. Player
        // Retrieve existing stats for Player 2
        if (!getPlayerStats(db, player2Email, pvp_win_count_2, pvp_lose_count_2, pvp_total_games_2, pve_win_count_2, pve_lose_count_2, pve_total_games_2, t)) {
            std::cerr << "Failed to get player stats for " << player2Email << std::endl;
            return;
        }

        if (game_result == 1) { // Player 1 wins
            pvp_win_count_1++;
            pvp_lose_count_2++;
        }
        else if (game_result == -1) { // Player 2 wins
            pvp_lose_count_1++;
            pvp_win_count_2++;
        }
        //else if (game_result == 2) { // Tie
        //    // No changes in win or lose counts
        //}

        // Increment total games for both players
        pvp_total_games_1++;
        pvp_total_games_2++;

        // Update player stats
        updatePlayerStats(db, player1Email, pvp_win_count_1, pvp_lose_count_1, pvp_total_games_1, pve_win_count_1, pve_lose_count_1, pve_total_games_1);
        updatePlayerStats(db, player2Email, pvp_win_count_2, pvp_lose_count_2, pvp_total_games_2, pve_win_count_2, pve_lose_count_2, pve_total_games_2);
    }
    else if (gameMode == 2) { // Player vs. AI
        if (game_result == 1) { // Player 1 wins against AI
            pve_win_count_1++;
            pve_total_games_1++;
        }
        else if (game_result == -1) { // AI wins
            pve_lose_count_1++;
            pve_total_games_1++;
        }
        else if (game_result == 2) { // Tie with AI
            pve_total_games_1++;
        }

        // Update stats for Player 1 for both PvP and PvE
        updatePlayerStats(db, player1Email, pvp_win_count_1, pvp_lose_count_1, pvp_total_games_1, pve_win_count_1, pve_lose_count_1, pve_total_games_1);
    }
}
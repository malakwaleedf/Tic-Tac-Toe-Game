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
#include "authentication.h"
#include "c_time.h"
#include "stats.h"
#include "game_prep.h"
#include "save_game.h"
#include "history_handle.h"

using namespace std;

void saveMove(int gameId, const GameBoard& board, const std::string& playerTurn, int moveNumber, sqlite3* db) {
    char boardStr[10];
    int index = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++)
        {
            if (board.board[i][j] == 1) {
                boardStr[index] = 'X';
            }
            else if (board.board[i][j] == -1) {
                boardStr[index] = 'O';
            }
            else {
                boardStr[index] = '-';
            }
            index++;
        }
    }
    boardStr[9] = '\0';

    std::string insertMoveSQL = "INSERT INTO moves (game_id, board, player_turn, move_number) "
        "VALUES (" + std::to_string(gameId) + ", '" + boardStr + "', '" + playerTurn + "', " + std::to_string(moveNumber) + ")";
    char* errMsg = nullptr;
    int status = sqlite3_exec(db, insertMoveSQL.c_str(), nullptr, nullptr, &errMsg);
    if (status != SQLITE_OK) {
        std::cerr << "Error inserting move: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

void showBoard(const char boardStr[]) {
    cout << "Current Board:" << endl;
    cout << " 1 2 3 " << endl;
    cout << "-------" << endl;
    int index = 0;

    for (int i = 0; i < 3; i++) {
        cout << i + 1 << "|";
        for (int j = 0; j < 3; j++)
        {
            cout << boardStr[index] << " ";
            index++;
        }
        cout << endl;
    }
    cout << " -------" << endl;
}

void showMoveByMove(int gameId, const std::string& playerEmail, sqlite3* db) {
    std::string querySQL = "SELECT board, player_turn FROM moves WHERE game_id = ? AND (player_turn = ? OR player_turn = (SELECT player2_email FROM games WHERE id = ?)) ORDER BY move_number";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, querySQL.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, gameId);
    sqlite3_bind_text(stmt, 2, playerEmail.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, gameId);

    bool firstMove = true;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const unsigned char* boardStr = sqlite3_column_text(stmt, 0);
        const unsigned char* playerTurn = sqlite3_column_text(stmt, 1);
        if (boardStr && playerTurn) {
            std::string player(reinterpret_cast<const char*>(playerTurn));
            if (firstMove) {
                std::cout << "Press any key to start showing moves of Game " << gameId << ":" << std::endl;
                std::cin.get();
                firstMove = false;
            }
            showBoard(reinterpret_cast<const char*>(boardStr));
            std::cout << "Player's move: " << player << std::endl;
            std::cout << std::endl;
            std::cout << "Press any key to show the next move:" << std::endl;
            std::cin.get();
        }
    }

    if (firstMove) {
        std::cout << "No moves found for game " << gameId << "." << std::endl;
    }

    sqlite3_finalize(stmt);
}


std::vector<int> getPlayerGameIds(const std::string& playerEmail, sqlite3* db) {
    std::vector<int> gameIds;
    std::string querySQL = "SELECT id FROM games WHERE player1_email = ? OR player2_email = ?";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, querySQL.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        return gameIds;
    }

    sqlite3_bind_text(stmt, 1, playerEmail.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, playerEmail.c_str(), -1, SQLITE_STATIC);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int gameId = sqlite3_column_int(stmt, 0);
        gameIds.push_back(gameId);
    }

    sqlite3_finalize(stmt);
    return gameIds;
}

void showFinalMove(int gameId, const std::string& playerEmail, sqlite3* db) {
    std::string querySQL = "SELECT board, player_turn FROM moves WHERE game_id = ? ORDER BY move_number DESC LIMIT 1";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, querySQL.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, gameId);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const unsigned char* boardStr = sqlite3_column_text(stmt, 0);
        const unsigned char* playerTurn = sqlite3_column_text(stmt, 1);
        if (boardStr && playerTurn) {
            std::string boardString(reinterpret_cast<const char*>(boardStr));
            std::string player(reinterpret_cast<const char*>(playerTurn));
            showBoard(reinterpret_cast<const char*>(boardStr));
            std::cout << "Player's move: " << player << std::endl;
        }
    }
    else {
        std::cout << "No moves found for game " << gameId << "." << std::endl;
    }

    sqlite3_finalize(stmt);
}
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
#include "history_handle.h"

using namespace std;

std::string player1Email, player2Email;

class Game {
private:
    Player* player1;
    Player* player2;
    sqlite3* db;
    std::string player1Email;
    std::string player2Email;
    vector<vector<GameBoard>> allGames;
    int gameCounter;
    int gameMode;
    int move_num = 0;
    string players_move_email;

public:
    Game(Player* p1, Player* p2, sqlite3* dbConn, const std::string& email1, const std::string& email2, int mode)
        : player1(p1), player2(p2), db(dbConn), player1Email(email1), player2Email(email2), gameMode(mode) {}

    void play() {
        cout << "Welcome to Tic Tac Toe!" << endl;

        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::string currentDateStr = timeToString(now);

        std::string insertSQL = "INSERT INTO games (player1_email, player2_email, date, game_mode) "
            "VALUES ('" + player1Email + "', '" + player2Email + "', '" + currentDateStr + "', " + std::to_string(gameMode) + ")";
        char* errMsg = nullptr;
        int status = sqlite3_exec(db, insertSQL.c_str(), nullptr, nullptr, &errMsg);

        if (status != SQLITE_OK) {
            std::cerr << "Error inserting game data: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            return;
        }
        int lastGameId = sqlite3_last_insert_rowid(db);

        if (gameMode == 2) {
            cout << "You will be playing against an unbeatable AI." << endl;
            cout << "You will be X and the AI will be O. Let's begin!" << endl;
        }
        else if (gameMode == 1) {
            cout << "You will be playing against each other." << endl;
            cout << "Player 1 will be X and Player 2 will be O." << endl;
        }

        bool playAgain = true;
        while (playAgain) {
            GameBoard board;
            vector<GameBoard> moves;
            bool player1Turn = true;

            while (true) {
                move_num++;
                board.display();

                if (player1Turn) {
                    player1->makeMove(board);
                    players_move_email = player1Email;
                }
                else {
                    player2->makeMove(board);
                    players_move_email = player2Email;
                }

                moves.push_back(board);
                saveMove(lastGameId, board, players_move_email, move_num, db);

                int winner = board.checkWin();
                if (winner != 0) {
                    board.display();
                    handleGameOutcome(db, player1Email, player2Email, winner, gameMode);

                    if (winner == 1) {
                        cout << "Player 1 wins!" << endl;
                        player1->win_count++;
                        player1->total_games++;
                        player2->lose_count++;
                        player2->total_games++;
                    }
                    else if (winner == -1) {
                        cout << "Player 2 wins!" << endl;
                        player2->win_count++;
                        player2->total_games++;
                        player1->lose_count++;
                        player1->total_games++;
                    }
                    else if (winner == 2) {
                        cout << "It's a tie!" << endl;
                        player1->total_games++;
                        player2->total_games++;
                    }
                    break;
                }
                player1Turn = !player1Turn;
            }

            allGames.push_back(moves);

            cout << "Do you want to play again? (1: Yes, 0: No): ";
            int playAgainChoice;
            cin >> playAgainChoice;
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (playAgainChoice)
            {
                std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
                std::string currentDateStr = timeToString(now);

                std::string insertSQL = "INSERT INTO games (player1_email, player2_email, date, game_mode) "
                    "VALUES ('" + player1Email + "', '" + player2Email + "', '" + currentDateStr + "', " + std::to_string(gameMode) + ")";
                char* errMsg = nullptr;
                int status = sqlite3_exec(db, insertSQL.c_str(), nullptr, nullptr, &errMsg);

                if (status != SQLITE_OK) {
                    std::cerr << "Error inserting game data: " << errMsg << std::endl;
                    sqlite3_free(errMsg);
                    return;
                }
                lastGameId = sqlite3_last_insert_rowid(db);
            }
            playAgain = (playAgainChoice == 1);
        }

        cout << "1)Player1 stats" << endl << "2)Player 2 stats" << endl << "3) Show Moves" << endl;
        int choose_1;
        cin >> choose_1;

        int pvp_win_count;
        int pvp_lose_count;
        int pvp_total_games;
        int pve_win_count;
        int pve_lose_count;
        int pve_total_games;
        int total_games;
        int retrieved;

        if (choose_1 == 1) {
            retrieved = 1 + getPlayerStats(db, player1Email, pvp_win_count, pvp_lose_count, pvp_total_games, pve_win_count, pve_lose_count, pve_total_games, total_games);
            cout << "Games won: " << pvp_win_count + pve_win_count << endl;
            cout << "Games lost: " << pvp_lose_count + pve_lose_count << endl;
            cout << "Games Played: " << total_games << endl;
        }
        else if (choose_1 == 2) {
            retrieved = 2 + getPlayerStats(db, player2Email, pvp_win_count, pvp_lose_count, pvp_total_games, pve_win_count, pve_lose_count, pve_total_games, total_games);
            cout << "Games won: " << pvp_win_count + pve_win_count << endl;
            cout << "Games lost: " << pvp_lose_count + pve_lose_count << endl;
            cout << "Games Played: " << total_games << endl;
        }
        else if (choose_1 == 3) {
            while (true) {
                int which_player;
                std::vector<int> gameIds;
                string playerEmail;
                cout << "Which player(1:player1 and 2:player2)" << endl;
                cin >> which_player;
                if (which_player == 1)
                    playerEmail = player1Email;
                else if (which_player == 2)
                    playerEmail = player2Email;
                else
                {
                    cout << "Invalid response" << endl;
                    break;
                }
                gameIds = getPlayerGameIds(playerEmail, db);

                if (gameIds.empty()) {
                    std::cout << "No games found for player " << playerEmail << "." << std::endl;
                    return;
                }

                std::cout << "Games for player " << playerEmail << ":" << std::endl;
                for (size_t i = 0; i < gameIds.size(); ++i) {
                    std::cout << i + 1 << ". Game ID: " << gameIds[i] << std::endl;
                }

                std::cout << "Enter the number of the game you want to display: ";
                int chosenGame;
                std::cin >> chosenGame;
                if (chosenGame >= 1 && chosenGame <= gameIds.size()) {
                    int gameId = gameIds[chosenGame - 1];
                    std::cout << "1) Show final board" << std::endl << "2) Show move by move" << std::endl;
                    int choose;
                    std::cin >> choose;
                    if (choose == 1) {
                        showFinalMove(gameId, playerEmail, db);
                    }
                    else if (choose == 2) {
                        showMoveByMove(gameId, playerEmail, db);
                    }
                    else {
                        std::cout << "Invalid option." << std::endl;
                        break;
                    }
                }
                else {
                    std::cout << "Invalid game number." << std::endl;
                    break;
                }
                cout << "Do you want to display moves of another game? (1: Yes, 0: No): ";
                int displayAnotherGameChoice;
                cin >> displayAnotherGameChoice;
                if (displayAnotherGameChoice != 1) {
                    break;
                }
            }
        }
    }

};



int main() {
    sqlite3* db; // SQLite database connection
    const char* dbFilename = "tictactoe26.db"; // Database file name

    // Open the database connection
    if (sqlite3_open(dbFilename, &db) != SQLITE_OK) {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << endl;
        return 1; // Exit on error
    }
    
    int gameMode;
    cout << "Choose game mode: 1. Player vs. Player, 2. Player vs. AI" << endl;
    cin >> gameMode;

    if (gameMode == 1) { // Player vs. Player

        // Ask if players have accounts
        int hasAccount1;
        cout << "Player 1, do you have an account? (1: Yes, 0: No): ";
        cin >> hasAccount1;
        if (hasAccount1 == 1) {
            player1Email = login(db); // Log in Player 1
            if (player1Email == "problem")
            {
                return 0;
            }
        }
        else {
            player1Email = signup(db); // Sign up Player 1
        }

        int hasAccount2;
        cout << "Player 2, do you have an account? (1: Yes, 0: No): ";
        cin >> hasAccount2;
        if (hasAccount2 == 1) {
            player2Email = login(db); // Log in Player 2
            if (player2Email == "problem")
            {
                return 0;
            }
        }
        else {
            player2Email = signup(db); // Sign up Player 2
        }

        // Set up the game for Player vs. Player
        Human1Player human1;
        Human2Player human2;
        Game game(&human1, &human2, db, player1Email, player2Email, gameMode);
        game.play(); // Start the game
    }
    else if (gameMode == 2) { // Player vs. AI
        player2Email = 123;
        int hasAccount;
        cout << "Do you have an account? (1: Yes, 0: No): ";
        cin >> hasAccount;
        if (hasAccount == 1) {
            player1Email = login(db); // Log in
            if (player1Email == "problem")
            {
                return 0;
            }
        }
        else {
            player1Email = signup(db); // Sign up
        }

        Human1Player human;
        AIPlayer ai; // Set up AI
        Game game(&human, &ai, db, player1Email, "AI", gameMode);
        game.play(); // Start the game
    }
    else {
        cout << "Invalid choice." << endl;
    }

    // Close the database connection
    sqlite3_close(db);
    return 0;
}

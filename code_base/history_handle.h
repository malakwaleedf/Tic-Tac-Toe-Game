#pragma once

using namespace std;

void saveMove(int gameId, const GameBoard& board, const std::string& playerTurn, int moveNumber, sqlite3* db);
void showBoard(const char boardStr[]);
void showMoveByMove(int gameId, const std::string& playerEmail, sqlite3* db);
std::vector<int> getPlayerGameIds(const std::string& playerEmail, sqlite3* db);
void showFinalMove(int gameId, const std::string& playerEmail, sqlite3* db);
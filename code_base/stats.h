#pragma once

using namespace std;

bool getPlayerStats(sqlite3* db, const string& email,
    int& pvp_win_count, int& pvp_lose_count, int& pvp_total_games,
    int& pve_win_count, int& pve_lose_count, int& pve_total_games, int& total_games);

void updatePlayerStats(sqlite3* db, const string& email,
    int pvp_win_count, int pvp_lose_count, int pvp_total_games,
    int pve_win_count, int pve_lose_count, int pve_total_games);

void handleGameOutcome(sqlite3* db, const string& player1Email,
    const string& player2Email, int game_result, int gameMode);
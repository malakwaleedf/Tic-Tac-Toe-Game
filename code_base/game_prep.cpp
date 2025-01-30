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

using namespace std;


GameBoard::GameBoard()
{
    // Initialize the game board to all zeros
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = 0;
        }
    }
}

    // Display the game board
void GameBoard::display() const {
    cout << "  1 2 3 " << endl;
    cout << " -------" << endl;
    for (int i = 0; i < 3; ++i) {
        cout << i + 1 << "|";
        for (int j = 0; j < 3; ++j) {
            if (board[i][j] == 1) {
                cout << "X ";
            }
            else if (board[i][j] == -1) {
                cout << "O ";
            }
            else {
                cout << "- ";
            }
        }
        cout << endl;
    }
    cout << " -------" << endl;
}


    // Check if the game has been won by a player
int GameBoard::checkWin() const {
    // Check rows
    for (int i = 0; i < 3; i++) {
        if (board[i][0] + board[i][1] + board[i][2] == 3) {
            return 1; // Player 1 wins
        }
        else if (board[i][0] + board[i][1] + board[i][2] == -3) {
            return -1; // Player 2 wins
        }
    }
    // Check columns
    for (int j = 0; j < 3; j++) {
        if (board[0][j] + board[1][j] + board[2][j] == 3) {
            return 1; // Player 1 wins
        }
        else if (board[0][j] + board[1][j] + board[2][j] == -3) {
            return -1; // Player 2 wins
        }
    }
    // Check diagonals
    if (board[0][0] + board[1][1] + board[2][2] == 3 || board[0][2] + board[1][1] + board[2][0] == 3) {
        return 1; // Player 1 wins
    }
    else if (board[0][0] + board[1][1] + board[2][2] == -3 || board[0][2] + board[1][1] + board[2][0] == -3) {
        return -1; // Player 2 wins
    }
    // Check for draw
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == 0) {
                return 0; // Game is not over yet
            }
        }
    }
    return 2; // Game is a draw
}

    // Accessor for the board
int GameBoard::getValue(int row, int col) const {
    return board[row][col];
}


    // Setter for the board
void GameBoard::setValue(int row, int col, int value) {

    board[row][col] = value;
    ch_row = row;
    ch_col = col;
}


void Human1Player::makeMove(GameBoard& board) const 
{
    int row, col;
    cout << "Enter row and column to place your mark (1-3): ";
    while (true) {
        cin >> row >> col;
        if (row > 3 || col > 3 || row == 0 || col == 0) {
            cout << "false coordinates" << endl;
            cout << "Re-enter moves" << endl;
        }
        else break;
    }
    row--;
    col--;
    while (board.getValue(row, col) != 0) {
        cout << "That spot is already taken. Enter a different spot: ";
        cin >> row >> col;
        row--;
        col--;
    }
    board.setValue(row, col, 1);
}


void Human2Player::makeMove(GameBoard& board) const  {
    int row, col;
    cout << "Enter row and column to place your mark (1-3): ";
    while (true) {
        cin >> row >> col;
        if (row > 3 || col > 3 || row == 0 || col == 0) {
            cout << "false coordinates" << endl;
            cout << "Re-enter moves" << endl;
        }
        else break;
    }
    row--;
    col--;
    while (board.getValue(row, col) != 0) {
        cout << "That spot is already taken. Enter a different spot: ";
        cin >> row >> col;
        row--;
        col--;
    }
    board.setValue(row, col, -1);
}

void AIPlayer::makeMove(GameBoard& board) const  {
    cout << "AI Move:" << endl;
    TreeNode* root = new TreeNode;
    root->board = board;
    build_tree(root, -1); // AI is player -1

    int best_score = numeric_limits<int>::min();
    TreeNode* best_move = nullptr;
    for (TreeNode* child : root->children) {
        int score = minimax(child, numeric_limits<int>::min(), numeric_limits<int>::max(), false, 9); // Increase depth of search
        if (score > best_score) {
            best_score = score;
            best_move = child;
        }
    }

    board.setValue(best_move->moveRow, best_move->moveCol, -1); // AI's move

    // Free memory
    for (TreeNode* child : root->children) {
        delete child;
    }
    delete root;
}

void AIPlayer::build_tree(TreeNode* node, int player) const {
    int winner = node->board.checkWin();
    if (winner != 0) {
        node->score = winner;
        return;
    }

    // Generate child nodes for possible moves
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (node->board.getValue(i, j) == 0) {
                TreeNode* child = new TreeNode;
                child->board = node->board;
                child->board.setValue(i, j, player);
                child->moveRow = i;
                child->moveCol = j;
                node->children.push_back(child);
                build_tree(child, -player);
            }
        }
    }
}

int AIPlayer::minimax(TreeNode* node, int alpha, int beta, bool is_max, int depth) const { // Adjust depth parameter
    if (node->children.empty() || depth == 0) {
        return evaluate(node->board); // Evaluate the board state
    }

    if (is_max) {
        int max_score = numeric_limits<int>::min();
        for (TreeNode* child : node->children) {
            int score = minimax(child, alpha, beta, false, depth - 1);
            max_score = max(max_score, score);
            alpha = max(alpha, score);
            if (alpha >= beta) {
                break;
            }
        }
        return max_score;
    }
    else {
        int min_score = numeric_limits<int>::max();
        for (TreeNode* child : node->children) {
            int score = minimax(child, alpha, beta, true, depth - 1);
            min_score = min(min_score, score);
            beta = min(beta, score);
            if (alpha >= beta) {
                break;
            }
        }
        return min_score;
    }
}

int AIPlayer::evaluate(const GameBoard& board) const {
    int result = board.checkWin();
    if (result == 1) { // If player wins, return a low score
        return -1000;
    }
    else if (result == -1) { // If AI wins, return a high score
        return 1000;
    }
    else if (result == 2) { // If it's a draw, return 0
        return 0;
    }
    // Otherwise, return a neutral score
    return 0;
}

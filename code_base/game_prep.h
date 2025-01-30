#pragma once


class GameBoard {
private:

public:
    int board[3][3]; // Represent the game board
    int ch_row, ch_col;
    GameBoard();
    void display() const;
    int checkWin() const;
    int getValue(int row, int col) const;
    void setValue(int row, int col, int value);
};

// Abstract class for players
class Player {
public:
    virtual void makeMove(GameBoard& board) const = 0;
    int win_count = 0;
    int lose_count = 0;
    int total_games = 0;

};


// Human player class
class Human1Player : public Player {
public:
    int win_count = 0;
    int lose_count = 0;
    int total_games = 0;
    void makeMove(GameBoard& board) const override;
};

class Human2Player : public Player {
public:
    int win_count = 0;
    int lose_count = 0;
    int total_games = 0;
    void makeMove(GameBoard& board) const override;
};


class AIPlayer : public Player {
public:
    void makeMove(GameBoard& board) const override;
    

private:
    // Define TreeNode structure
    struct TreeNode {
        GameBoard board;
        vector<TreeNode*> children;
        int score;
        int moveRow;
        int moveCol;
    };

    // Create a recursive function to build the game tree
    void build_tree(TreeNode* node, int player) const;
    
    // Create a function to perform minimax with alpha-beta pruning
    int minimax(TreeNode* node, int alpha, int beta, bool is_max, int depth) const;
    
    // Evaluate the board state
    int evaluate(const GameBoard& board) const;
};
#include "../Project/aiplayer.h"
#include "../Project/gameboard.h"
#include "../Project/mainwindow.h"
#include <QTest>
#include <iostream>
#include <QMessageBox>


// Test fixture for AIPlayer unit tests
class Tests : public QObject {
    Q_OBJECT


private slots:

    //aiplayer tests
    void testAIMove();
    void testBuildTree();
    void testMinimax();
    void testEvaluate();

    //gameboard tests
    void testCheckWin();
    void testGameNotOver();
    void testInitialEmptyBoard();
    void testSetGetValue();
    void testOutOfBoundsAccess();

    //mainwindow tests
    void testLogin();
    void testSignup();
    void testCustomHash();
    void testHashPassword();






    // Helper function to create a board with a specific state
    GameBoard createBoard(std::initializer_list<std::initializer_list<int>> values) {
        GameBoard board;
        int row = 0;
        for (auto& rowValues : values) {
            int col = 0;
            for (auto value : rowValues) {
                board.setValue(row, col, value);
                ++col;
            }
            ++row;
        }
        return board;
    }
};


void Tests::testAIMove() {
    GameBoard board;
    AIPlayer aiPlayer;

    // Test AI move on an empty board
    {
        // Simulate an empty board
        aiPlayer.makeMove(board);

        // Assert that AI has made a move
        bool moveMade = false;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (board.getValue(i, j) != 0) {
                    moveMade = true;
                    break;
                }
            }
        }
        QVERIFY(moveMade);
    }

    // Test AI move on a partially filled board
    {
        // Set up a partially filled board
        board.setValue(0, 0, 1);
        board.setValue(0, 1, -1);
        board.setValue(1, 1, 1);

        aiPlayer.makeMove(board);

        // Assert that AI has made a move in one of the empty spaces
        bool moveMade = false;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (board.getValue(i, j) != 0) {
                    if (!(i == 0 && j == 0) && !(i == 0 && j == 1) && !(i == 1 && j == 1)) {
                        moveMade = true;
                        break;
                    }
                }
            }
        }
        QVERIFY(moveMade);
    }
}




// Test AIPlayer build_tree function
void Tests::testBuildTree() {
    GameBoard board;
    AIPlayer aiPlayer;
    TreeNode* root = new TreeNode;

    // Simulate an initial board state and build the tree
    board.setValue(0, 0, 0); // Empty board
    aiPlayer.build_tree(root, -1);

    // Assert that root node has children corresponding to possible moves
    QVERIFY(root->children.size() > 0);

    delete root;
}

void Tests::testMinimax() {
    GameBoard board;
    AIPlayer aiPlayer;

    // Test AIPlayer minimax function with initial board state (empty board)
    {
        TreeNode* root = new TreeNode;
        board.setValue(0, 0, 0); // Empty board
        aiPlayer.build_tree(root, -1);
        int score = aiPlayer.minimax(root, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true, 3);

        QVERIFY(score >= -1000);
        delete root;
    }

    // Test AIPlayer minimax function with one move made by the maximizer
    {
        TreeNode* root = new TreeNode;
        board.setValue(0, 0, 1); // Maximizer's move
        aiPlayer.build_tree(root, -1);
        int score = aiPlayer.minimax(root, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), false, 3);

        QVERIFY(score <= 1000);
        delete root;
    }

    // Test AIPlayer minimax function with one move made by the minimizer
    {
        TreeNode* root = new TreeNode;
        board.setValue(0, 0, -1); // Minimizer's move
        aiPlayer.build_tree(root, 1);
        int score = aiPlayer.minimax(root, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true, 3);

        QVERIFY(score >= -1000);
        delete root;
    }

    // Test AIPlayer minimax function with a complex board state and deeper depth
    {
        TreeNode* root = new TreeNode;
        board.setValue(0, 0, 1);
        board.setValue(1, 1, -1);
        board.setValue(2, 2, 1);
        aiPlayer.build_tree(root, -1);
        int score = aiPlayer.minimax(root, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true, 5);

        QVERIFY(score >= -1000);
        delete root;
    }

    // Test AIPlayer minimax function with another complex state and different starting player
    {
        TreeNode* root = new TreeNode;
        board.setValue(0, 0, -1);
        board.setValue(0, 1, 1);
        board.setValue(0, 2, -1);
        board.setValue(1, 0, 1);
        aiPlayer.build_tree(root, -1);
        int score = aiPlayer.minimax(root, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), false, 4);

        QVERIFY(score <= 1000);
        delete root;
    }
}

void Tests::testEvaluate() {
    AIPlayer aiPlayer;

    // Test AI wins horizontally
    {
        GameBoard board = createBoard({
            { -1, -1, -1 },
            {  0,  0,  0 },
            {  0,  0,  0 }
        });
        QCOMPARE(aiPlayer.evaluate(board), 1000);
    }

    // Test AI wins vertically
    {
        GameBoard board = createBoard({
            { -1,  0,  0 },
            { -1,  0,  0 },
            { -1,  0,  0 }
        });
        QCOMPARE(aiPlayer.evaluate(board), 1000);
    }

    // Test AI wins diagonally
    {
        GameBoard board = createBoard({
            { -1,  0,  0 },
            {  0, -1,  0 },
            {  0,  0, -1 }
        });
        QCOMPARE(aiPlayer.evaluate(board), 1000);
    }

    // Test Player wins horizontally
    {
        GameBoard board = createBoard({
            {  1,  1,  1 },
            {  0,  0,  0 },
            {  0,  0,  0 }
        });
        QCOMPARE(aiPlayer.evaluate(board), -1000);
    }

    // Test Player wins vertically
    {
        GameBoard board = createBoard({
            {  0,  0,  1 },
            {  0,  0,  1 },
            {  0,  0,  1 }
        });
        QCOMPARE(aiPlayer.evaluate(board), -1000);
    }

    // Test Player wins diagonally
    {
        GameBoard board = createBoard({
            {  1,  0,  0 },
            {  0,  1,  0 },
            {  0,  0,  1 }
        });
        QCOMPARE(aiPlayer.evaluate(board), -1000);
    }

    // Test game is a draw
    {
        GameBoard board = createBoard({
            {  1, -1,  1 },
            { -1,  1, -1 },
            { -1,  1, -1 }
        });
        QCOMPARE(aiPlayer.evaluate(board), 0);
    }
}



void Tests::testCheckWin() {
    // Test Player 1 wins by a row
    {
        GameBoard board;
        board.setValue(0, 0, 1);
        board.setValue(0, 1, 1);
        board.setValue(0, 2, 1);
        QCOMPARE(board.checkWin(), 1);
    }

    // Test Player 2 wins by a row
    {
        GameBoard board;
        board.setValue(1, 0, -1);
        board.setValue(1, 1, -1);
        board.setValue(1, 2, -1);
        QCOMPARE(board.checkWin(), -1);
    }

    // Test Player 1 wins by a column
    {
        GameBoard board;
        board.setValue(0, 2, 1);
        board.setValue(1, 2, 1);
        board.setValue(2, 2, 1);
        QCOMPARE(board.checkWin(), 1);
    }

    // Test Player 2 wins by a column
    {
        GameBoard board;
        board.setValue(0, 0, -1);
        board.setValue(1, 0, -1);
        board.setValue(2, 0, -1);
        QCOMPARE(board.checkWin(), -1);
    }

    // Test Player 1 wins by the main diagonal
    {
        GameBoard board;
        board.setValue(0, 0, 1);
        board.setValue(1, 1, 1);
        board.setValue(2, 2, 1);
        QCOMPARE(board.checkWin(), 1);
    }

    // Test Player 2 wins by the main diagonal
    {
        GameBoard board;
        board.setValue(0, 0, -1);
        board.setValue(1, 1, -1);
        board.setValue(2, 2, -1);
        QCOMPARE(board.checkWin(), -1);
    }

    // Test Player 1 wins by the anti-diagonal
    {
        GameBoard board;
        board.setValue(0, 2, 1);
        board.setValue(1, 1, 1);
        board.setValue(2, 0, 1);
        QCOMPARE(board.checkWin(), 1);
    }

    // Test Player 2 wins by the anti-diagonal
    {
        GameBoard board;
        board.setValue(0, 2, -1);
        board.setValue(1, 1, -1);
        board.setValue(2, 0, -1);
        QCOMPARE(board.checkWin(), -1);
    }

    // Test game is a draw
    {
        GameBoard board;
        board.setValue(0, 0, 1);
        board.setValue(0, 1, -1);
        board.setValue(0, 2, 1);
        board.setValue(1, 0, -1);
        board.setValue(1, 1, 1);
        board.setValue(1, 2, -1);
        board.setValue(2, 0, -1);
        board.setValue(2, 1, 1);
        board.setValue(2, 2, -1);
        QCOMPARE(board.checkWin(), 2);
    }
}


void Tests::testGameNotOver() {
    GameBoard board;
    // Set up a board where the game is not over yet
    board.setValue(0, 0, 1);
    board.setValue(1, 1, -1);
    board.setValue(2, 2, 0);
    QCOMPARE(board.checkWin(), 0);
}




void Tests::testInitialEmptyBoard() {
    GameBoard board;
    // Test the initial empty board state
    QCOMPARE(board.checkWin(), 0);
}

void Tests::testSetGetValue() {
    GameBoard board;

    // Set up a specific board state for testing
    board.setValue(0, 0, 1);
    board.setValue(1, 1, -1);
    board.setValue(2, 2, 0);

    // Verify the values retrieved from the board
    QCOMPARE(board.getValue(0, 0), 1); // Expected value: 1
    QCOMPARE(board.getValue(1, 1), -1); // Expected value: -1
    QCOMPARE(board.getValue(2, 2), 0); // Expected value: 0

    // Modify values and verify again
    board.setValue(0, 0, -1);
    board.setValue(1, 1, 1);
    board.setValue(2, 2, 1);

    QCOMPARE(board.getValue(0, 0), -1); // Expected value: -1 after modification
    QCOMPARE(board.getValue(1, 1), 1); // Expected value: 1 after modification
    QCOMPARE(board.getValue(2, 2), 1); // Expected value: 1 after modification
}

// Additional test case to verify out-of-bound access handling
void Tests::testOutOfBoundsAccess() {
    GameBoard board;

    // Set values within bounds
    board.setValue(0, 0, 1);
    board.setValue(1, 1, -1);
    board.setValue(2, 2, 0);

    // Verify in-bound accesses
    QCOMPARE(board.getValue(0, 0), 1); // Expected value: 1
    QCOMPARE(board.getValue(1, 1), -1); // Expected value: -1
    QCOMPARE(board.getValue(2, 2), 0); // Expected value: 0

    // Attempt to access out-of-bound indices
    QCOMPARE(board.getValue(3, 0), 0); // Expected value: 0 (default for out-of-bounds)
    QCOMPARE(board.getValue(0, 3), 0); // Expected value: 0 (default for out-of-bounds)
}

void Tests::testLogin() {
    sqlite3 *db;
    int rc = sqlite3_open(":memory:", &db);
    QVERIFY2(rc == SQLITE_OK, "Failed to open SQLite database");

    // Create the players table and insert a test user
    const char *createTableSQL = "CREATE TABLE players (id INTEGER PRIMARY KEY, email TEXT, password TEXT, last_login_date TEXT)";
    char *errMsg = nullptr;
    rc = sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg);
    QVERIFY2(rc == SQLITE_OK, QString("Failed to create table: %1").arg(errMsg).toStdString().c_str());
    sqlite3_free(errMsg);

    const char *insertSQL = "INSERT INTO players (email, password) VALUES ('test@example.com', 'testpassword')";
    rc = sqlite3_exec(db, insertSQL, nullptr, nullptr, &errMsg);
    QVERIFY2(rc == SQLITE_OK, QString("Failed to insert test user: %1").arg(errMsg).toStdString().c_str());
    sqlite3_free(errMsg);

    // Test correct login
    MainWindow m;
    std::string email = "test@example.com";
    std::string password = "testpassword";
    std::string result = m.login(db, email, password);
    QCOMPARE(result, email);

    // Test incorrect login
    password = "wrongpassword";
    result = m.login(db, email, password);
    QCOMPARE(result, std::string(""));

    // Test non-existent user
    email = "nonexistent@example.com";
    password = "testpassword";
    result = m.login(db, email, password);
    QCOMPARE(result, std::string(""));

    // Clean up
    sqlite3_close(db);
}

void Tests::testSignup() {
    sqlite3 *db;
    int rc = sqlite3_open(":memory:", &db);
    QVERIFY2(rc == SQLITE_OK, "Failed to open SQLite database");

    const char *createTableSQL = "CREATE TABLE players (id INTEGER PRIMARY KEY, email TEXT, password TEXT, name TEXT, age INTEGER, city TEXT, current_date TEXT)";
    char *errMsg = nullptr;
    rc = sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg);
    QVERIFY2(rc == SQLITE_OK, QString("Failed to create table: %1").arg(errMsg).toStdString().c_str());
    sqlite3_free(errMsg);

    MainWindow mainWindow;

    std::string email = "test@example.com";
    std::string password = "testpassword";
    std::string name = "Test User";
    int age = 30;
    std::string city = "Test City";
    std::string result = mainWindow.signup(db, email, password, name, age, city);
    QCOMPARE(result, email);

    result = mainWindow.signup(db, email, password, name, age, city);
    QCOMPARE(result, std::string(""));

    const char *selectSQL = "SELECT email, password, name, age, city FROM players WHERE email = 'test@example.com'";
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr);
    QVERIFY2(rc == SQLITE_OK, "Failed to prepare select statement");

    rc = sqlite3_step(stmt);
    QVERIFY2(rc == SQLITE_ROW, "No data found");

    QCOMPARE(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))), email);
    QCOMPARE(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))), password);
    QCOMPARE(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))), name);
    QCOMPARE(sqlite3_column_int(stmt, 3), age);
    QCOMPARE(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4))), city);

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}


void Tests::testCustomHash() {
    MainWindow mainWindow;
    std::string str = "teststring";
    uint64_t hash1 = mainWindow.customHash(str);
    uint64_t hash2 = mainWindow.customHash(str);

    // Verify that the hash is consistent
    QCOMPARE(hash1, hash2);

    // Verify that different strings produce different hashes
    std::string differentStr = "differentstring";
    uint64_t differentHash = mainWindow.customHash(differentStr);
    QVERIFY(hash1 != differentHash);
}

void Tests::testHashPassword() {
    MainWindow mainWindow;
    std::string password = "password123";
    std::string hashedPassword = mainWindow.hashPassword(password);

    // Verify that the hashed password is not empty
    QVERIFY(!hashedPassword.empty());

    // Verify that hashing the same password produces the same result
    std::string hashedPassword2 = mainWindow.hashPassword(password);
    QCOMPARE(hashedPassword, hashedPassword2);

    // Verify that different passwords produce different hashes
    std::string differentPassword = "differentpassword";
    std::string differentHashedPassword = mainWindow.hashPassword(differentPassword);
    QVERIFY(hashedPassword != differentHashedPassword);
}







// Include the QTEST_MAIN macro to compile the unit tests
QTEST_MAIN(Tests)

#include "tst_unittests.moc" // Include the moc file for Qt Test

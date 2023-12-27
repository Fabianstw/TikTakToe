//
// Created by Fabian Stiewe on 26.12.23.
//


#include <iostream>
#include <array>
#include <vector>
#include <ctime>

using namespace std;

class TicTacToeGame {

    // init to get the starting player
    int player = 0;
    array<array<string, 3>, 3> board;

public:

    explicit TicTacToeGame(int getPlayer) {

        if (0 <= getPlayer && getPlayer < 2) {
            player = getPlayer;
        } else {
            std::cout << "You did not provide a number between 0 and 1, random number is provided" << std::endl;
            player = chooseRandomPlayer();
        }
        std::cout << "Player " << player << " starts" << std::endl;

        // init the board
        board = generateBoard();

        // first print of Board
        printBoard();

        // start the mainGameLoop
        mainGameLoop();

    }

private:

    void mainGameLoop () {

        // start with a while loop until there have been 9 turns
        int counter = 0;
        bool ai = true;
        int player_input;

        while (true) {

            while (counter < 9) {

                // first get the player input
                // do not simplify this, because of possible change of AI
                if (ai == true and player == 1) {
                     player_input = getAI_input(counter);
                } else {
                    player_input = getPlayerInput();
                }

                if (player == 0) {
                    board[(player_input-1) / 3][(player_input-1) % 3] = "X";
                } else {
                    board[(player_input-1) / 3][(player_input-1) % 3] = "O";
                }

                printBoard();

                if (checkWinner(board)) {
                    std::cout << "Player " << player << " won" << std::endl;
                    break;
                }

                // every round to change the player
                changePlayer();
                counter += 1;
            }

            std::cout << "It is a draw" << std::endl;

            std::cout << "Do you want to play again? Then enter: 'Y'. \n" << std::endl;
            string current_input;
            std::cin >> current_input;
            if (current_input != "Y") {
                return;
            }
            std::cout << "New Game" << std::endl;
            counter = 0;
            board = generateBoard();
            player = chooseRandomPlayer();

        }

    }

    int getAI_input (int round) {

        int new_move;

        if (round == 0) {
            return 5;
        }

        if (round == 1) {
            if (board[1][1] == " ") {
                return 5;
            } else {
                return 1;
            }
        }

        // check if it is possible to win in one move
        new_move = checkIfAICanWinInOneMove(board);
        if (new_move != -1) {
            return new_move;
        }

        // check if the player can win in one move and block this possibility
        new_move = checkIfPlayerCanWinInOneMove(board);
        if (new_move != -1) {
            return new_move;
        }

        pair<int, int> answer = getAIinputMinMax(round, board, false);
        if (0 <= answer.second && answer.second <= 8) {
            return answer.second;
        }

        // nothing found to win or block, so choose a random number, because we are not that smart
        while (true) {
            int random_number = rand() % 9 + 1;
            if (board[(random_number-1) / 3][(random_number-1) % 3] == " ") {
                return random_number;
            }
        }

        // no return here should have returned before already
    }

    pair<int, int> getAIinputMinMax (int round, const array<array<string, 3>, 3>& board_copy, bool player_here) {

        // Calculate all possible games till the end
        // win is 1, draw is 0, lose is -1
        if (round == 9) {
            // return 0 and an empty vector
            return make_pair(0, -1);
        }

        // get all free fields
        vector<int> free_fields;

        for (int i = 0; i < 9; i++) {
            if (board_copy[i / 3][i % 3] == " ") {
                free_fields.push_back(i);
            }
        }

        // save all the results
        vector<int> results;
        for (int free_field : free_fields) {

            array<array<string, 3>, 3> board_copy_move = board_copy;
            if (!player_here) {
                board_copy_move[free_field / 3][free_field % 3] = "O";
            } else {
                board_copy_move[free_field / 3][free_field % 3] = "X";
            }
            pair<int, int> tmp = getAIinputMinMax(round + 1, board_copy_move, !player_here);

            // check if AI has won
            if (player_here) {
                if (checkWinner(board_copy_move)) {
                    // return -1 and an empty vector
                    return make_pair(-1, -1);
                }
            // check if the player has won
            } else {
                if (checkWinner(board_copy_move)) {
                    // return 1 and an empty vector
                    return make_pair(1,free_field);
                }
            }

            results.push_back(tmp.first);

        }

        int bestResult = player_here ? 2 : -2;  // Initialize bestResult based on player's turn
        int move;
        for (size_t i = 0; i < results.size(); ++i) {
            int result = results[i];
            int moveIndex = free_fields[i];

            if ((player_here && result < bestResult) || (!player_here && result > bestResult)) {
                bestResult = result;
                move = moveIndex + 1;  // Update move based on the correct index in free_fields
            }
        }

        return make_pair(bestResult, move);

    }

    static int checkIfAICanWinInOneMove (array<array<string, 3>, 3> board_copy) {
        // check the rows
        for (int i = 0; i < 3; i++) {
            if (board_copy[i][0] == board_copy[i][1] && board_copy[i][0] == "O" && board_copy[i][2] == " ") {
                return 3*i + 3;
            }
            if (board_copy[i][1] == board_copy[i][2] && board_copy[i][1] == "O" && board_copy[i][0] == " ") {
                return 3*i + 1;
            }
            if (board_copy[i][0] == board_copy[i][2] && board_copy[i][0] == "O" && board_copy[i][1] == " ") {
                return 3*i + 2;
            }
        }

        // check the columns
        for (int i = 0; i < 3; i++) {
            if (board_copy[0][i] == board_copy[1][i] && board_copy[0][i] == "O" && board_copy[2][i] == " ") {
                return 7 + i;
            }
            if (board_copy[1][i] == board_copy[2][i] && board_copy[1][i] == "O" && board_copy[0][i] == " ") {
                return 1 + i;
            }
            if (board_copy[0][i] == board_copy[2][i] && board_copy[0][i] == "O" && board_copy[1][i] == " ") {
                return 4 + i;
            }
        }

        // check the diagonals
        if (board_copy[0][0] == board_copy[1][1] && board_copy[0][0] == "O" && board_copy[2][2] == " ") {
            return 9;
        }
        if (board_copy[1][1] == board_copy[2][2] && board_copy[1][1] == "O" && board_copy[0][0] == " ") {
            return 1;
        }
        if (board_copy[0][0] == board_copy[2][2] && board_copy[0][0] == "O" && board_copy[1][1] == " ") {
            return 5;
        }
        if (board_copy[0][2] == board_copy[1][1] && board_copy[0][2] == "O" && board_copy[2][0] == " ") {
            return 7;
        }
        if (board_copy[1][1] == board_copy[2][0] && board_copy[1][1] == "O" && board_copy[0][2] == " ") {
            return 3;
        }
        if (board_copy[0][2] == board_copy[2][0] && board_copy[0][2] == "O" && board_copy[1][1] == " ") {
            return 5;
        }

        // not possible to win in one move
        return -1;
    }

    static int checkIfPlayerCanWinInOneMove (array<array<string, 3>, 3> board_copy) {

        // check the rows
        for (int i = 0; i < 3; i++) {
            if (board_copy[i][0] == board_copy[i][1] && board_copy[i][0] == "X" && board_copy[i][2] == " ") {
                return 3*i + 3;
            }
            if (board_copy[i][1] == board_copy[i][2] && board_copy[i][1] == "X" && board_copy[i][0] == " ") {
                return 3*i + 1;
            }
            if (board_copy[i][0] == board_copy[i][2] && board_copy[i][0] == "X" && board_copy[i][1] == " ") {
                return 3*i + 2;
            }
        }

        // check the columns
        for (int i = 0; i < 3; i++) {
            if (board_copy[0][i] == board_copy[1][i] && board_copy[0][i] == "X" && board_copy[2][i] == " ") {
                return 7 + i;
            }
            if (board_copy[1][i] == board_copy[2][i] && board_copy[1][i] == "X" && board_copy[0][i] == " ") {
                return 1 + i;
            }
            if (board_copy[0][i] == board_copy[2][i] && board_copy[0][i] == "X" && board_copy[1][i] == " ") {
                return 4 + i;
            }
        }

        // check the diagonals
        if (board_copy[0][0] == board_copy[1][1] && board_copy[0][0] == "X" && board_copy[2][2] == " ") {
            return 9;
        }
        if (board_copy[1][1] == board_copy[2][2] && board_copy[1][1] == "X" && board_copy[0][0] == " ") {
            return 1;
        }
        if (board_copy[0][0] == board_copy[2][2] && board_copy[0][0] == "X" && board_copy[1][1] == " ") {
            return 5;
        }
        if (board_copy[0][2] == board_copy[1][1] && board_copy[0][2] == "X" && board_copy[2][0] == " ") {
            return 7;
        }
        if (board_copy[1][1] == board_copy[2][0] && board_copy[1][1] == "X" && board_copy[0][2] == " ") {
            return 3;
        }
        if (board_copy[0][2] == board_copy[2][0] && board_copy[0][2] == "X" && board_copy[1][1] == " ") {
            return 5;
        }

        return -1;

    }

    static bool checkWinner (array<array<string, 3>, 3> board_copy) {

        // check if there is a winner
        // check the rows
        for (int i = 0; i < 3; i++) {
            if (board_copy[i][0] == board_copy[i][1] && board_copy[i][1] == board_copy[i][2] && board_copy[i][0] != " ") {
                return true;
            }
        }

        // check the columns
        for (int i = 0; i < 3; i++) {
            if (board_copy[0][i] == board_copy[1][i] && board_copy[1][i] == board_copy[2][i] && board_copy[0][i] != " ") {
                return true;
            }
        }

        // check the diagonals
        if (board_copy[0][0] == board_copy[1][1] && board_copy[1][1] == board_copy[2][2] && board_copy[0][0] != " ") {
            return true;
        }

        if (board_copy[0][2] == board_copy[1][1] && board_copy[1][1] == board_copy[2][0] && board_copy[0][2] != " ") {
            return true;
        }

        return false;

    }

    int getPlayerInput () {
        // get the input of the player
        int player_input;
        while (true) {
            std::cout << "Player " << player << " please enter a number between 1 and 9: " << std::endl;
            std::cin >> player_input;
            if (1 <= player_input && player_input <= 9) {
                // check if the field is empty
                if (board[(player_input-1) / 3][(player_input-1) % 3] == " ") {
                    return player_input;
                }
            }
            std::cout << "You did not provide a valid input" << std::endl;
        }
        // no return here needed anymore
    }

    void changePlayer () {
        // add one a mod 2 to change the player
        player = (player + 1) % 2;
    }

    void printBoard () {
        std::cout << "Board: " << std::endl;
        for (int i = 0; i < 3; i++) {

            for (int j = 0; j < 3; j++) {
                if (j == 2) {
                    std::cout << board[i][j];
                    break;
                }
                std::cout << board[i][j] << " | ";
            }
            if (i == 2) {
                std::cout << std::endl;
                break;
            }
            std::cout << std::endl;
            std::cout << "---------" << std::endl;

        }
    }

    static array<array<string , 3>, 3> generateBoard() {
        array<array<string , 3>, 3> board;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                board[i][j] = " ";
            }
        }
        return board;
    }

    static int chooseRandomPlayer () {
        srand(time(0)); // seed the random number generator
        return rand() % 2;
    }

};

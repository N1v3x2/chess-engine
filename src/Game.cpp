#include "Board.h"
#include "Move.h"

#include <iostream>
#include <stdexcept>

using std::cout, std::cin;
using std::invalid_argument;

int main() {
    Board board;

    // TODO: check for game end conditions
    while (true) {
        board.printBoard();

        bool validMove = false;
        string inputMove;
        unordered_map<string, Move> legalMoves = board.generateMoves();

        while (!validMove) {
            cout << "Make your move ("
                 << (board.getSideToPlay() == PC_WHITE ? "white" : "black")
                 << "): ";
            cin >> inputMove;
            try {
                Move move = board.parseMove(inputMove, legalMoves);
                board.makeMove(move);
                validMove = true;
            } catch (const invalid_argument& err) {
                cout << "Parse error: " << err.what() << "\n";
            }
        }

        board.flipSides();
    }

    cout << "Thanks for playing!\n";
}

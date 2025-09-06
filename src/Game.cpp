#include "Board.h"
#include "Move.h"
#include "Zobrist.h"
#include <iostream>
#include <stdexcept>

using std::cout, std::cin;
using std::invalid_argument;

int main() {
    zobrist::init();

    Board board;
    bool validMove;
    string inputMove;
    unordered_map<string, Move> legalMoves;

    // TODO: check for game end conditions
    while (true) {
        board.printBoard();

        validMove = false;
        legalMoves = board.generateMoves();

        while (!validMove) {
            cout << "[game] Make your move ("
                 << (board.getSideToPlay() == PC_WHITE ? "white" : "black")
                 << "): ";
            cin >> inputMove;
            try {
                Move move = board.parseMove(inputMove, legalMoves);
                board.makeMove(move);
                validMove = true;
            } catch (const invalid_argument& err) {
                cout << "[game] " << err.what() << "\n";
            }
        }

        board.flipSides();
    }

    cout << "Thanks for playing!\n";
}

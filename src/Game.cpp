#include "board/Board.hpp"
#include <iostream>
#include <stdexcept>

using std::cout, std::cin;
using std::invalid_argument;

int main() {
    zobrist::init();

    Board board;
    bool validMove = false, cont = true;
    string inputMove, gameMsg;

    while (true) {
        board.printBoard();
        board.generateMoves();

        cont = false;
        switch (board.checkGameResult()) {
        case GR_CHECKMATE:
            gameMsg = "Checkmate!\n";
            break;
        case GR_STALEMATE:
            gameMsg = "Draw by stalemate.\n";
            break;
        case GR_50MOVE:
            gameMsg = "Draw by 50 move rule.\n";
            break;
        case GR_3FOLD:
            gameMsg = "Draw by threefold repetition.\n";
            break;
        case GR_INSUFFICIENT:
            gameMsg = "Draw by insufficient material.\n";
            break;
        case GR_CONTINUE:
            cont = true;
            break;
        }
        if (!cont) {
            cout << "GAME> " << gameMsg;
            break;
        }

        validMove = false;
        while (!validMove) {
            cout << "GAME> Make your move ("
                 << (board.getSideToPlay() == PC_WHITE ? "white" : "black")
                 << "):\nPLAYER> ";
            cin >> inputMove;
            try {
                Move move = board.parseMove(inputMove);
                board.makeMove(move);
                validMove = true;
            } catch (const invalid_argument& err) {
                cout << "GAME> " << err.what() << '\n';
            }
        }

        board.flip();
    }
}

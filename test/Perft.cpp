#include "Board.hpp"
#include <iostream>

using std::cout, std::cin;
using u64 = unsigned long long;

u64 perft(int depth, Board& board) {
    if (depth == 0) return 1;
    u64 nodes = 0;

    auto pseudo = board.generatePseudoLegalMoves();
    for (const auto& [_, move] : pseudo) {
        board.makeMove(move);
        if (!board.isInCheck()) {
            board.flip();
            nodes += perft(depth - 1, board);
            board.flip();
        }
        board.unmakeMove();
    }

    return nodes;
}

int main() {
    Board board;
    int depth;
    cout << "Depth: ";
    cin >> depth;
    cout << "Perft (depth" << depth << "): " << perft(depth, board) << '\n';
}

#include "Board.hpp"
#include <iostream>

using std::cout, std::cin;
using u64 = unsigned long long;

u64 perft(int depth, bool listNodes, Board& board) {
    if (depth == 0) return 1;
    u64 nodes, totalNodes = 0;

    auto pseudo = board.generatePseudoLegalMoves();
    for (const auto& [notation, move] : pseudo) {
        board.makeMove(move);
        if (!board.isInCheck()) {
            board.flip();
            nodes = perft(depth - 1, false, board);
            board.flip();
            totalNodes += nodes;
            if (listNodes) cout << notation << ": " << nodes << '\n';
        }
        board.unmakeMove();
    }

    if (listNodes) cout << "\nNodes searched: " << totalNodes << '\n';
    return totalNodes;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: perft <depth>\n";
        return 1;
    }

    Board board;
    int depth = atoi(argv[1]);
    perft(depth, true, board);
}

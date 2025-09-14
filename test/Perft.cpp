#include "../src/board/Board.h"

using u64 = unsigned long long;

u64 perft(int depth, Board& board) {
    if (depth == 0) return 1;
    u64 nodes = 0;

    auto pseudo = board.generatePseudoLegalMoves();
    for (const auto& [_, move] : pseudo) {
        board.makeMove(move);
        if (!board.isInCheck()) nodes += perft(depth - 1, board);
        board.unmakeMove();
    }

    return nodes;
}

int main() {}

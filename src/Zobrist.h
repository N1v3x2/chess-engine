#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include "Piece.h"
#include <array>
#include <random>
#include <unordered_map>

using namespace pieceutils;
using std::array;
using std::unordered_map;
using hash_t = unsigned long long;

namespace zobrist {

array<array<hash_t, 64>, 12> pieceSquare;
// 0: white kingside, 1: white queenside, 2: black kingside, 3: black queenside
array<hash_t, 4> castleRights;
// NOTE: technically I only need 8 numbers, one for each file, but `GameState`
// tracks the exact ep sqaure and I'm too lazy to update that logic right now
array<hash_t, 64> epSquare;
array<hash_t, 2> sideToMove;

void init() {
    // Generate random integers between 0 and INT64_MAX
    std::random_device rd;
    std::mt19937_64 gen(rd);

    for (int i = 0; i < 12; ++i)
        for (int j = 0; j < 64; ++j)
            pieceSquare[i][j] = gen();
    for (auto& e : castleRights)
        e = gen();
    for (auto& e : epSquare)
        e = gen();
    for (auto& e : sideToMove)
        e = gen();
}

// Get the piece's index inside the `pieceSquare` array's second dimension
unsigned int getPieceIdx(Piece piece) {
    return getPieceType(piece) + (getPieceColor(piece) == PC_WHITE ? 5 : 0);
}

} // namespace zobrist

#endif // !TRANSPOSITION_TABLE_H

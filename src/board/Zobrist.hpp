#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include "Piece.hpp"
#include <array>
#include <random>
#include <unordered_map>

using namespace pieceutils;
using std::array;
using std::unordered_map;

namespace zobrist {

using zhash_t = unsigned long long;

array<array<zhash_t, 12>, 64> pieceSquare;

// 0: white kingside, 1: white queenside, 2: black kingside, 3: black queenside
array<zhash_t, 4> castleRights;

// NOTE: technically I only need 8 numbers, one for each file, but `GameState`
// tracks the exact ep sqaure and I'm too lazy to update that logic right now
array<zhash_t, 64> epSquare;

zhash_t whiteToMove;

void init() {
    // Generate random integers between 0 and INT64_MAX
    std::random_device rd;
    std::mt19937_64 gen(rd());

    for (auto& square : pieceSquare)
        for (auto& piece : square)
            piece = gen();
    for (auto& e : castleRights)
        e = gen();
    for (auto& e : epSquare)
        e = gen();
    whiteToMove = gen();
}

// Get the piece's index inside the `pieceSquare` array's second dimension
unsigned int getPieceIdx(Piece piece) {
    return getPieceType(piece) - 1 + (getPieceColor(piece) == PC_WHITE ? 0 : 6);
}

} // namespace zobrist

#endif // !TRANSPOSITION_TABLE_H

#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include "GameState.h"
#include "Move.h"
#include <array>
#include <random>
#include <unordered_map>

using std::array;
using std::unordered_map;
using hash_t = unsigned long long;

namespace {

array<array<hash_t, 64>, 12> pieceSquareLookup;
array<hash_t, 8> castleRightsLookup;
array<hash_t, 8> epLookup;
array<hash_t, 2> sideToMoveLookup;

} // namespace

namespace zobrist {

void init() {
    std::random_device rd;
    std::mt19937_64 gen(rd);
    for (int i = 0; i < 12; ++i)
        for (int j = 0; j < 64; ++j)
            pieceSquareLookup[i][j] = gen();
    for (auto& e : castleRightsLookup)
        e = gen();
    for (auto& e : epLookup)
        e = gen();
    for (auto& e : sideToMoveLookup)
        e = gen();
}

void updateHash(hash_t& hash, const Move& move, const GameState& state) {}

void undoUpdateHash(hash_t& hash, const Move& move, const GameState& state) {}

} // namespace zobrist

#endif // !TRANSPOSITION_TABLE_H

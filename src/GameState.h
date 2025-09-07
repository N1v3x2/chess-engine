#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "Piece.h"
#include <array>

using std::array;

struct GameState {
  private:
    bool canWhiteQueensideCastle = true;
    bool canWhiteKingsideCastle = true;
    bool canBlackQueensideCastle = true;
    bool canBlackKingsideCastle = true;

  public:
    Piece epCapturedPiece = P_EMPTY;
    // NOTE: can optimize this to just track the file
    int epSquare = -1;
    int halfmoveClock = 0; // Capture, pawn move, castle
    bool canKingsideCastle(PieceColor side) const {
        return side == PC_WHITE ? canWhiteKingsideCastle
                                : canBlackKingsideCastle;
    }
    bool canQueensideCastle(PieceColor side) const {
        return side == PC_WHITE ? canWhiteQueensideCastle
                                : canBlackQueensideCastle;
    }
    void removeKingsideCastleRight(PieceColor side) {
        if (side == PC_WHITE)
            canWhiteKingsideCastle = false;
        else
            canBlackKingsideCastle = false;
    }
    void removeQueensideCastleRights(PieceColor side) {
        if (side == PC_WHITE)
            canWhiteQueensideCastle = false;
        else
            canBlackQueensideCastle = false;
    }

    // Diffs two gamestates for Zobrist hashing purposes
    // Non-zero elements indicate differences
    array<int, 5> diff(const GameState& other) {
        return {canWhiteKingsideCastle != other.canWhiteKingsideCastle,
                canWhiteQueensideCastle != other.canWhiteQueensideCastle,
                canBlackKingsideCastle != other.canBlackKingsideCastle,
                canBlackQueensideCastle != other.canBlackQueensideCastle,
                epSquare != other.epSquare ? epSquare : 0};
    }
};

#endif // !GAME_STATE_H

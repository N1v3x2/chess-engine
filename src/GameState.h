#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "Piece.h"

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
    void removeKingsideCastlingRights(PieceColor side) {
        if (side == PC_WHITE)
            canWhiteKingsideCastle = false;
        else
            canBlackKingsideCastle = false;
    }
    void removeQueensideCastlingRights(PieceColor side) {
        if (side == PC_WHITE)
            canWhiteQueensideCastle = false;
        else
            canBlackQueensideCastle = false;
    }
};

#endif // !GAME_STATE_H

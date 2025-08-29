#ifndef MOVE_H
#define MOVE_H

#include "Piece.h"
#include <string>

using std::string;
using std::to_string;

using ui = unsigned int;

enum MoveType {
    MT_QUIET = 0b0000,
    MT_DOUBLE_PAWN_PUSH = 0b0001,
    MT_CASTLE_KING = 0b0010,
    MT_CASTLE_QUEEN = 0b0011,
    MT_CAPTURE = 0b0100,
    MT_CAPTURE_EP = 0b0101,
    MT_PROMOTION_KNIGHT = 0b1000,
    MT_PROMOTION_BISHOP = 0b1001,
    MT_PROMOTION_ROOK = 0b1010,
    MT_PROMOTION_QUEEN = 0b1011,
};

class Move {
  private:
    ui from, to, flags;
    Piece fromPiece, toPiece;

    static string getFile(ui square) { return string(1, 'a' + square % 8); }
    static string getRank(ui square) { return to_string(square / 8 + 1); }

  public:
    Move() : from(0), to(0), flags(0), fromPiece(P_EMPTY), toPiece(P_EMPTY) {}

    Move(ui from, ui to, int flags, Piece p1, Piece p2)
        : from(from), to(to), flags(flags), fromPiece(p1), toPiece(p2) {}

    Piece getFromPiece() { return fromPiece; }
    PieceType getFromPieceType() { return getPieceType(fromPiece); }
    Piece getToPiece() { return toPiece; }
    ui getFromSquare() { return from; }
    ui getToSquare() { return to; }

    bool isDoublePawnPush() {
        return (flags & MT_DOUBLE_PAWN_PUSH) == MT_DOUBLE_PAWN_PUSH;
    }
    bool isKingSideCastle() {
        return (flags & MT_CASTLE_KING) == MT_CASTLE_KING;
    }
    bool isQueenSideCastle() {
        return (flags & MT_CASTLE_QUEEN) == MT_CASTLE_QUEEN;
    }
    bool isCapture() { return (flags & MT_CAPTURE) == MT_CAPTURE; }
    bool isEnpassantCapture() {
        return (flags & MT_CAPTURE_EP) == MT_CAPTURE_EP;
    }
    bool isKnightPromotion() {
        return (flags & MT_PROMOTION_KNIGHT) == MT_PROMOTION_KNIGHT;
    }
    bool isBishopPromotion() {
        return (flags & MT_PROMOTION_BISHOP) == MT_PROMOTION_BISHOP;
    }
    bool isRookPromotion() {
        return (flags & MT_PROMOTION_ROOK) == MT_PROMOTION_ROOK;
    }
    bool isQueenPromotion() {
        return (flags & MT_PROMOTION_QUEEN) == MT_PROMOTION_QUEEN;
    }

    string getCoordinateNotation() {
        string fromSquare = getFile(from) + getRank(from);
        string toSquare = getFile(to) + getRank(to);
        string promotionPiece;
        if (isKnightPromotion())
            promotionPiece = "n";
        else if (isBishopPromotion())
            promotionPiece = "b";
        else if (isRookPromotion())
            promotionPiece = "r";
        else if (isQueenPromotion())
            promotionPiece = "q";
        return fromSquare + toSquare + promotionPiece;
    }
};

#endif // !MOVE_H

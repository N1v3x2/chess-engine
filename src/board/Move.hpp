#ifndef MOVE_H
#define MOVE_H

#include "Piece.hpp"
#include <string>

using pieceutils::getPieceType;
using std::string;
using std::to_string;
using u32 = unsigned int;

namespace moveutils {

u32 convertStringToSquare(const string& square) {
    return 8 * (square[1] - '1') + (square[0] - 'a');
}

} // namespace moveutils

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
    u32 from, to, flags;
    Piece fromPiece, toPiece;

    static string getFile(u32 square) { return string(1, 'a' + square % 8); }
    static string getRank(u32 square) { return to_string(square / 8 + 1); }

  public:
    Move() : from(0), to(0), flags(0), fromPiece(P_EMPTY), toPiece(P_EMPTY) {}

    Move(u32 from, u32 to, int flags, Piece p1, Piece p2)
        : from(from), to(to), flags(flags), fromPiece(p1), toPiece(p2) {}

    Piece getFromPiece() const { return fromPiece; }
    PieceType getFromPieceType() const { return getPieceType(fromPiece); }
    Piece getToPiece() const { return toPiece; }
    u32 getFromSquare() const { return from; }
    u32 getToSquare() const { return to; }

    bool isDoublePawnPush() const { return flags == MT_DOUBLE_PAWN_PUSH; }
    bool isKingSideCastle() const { return flags == MT_CASTLE_KING; }
    bool isQueenSideCastle() const { return flags == MT_CASTLE_QUEEN; }
    bool isCapture() const { return (flags & MT_CAPTURE) == MT_CAPTURE; }
    bool isEnpassantCapture() const { return flags == MT_CAPTURE_EP; }
    bool isKnightPromotion() const {
        return (flags & ~MT_CAPTURE) == MT_PROMOTION_KNIGHT;
    }
    bool isBishopPromotion() const {
        return (flags & ~MT_CAPTURE) == MT_PROMOTION_BISHOP;
    }
    bool isRookPromotion() const {
        return (flags & ~MT_CAPTURE) == MT_PROMOTION_ROOK;
    }
    bool isQueenPromotion() const {
        return (flags & ~MT_CAPTURE) == MT_PROMOTION_QUEEN;
    }

    string getCoordinateNotation() const {
        string fromSquare = getFile(from) + getRank(from);
        string toSquare = getFile(to) + getRank(to);
        string promotionPiece;
        if (isQueenPromotion())
            promotionPiece = "q";
        else if (isRookPromotion())
            promotionPiece = "r";
        else if (isBishopPromotion())
            promotionPiece = "b";
        else if (isKnightPromotion())
            promotionPiece = "n";
        return fromSquare + toSquare + promotionPiece;
    }
};

#endif // !MOVE_H

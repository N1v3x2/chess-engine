#ifndef PIECE_H
#define PIECE_H

constexpr unsigned int BOFFSET = 8;

enum PieceType {
    PT_EMPTY = 0,
    PT_PAWN = 1,
    PT_KNIGHT = 2,
    PT_BISHOP = 3,
    PT_ROOK = 4,
    PT_QUEEN = 5,
    PT_KING = 6
};

enum PieceColor {
    PC_WHITE = 0,
    PC_BLACK = 1,
};

enum Piece {
    // P_INVALID = PT_INVALID,
    P_EMPTY = PT_EMPTY,

    P_WPAWN = PT_PAWN,
    P_WKNIGHT = PT_KNIGHT,
    P_WBISHOP = PT_BISHOP,
    P_WROOK = PT_ROOK,
    P_WQUEEN = PT_QUEEN,
    P_WKING = PT_KING,

    P_BPAWN = BOFFSET | PT_PAWN,
    P_BKNIGHT = BOFFSET | PT_KNIGHT,
    P_BBISHOP = BOFFSET | PT_BISHOP,
    P_BROOK = BOFFSET | PT_ROOK,
    P_BQUEEN = BOFFSET | PT_QUEEN,
    P_BKING = BOFFSET | PT_KING,
};

constexpr PieceColor getPieceColor(Piece p) {
    return (p & BOFFSET) ? PC_BLACK : PC_WHITE;
}
constexpr PieceType getPieceType(Piece p) { return (PieceType)(p & ~BOFFSET); }

#endif // !PIECE_H

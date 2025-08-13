#include <iostream>

using std::cout, std::endl;

constexpr unsigned int BOFFSET = 8;

enum PieceType {
    // PT_INVALID = -1,
    PT_EMPTY = 0,
    PT_PAWN = 1,
    PT_KNIGHT = 2,
    PT_BISHOP = 3,
    PT_ROOK = 4,
    PT_QUEEN = 5,
    PT_KING = 6
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

bool isWhite(Piece p) { return p & BOFFSET; }

class Board {
  private:
    int mailbox[120] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, 0,  1,  2,  3,  4,  5,  6,  7,  -1, -1, 8,  9,  10, 11, 12,
        13, 14, 15, -1, -1, 16, 17, 18, 19, 20, 21, 22, 23, -1, -1, 24, 25, 26,
        27, 28, 29, 30, 31, -1, -1, 32, 33, 34, 35, 36, 37, 38, 39, -1, -1, 40,
        41, 42, 43, 44, 45, 46, 47, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, -1,
        -1, 56, 57, 58, 59, 60, 61, 62, 63, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    unsigned int mailbox64[64] = {
        21, 22, 23, 24, 25, 26, 27, 28, 31, 32, 33, 34, 35, 36, 37, 38,
        41, 42, 43, 44, 45, 46, 47, 48, 51, 52, 53, 54, 55, 56, 57, 58,
        61, 62, 63, 64, 65, 66, 67, 68, 71, 72, 73, 74, 75, 76, 77, 78,
        81, 82, 83, 84, 85, 86, 87, 88, 91, 92, 93, 94, 95, 96, 97, 98};

    // pawn, knight, bishop, rook, queen, king
    bool doesSlide[6] = {false, false, true, true, true, false};
    unsigned int numOffsets[6] = {0, 8, 4, 4, 8, 8};
    int offsets[6][8] = {
        {0, 0, 0, 0, 0, 0, 0, 0},         {-21, -19, -12, -8, 8, 12, 19, 21},
        {-11, -9, 9, 11, 0, 0, 0, 0},     {-10, -1, 1, 10, 0, 0, 0, 0},
        {-11, -10, -9, -1, 1, 9, 10, 11}, {-11, -10, -9, -1, 1, 9, 10, 11}};

    bool isWhiteTurn;
    // No piece list for now
    Piece currentBoard[64] = {
        P_WROOK, P_WKNIGHT, P_WBISHOP, P_WQUEEN, P_WKING, P_WBISHOP, P_WKNIGHT,
        P_WROOK, P_WPAWN,   P_WPAWN,   P_WPAWN,  P_WPAWN, P_WPAWN,   P_WPAWN,
        P_WPAWN, P_WPAWN,   P_EMPTY,   P_EMPTY,  P_EMPTY, P_EMPTY,   P_EMPTY,
        P_EMPTY, P_EMPTY,   P_EMPTY,   P_EMPTY,  P_EMPTY, P_EMPTY,   P_EMPTY,
        P_EMPTY, P_EMPTY,   P_EMPTY,   P_EMPTY,  P_EMPTY, P_EMPTY,   P_EMPTY,
        P_EMPTY, P_EMPTY,   P_EMPTY,   P_EMPTY,  P_EMPTY, P_EMPTY,   P_EMPTY,
        P_EMPTY, P_EMPTY,   P_EMPTY,   P_EMPTY,  P_EMPTY, P_EMPTY,   P_BPAWN,
        P_BPAWN, P_BPAWN,   P_BPAWN,   P_BPAWN,  P_BPAWN, P_BPAWN,   P_BPAWN,
        P_BROOK, P_BKNIGHT, P_BBISHOP, P_BQUEEN, P_BKING, P_BBISHOP, P_BKNIGHT,
        P_BROOK,
    };

  public:
};

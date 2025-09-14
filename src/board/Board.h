#ifndef BOARD_H
#define BOARD_H

#define um unordered_map

#include "Graphics.h"
#include "Move.h"
#include "Piece.h"
#include "Zobrist.h"
#include <array>
#include <cmath>
#include <iostream>
#include <stack>
#include <stdexcept>
#include <unordered_map>
#include <vector>

using namespace moveutils;
using namespace pieceutils;
using std::array;
using std::cout;
using std::invalid_argument;
using std::runtime_error;
using std::stack;
using std::um;
using std::vector;
using zobrist::getPieceIdx;
using zobrist::zhash_t;
using u32 = unsigned int;
using u64 = unsigned long long;

namespace {
constexpr array<int, 120> mailbox{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, 0,  1,  2,  3,  4,  5,  6,  7,  -1, -1, 8,  9,  10, 11, 12,
    13, 14, 15, -1, -1, 16, 17, 18, 19, 20, 21, 22, 23, -1, -1, 24, 25, 26,
    27, 28, 29, 30, 31, -1, -1, 32, 33, 34, 35, 36, 37, 38, 39, -1, -1, 40,
    41, 42, 43, 44, 45, 46, 47, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, -1,
    -1, 56, 57, 58, 59, 60, 61, 62, 63, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
constexpr array<int, 64> mailbox64{
    21, 22, 23, 24, 25, 26, 27, 28, 31, 32, 33, 34, 35, 36, 37, 38,
    41, 42, 43, 44, 45, 46, 47, 48, 51, 52, 53, 54, 55, 56, 57, 58,
    61, 62, 63, 64, 65, 66, 67, 68, 71, 72, 73, 74, 75, 76, 77, 78,
    81, 82, 83, 84, 85, 86, 87, 88, 91, 92, 93, 94, 95, 96, 97, 98};

// pawn, knight, bishop, rook, queen, king
constexpr array<bool, 6> doesSlide{false, false, true, true, true, false};
constexpr array<u32, 6> numOffsets{0, 8, 4, 4, 8, 8};
constexpr array<array<int, 8>, 6> offsets{{{0, 0, 0, 0, 0, 0, 0, 0},
                                           {-21, -19, -12, -8, 8, 12, 19, 21},
                                           {-11, -9, 9, 11, 0, 0, 0, 0},
                                           {-10, -1, 1, 10, 0, 0, 0, 0},
                                           {-11, -10, -9, -1, 1, 9, 10, 11},
                                           {-11, -10, -9, -1, 1, 9, 10, 11}}};
} // namespace

enum GameResult {
    GR_CONTINUE,
    GR_CHECKMATE,
    GR_STALEMATE,
    GR_3FOLD,
    GR_50MOVE,
    GR_INSUFFICIENT // insufficient material
};

class Board {
  private:
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
        array<bool, 4> diff(const GameState& other) {
            return {canWhiteKingsideCastle != other.canWhiteKingsideCastle,
                    canWhiteQueensideCastle != other.canWhiteQueensideCastle,
                    canBlackKingsideCastle != other.canBlackKingsideCastle,
                    canBlackQueensideCastle != other.canBlackQueensideCastle};
        }
    };

    PieceColor sideToMove; // 0 = white, 1 = black
    // No piece list for now
    array<Piece, 64> board{
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
    um<string, Move> legalMoves;
    vector<Move> gameList;
    stack<GameState> gameStateStack;
    um<zhash_t, int> transpositionTable;
    zhash_t positionHash;

    bool isAttacked(u32 square) {
        // Superpiece
        int from;
        for (int pieceType = 1; pieceType < 6; ++pieceType) {
            for (u32 j = 0; j < numOffsets[pieceType]; ++j) {
                for (u32 k = 1;; ++k) {
                    from =
                        mailbox[mailbox64[square] + k * offsets[pieceType][j]];
                    // Reached friendly piece or edge of board
                    if (from == -1 ||
                        (board[from] != P_EMPTY &&
                         getPieceColor(board[from]) == sideToMove))
                        break;

                    if (getPieceType(board[from]) ==
                            (PieceType)(pieceType + 1) &&
                        getPieceColor(board[from]) != sideToMove)
                        return true;

                    if (!doesSlide[pieceType]) break;
                }
            }
        }
        // Check pawn attacks
        int leftCaptureOffset = sideToMove == PC_WHITE ? 9 : -11;
        int rightCaptureOffset = sideToMove == PC_WHITE ? 11 : -9;

        for (auto captureOffset : {leftCaptureOffset, rightCaptureOffset}) {
            from = mailbox[mailbox64[square] + captureOffset];
            if (from != -1 && getPieceType(board[from]) == PT_PAWN &&
                getPieceColor(board[from]) != sideToMove)
                return true;
        }

        // Check En Passant attacks
        if (gameStateStack.top().epSquare == (int)square) {
            for (auto captureOffset : {-1, 1}) {
                from = mailbox[mailbox64[square] + captureOffset];
                if (from != -1 && getPieceType(board[from]) == PT_PAWN &&
                    getPieceColor(board[from]) != sideToMove)
                    return true;
            }
        }

        return false;
    }

    bool isInCheck() {
        int kingSquare = -1;
        for (u32 i = 0; i < 64; ++i) {
            if ((sideToMove == PC_WHITE && board[i] == P_WKING) ||
                (sideToMove == PC_BLACK && board[i] == P_BKING)) {
                kingSquare = i;
                break;
            }
        }
        if (kingSquare == -1) throw runtime_error("King not found");
        return isAttacked(kingSquare);
    }

    um<string, Move> generatePseudoLegalMoves() {
        um<string, Move> moves;

        // Pawn offsets
        int pushOffset = sideToMove == PC_WHITE ? 10 : -10;
        int leftCaptureOffset = sideToMove == PC_WHITE ? 9 : -11;
        int rightCaptureOffset = sideToMove == PC_WHITE ? 11 : -9;

        for (u32 from = 0; from < 64; ++from) {
            PieceType pieceType = getPieceType(board[from]);
            if (pieceType == PT_EMPTY) continue;
            if (getPieceColor(board[from]) == sideToMove) {
                int to;
                if (pieceType == PT_PAWN) {
                    // Push
                    to = mailbox[mailbox64[from] + pushOffset];
                    if (to != -1 && getPieceType(board[to]) == PT_EMPTY) {
                        // Promotion
                        if ((sideToMove == PC_BLACK && to < 8) ||
                            (sideToMove == PC_WHITE && to >= 56)) {
                            Move m(from, to, MT_PROMOTION_KNIGHT, board[from],
                                   board[to]);
                            moves.emplace(m.getCoordinateNotation(), m);

                            m = Move(from, to, MT_PROMOTION_BISHOP, board[from],
                                     board[to]);
                            moves.emplace(m.getCoordinateNotation(), m);

                            m = Move(from, to, MT_PROMOTION_ROOK, board[from],
                                     board[to]);
                            moves.emplace(m.getCoordinateNotation(), m);

                            m = Move(from, to, MT_PROMOTION_QUEEN, board[from],
                                     board[to]);
                            moves.emplace(m.getCoordinateNotation(), m);
                        } else {
                            Move m(from, to, MT_QUIET, board[from], board[to]);
                            moves.emplace(m.getCoordinateNotation(), m);
                        }

                        // Double push
                        if ((sideToMove == PC_BLACK && from >= 48 &&
                             from < 56) ||
                            (sideToMove == PC_WHITE && from >= 8 &&
                             from < 16)) {
                            to = mailbox[mailbox64[from] + 2 * pushOffset];
                            if (getPieceType(board[to]) == PT_EMPTY) {
                                Move m(from, to, MT_DOUBLE_PAWN_PUSH,
                                       board[from], board[to]);
                                moves.emplace(m.getCoordinateNotation(), m);
                            }
                        }
                    }

                    // Captures (white's perspective)
                    for (auto captureOffset :
                         {leftCaptureOffset, rightCaptureOffset}) {

                        to = mailbox[mailbox64[from] + captureOffset];
                        if (to != -1 && getPieceType(board[to]) != PT_EMPTY &&
                            getPieceColor(board[to]) != sideToMove) {
                            // Promotion
                            if ((sideToMove == PC_BLACK && to < 8) ||
                                (sideToMove == PC_WHITE && to >= 56)) {
                                Move m(from, to,
                                       MT_PROMOTION_KNIGHT | MT_CAPTURE,
                                       board[from], board[to]);
                                moves.emplace(m.getCoordinateNotation(), m);

                                m = Move(from, to,
                                         MT_PROMOTION_BISHOP | MT_CAPTURE,
                                         board[from], board[to]);
                                moves.emplace(m.getCoordinateNotation(), m);

                                m = Move(from, to,
                                         MT_PROMOTION_ROOK | MT_CAPTURE,
                                         board[from], board[to]);
                                moves.emplace(m.getCoordinateNotation(), m);

                                m = Move(from, to,
                                         MT_PROMOTION_QUEEN | MT_CAPTURE,
                                         board[from], board[to]);
                                moves.emplace(m.getCoordinateNotation(), m);
                            } else {
                                Move m(from, to, MT_CAPTURE, board[from],
                                       board[to]);
                                moves.emplace(m.getCoordinateNotation(), m);
                            }
                        }
                    }

                    // En passant
                    if (gameStateStack.top().epSquare != -1) {
                        int epSquare = gameStateStack.top().epSquare;
                        if (abs(gameStateStack.top().epSquare - (int)from) ==
                            1) {
                            to = epSquare + (sideToMove == PC_WHITE ? 8 : -8);
                            Move m(from, to, MT_CAPTURE_EP, board[from],
                                   board[epSquare]);
                            moves.emplace(m.getCoordinateNotation(), m);
                        }
                    }
                } else {
                    for (u32 j = 0; j < numOffsets[pieceType - 1]; ++j) {
                        for (u32 k = 1;; ++k) {
                            to = mailbox[mailbox64[from] +
                                         k * offsets[pieceType - 1][j]];
                            if (to == -1) break; // Move would go off the board

                            if (getPieceType(board[to]) == PT_EMPTY) {
                                Move m(from, to, MT_QUIET, board[from],
                                       board[to]);
                                moves.emplace(m.getCoordinateNotation(), m);
                            } else if (getPieceColor(board[to]) == sideToMove) {
                                break;
                            } else {
                                Move m(from, to, MT_CAPTURE, board[from],
                                       board[to]);
                                moves.emplace(m.getCoordinateNotation(), m);
                            }

                            if (!doesSlide[pieceType - 1]) break;
                        }
                    }
                    if (pieceType == PT_KING && !isAttacked(from)) {
                        // NOTE: cannot castle out of check
                        // NOTE: cannot castle through attacked square; however,
                        // squares outside the king's castle path can be
                        // attacked
                        GameState state = gameStateStack.top();
                        vector<u32> kingSquares;
                        bool pathEmpty;

                        if (state.canKingsideCastle(sideToMove)) {
                            if (sideToMove == PC_WHITE)
                                kingSquares = {5, 6};
                            else
                                kingSquares = {61, 62};

                            pathEmpty = true;
                            for (auto square : kingSquares) {
                                if (getPieceType(board[square]) != PT_EMPTY ||
                                    isAttacked(square)) {
                                    pathEmpty = false;
                                    break;
                                }
                            }
                            if (pathEmpty) {
                                Move m(from, from + 2, MT_CASTLE_KING,
                                       board[from], board[to]);
                                moves.emplace(m.getCoordinateNotation(), m);
                            }
                        }
                        if (state.canQueensideCastle(sideToMove)) {
                            if (sideToMove == PC_WHITE)
                                kingSquares = {2, 3};
                            else
                                kingSquares = {58, 59};

                            pathEmpty = true;
                            for (auto square : kingSquares) {
                                if (getPieceType(board[square]) != PT_EMPTY ||
                                    isAttacked(square)) {
                                    pathEmpty = false;
                                    break;
                                }
                            }
                            if (sideToMove == PC_WHITE &&
                                getPieceType(board[1]) != PT_EMPTY)
                                pathEmpty = false;

                            if (sideToMove == PC_BLACK &&
                                getPieceType(board[57]) != PT_EMPTY)
                                pathEmpty = false;

                            if (pathEmpty) {
                                Move m(from, from - 2, MT_CASTLE_QUEEN,
                                       board[from], board[to]);
                                moves.emplace(m.getCoordinateNotation(), m);
                            }
                        }
                    }
                }
            }
        }

        return moves;
    }

  public:
    Board() : sideToMove(PC_WHITE) {
        gameStateStack.emplace();

        // Initialize Zobrist hash
        positionHash = zobrist::whiteToMove;
        for (u32 i = 0; i < 4; ++i) {
            positionHash ^= zobrist::castleRights[i];
        }
        // Don't include empty squares in hash
        for (u32 i = 0; i < 16; ++i) {
            positionHash ^= zobrist::pieceSquare[i][getPieceIdx(board[i])];
        }
        for (u32 i = 48; i < 64; ++i) {
            positionHash ^= zobrist::pieceSquare[i][getPieceIdx(board[i])];
        }

        transpositionTable[positionHash] = 1;
    }

    friend u64 perft(int depth, Board& board);

    void flip() {
        sideToMove = (PieceColor)!sideToMove;
        positionHash ^= zobrist::whiteToMove;
        transpositionTable[positionHash]++;
    }
    PieceColor getSideToPlay() const { return sideToMove; }

    GameResult checkGameResult() {
        if (legalMoves.size() == 0 && isInCheck()) return GR_CHECKMATE;

        // Threefold repetition
        if (transpositionTable.count(positionHash) &&
            transpositionTable.at(positionHash) >= 3)
            return GR_3FOLD;

        // 50-move rule
        if (gameStateStack.top().halfmoveClock >= 50) return GR_50MOVE;

        // Stalemate
        if (legalMoves.size() == 0 && !isInCheck()) return GR_STALEMATE;

        // TODO: check insufficient material

        return GR_CONTINUE;
    }

    // No legality checks for now. Let's assume the move is legal
    void makeMove(const Move& move) {
        // Distinction between from/to necessary in case of promotion
        Piece fromPiece, toPiece;
        GameState newState = gameStateStack.top();

        if (newState.epSquare != -1) {
            positionHash ^= zobrist::epSquare[newState.epSquare];
            newState.epSquare = -1;
        }

        if (move.getFromPieceType() == PT_PAWN) {
            newState.halfmoveClock = 0;
            toPiece = fromPiece = move.getFromPiece();

            if (move.isDoublePawnPush()) {
                newState.epSquare = move.getToSquare();
                positionHash ^= zobrist::epSquare[newState.epSquare];
            }

            if (move.isEnpassantCapture()) {
                int offset = sideToMove == PC_WHITE ? -8 : 8;
                u32 epSquare = move.getToSquare() + offset;
                newState.epCapturedPiece = board[epSquare];
                board[epSquare] = P_EMPTY;
                positionHash ^=
                    zobrist::pieceSquare[epSquare]
                                        [getPieceIdx(board[epSquare])];
            }

            if (move.isKnightPromotion())
                toPiece = sideToMove == PC_WHITE ? P_WKNIGHT : P_BKNIGHT;
            else if (move.isBishopPromotion())
                toPiece = sideToMove == PC_WHITE ? P_WBISHOP : P_BBISHOP;
            else if (move.isRookPromotion())
                toPiece = sideToMove == PC_WHITE ? P_WROOK : P_BROOK;
            else if (move.isQueenPromotion())
                toPiece = sideToMove == PC_WHITE ? P_WQUEEN : P_BQUEEN;
        } else {
            newState.halfmoveClock += 1;
            toPiece = fromPiece = move.getFromPiece();

            if (move.getFromPieceType() == PT_KING) {
                if (newState.canKingsideCastle(sideToMove)) {
                    positionHash ^=
                        zobrist::castleRights[sideToMove == PC_WHITE ? 0 : 2];
                    newState.removeKingsideCastleRight(sideToMove);
                }
                if (newState.canQueensideCastle(sideToMove)) {
                    positionHash ^=
                        zobrist::castleRights[sideToMove == PC_WHITE ? 1 : 3];
                    newState.removeQueensideCastleRights(sideToMove);
                }

                Piece rook = sideToMove == PC_WHITE ? P_WROOK : P_BROOK;
                if (move.isKingSideCastle()) {
                    newState.halfmoveClock = 0;

                    positionHash ^= zobrist::pieceSquare[move.getToSquare() - 1]
                                                        [getPieceIdx(rook)] ^
                                    zobrist::pieceSquare[move.getToSquare() + 1]
                                                        [getPieceIdx(rook)];
                    board[move.getToSquare() - 1] = rook;
                    board[move.getToSquare() + 1] = P_EMPTY;
                }

                if (move.isQueenSideCastle()) {
                    newState.halfmoveClock = 0;

                    positionHash ^= zobrist::pieceSquare[move.getToSquare() + 1]
                                                        [getPieceIdx(rook)] ^
                                    zobrist::pieceSquare[move.getToSquare() - 2]
                                                        [getPieceIdx(rook)];
                    board[move.getToSquare() + 1] = rook;
                    board[move.getToSquare() - 2] = P_EMPTY;
                }
            }

            if (newState.canKingsideCastle(PC_WHITE) &&
                (move.getFromSquare() == 7 || move.getToSquare() == 7)) {
                positionHash ^= zobrist::castleRights[0];
                newState.removeKingsideCastleRight(PC_WHITE);
            }

            if (newState.canQueensideCastle(PC_WHITE) &&
                (move.getFromSquare() == 0 || move.getToSquare() == 0)) {
                positionHash ^= zobrist::castleRights[1];
                newState.removeQueensideCastleRights(PC_WHITE);
            }

            if (newState.canKingsideCastle(PC_BLACK) &&
                (move.getFromSquare() == 63 || move.getToSquare() == 63)) {
                positionHash ^= zobrist::castleRights[2];
                newState.removeKingsideCastleRight(PC_BLACK);
            }

            if (newState.canQueensideCastle(PC_BLACK) &&
                (move.getFromSquare() == 56 || move.getToSquare() == 56)) {
                positionHash ^= zobrist::castleRights[3];
                newState.removeQueensideCastleRights(PC_BLACK);
            }
        }

        positionHash ^=
            zobrist::pieceSquare[move.getFromSquare()][getPieceIdx(fromPiece)] ^
            zobrist::pieceSquare[move.getToSquare()][getPieceIdx(toPiece)];
        if (move.isCapture())
            positionHash ^=
                zobrist::pieceSquare[move.getToSquare()]
                                    [getPieceIdx(move.getToPiece())];

        board[move.getToSquare()] = fromPiece;
        board[move.getFromSquare()] = P_EMPTY;

        gameStateStack.push(newState);
        gameList.push_back(move);
        transpositionTable[positionHash]++;
    }

    void unmakeMove() {
        GameState state = gameStateStack.top();
        gameStateStack.pop();
        Move move = gameList.back();
        gameList.pop_back();

        // Erase previous move's hash
        if (--transpositionTable[positionHash] == 0)
            transpositionTable.erase(positionHash);

        auto diff = state.diff(gameStateStack.top());
        for (int i = 0; i < 4; ++i)
            if (diff[i]) positionHash ^= zobrist::castleRights[i];

        if (state.epSquare != -1)
            positionHash ^= zobrist::epSquare[state.epSquare];
        if (gameStateStack.top().epSquare != -1)
            positionHash ^= zobrist::epSquare[gameStateStack.top().epSquare];

        if (move.isEnpassantCapture()) {
            int offset = sideToMove == PC_WHITE ? -8 : 8;
            positionHash ^=
                zobrist::pieceSquare[move.getToSquare() + offset]
                                    [getPieceIdx(state.epCapturedPiece)];
            board[move.getToSquare() + offset] = state.epCapturedPiece;
        }

        Piece rook = sideToMove == PC_WHITE ? P_WROOK : P_BROOK;
        if (move.isKingSideCastle()) {
            positionHash ^=
                zobrist::pieceSquare[move.getToSquare() - 1]
                                    [getPieceIdx(rook)] ^
                zobrist::pieceSquare[move.getToSquare() + 1][getPieceIdx(rook)];
            board[move.getToSquare() - 1] = P_EMPTY;
            board[move.getToSquare() + 1] = rook;
        }
        if (move.isQueenSideCastle()) {
            positionHash ^=
                zobrist::pieceSquare[move.getToSquare() + 1]
                                    [getPieceIdx(rook)] ^
                zobrist::pieceSquare[move.getToSquare() - 2][getPieceIdx(rook)];
            board[move.getToSquare() + 1] = P_EMPTY;
            board[move.getToSquare() - 2] = rook;
        }

        Piece fromPiece = move.getFromPiece();
        // Handle promotions
        Piece toPiece = fromPiece;
        if (move.isKnightPromotion())
            toPiece = sideToMove == PC_WHITE ? P_WKNIGHT : P_BKNIGHT;
        else if (move.isBishopPromotion())
            toPiece = sideToMove == PC_WHITE ? P_WBISHOP : P_BBISHOP;
        else if (move.isRookPromotion())
            toPiece = sideToMove == PC_WHITE ? P_WROOK : P_BROOK;
        else if (move.isQueenPromotion())
            toPiece = sideToMove == PC_WHITE ? P_WQUEEN : P_BQUEEN;

        positionHash ^=
            zobrist::pieceSquare[move.getFromSquare()][getPieceIdx(fromPiece)] ^
            zobrist::pieceSquare[move.getToSquare()][getPieceIdx(toPiece)];
        if (move.isCapture()) {
            positionHash ^=
                zobrist::pieceSquare[move.getToSquare()]
                                    [getPieceIdx(move.getToPiece())];
        }

        board[move.getFromSquare()] = move.getFromPiece();
        board[move.getToSquare()] = move.getToPiece();
    }

    // Possible optimization: use linked list to efficiently remove
    // illegal moves
    void generateMoves() {
        auto pseudo = generatePseudoLegalMoves();
        um<string, Move> legal;

        for (auto& [notation, move] : pseudo) {
            makeMove(move);
            if (!isInCheck()) legal.emplace(notation, move);
            unmakeMove();
        }

        legalMoves = std::move(legal);
    }

    // IO
    void printBoard() {
        cout << graphics::CLEAR << graphics::HOME << '\n';
        int k;
        bool darkBg, darkPiece;
        for (int i = 7; i >= 0; --i) {
            cout << " " << i + 1 << " ";
            for (int j = 0; j < 8; ++j) {
                k = 8 * i + j;
                darkBg = (k + i) % 2;
                darkPiece = getPieceColor(board[k]) == PC_BLACK;
                cout << (darkBg ? graphics::DARK_SQUARE
                                : graphics::LIGHT_SQUARE)
                     << " "
                     << (darkPiece ? graphics::DARK_TEXT : graphics::LIGHT_TEXT)
                     << graphics::glyph.at(getPieceType(board[k])) << " "
                     << graphics::RESET;
            }
            cout << '\n';
        }
        cout << "    a  b  c  d  e  f  g  h\n";
    }

    Move parseMove(const string& move) const {
        if (move.size() < 4 || move.size() > 5)
            throw invalid_argument("Invalid input length.");

        if (move[0] < 'a' || move[0] > 'h' || move[1] < '1' || move[1] > '8')
            throw invalid_argument("Invalid from square.");

        if (move[2] < 'a' || move[2] > 'h' || move[3] < '1' || move[3] > '8')
            throw invalid_argument("Invalid to square.");

        int from = convertStringToSquare(move.substr(0, 2));
        int to = convertStringToSquare(move.substr(2, 2));

        if (from < 0 || from > 63)
            throw invalid_argument("From square out of bounds.");

        if (to < 0 || to > 63)
            throw invalid_argument("To square out of bounds.");

        if (getPieceType(board[from]) == PT_EMPTY)
            throw invalid_argument("Cannot move empty square.");

        if (getPieceColor(board[from]) != sideToMove)
            throw invalid_argument("Cannot move opponent's pieces.");

        if (getPieceType(board[from]) == PT_PAWN && to >= 56 &&
            move.size() == 4)
            throw invalid_argument("Must promote pawn on last rank.");

        if (!legalMoves.count(move))
            throw invalid_argument("Must make legal move.");

        return legalMoves.at(move);
    }
};

#endif // !BOARD_H

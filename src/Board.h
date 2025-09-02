#ifndef BOARD_H
#define BOARD_H

#include "GameState.h"
#include "Graphics.h"
#include "Move.h"
#include "MoveParser.h"
#include "Piece.h"
#include <array>
#include <cmath>
#include <iostream>
#include <stack>
#include <stdexcept>
#include <unordered_map>
#include <vector>

using namespace moveparser;
using namespace pieceutils;
using std::array;
using std::cout;
using std::invalid_argument;
using std::runtime_error;
using std::stack;
using std::unordered_map;
using std::vector;
using ui = unsigned int;

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
constexpr array<ui, 6> numOffsets{0, 8, 4, 4, 8, 8};
constexpr array<array<int, 8>, 6> offsets{{{0, 0, 0, 0, 0, 0, 0, 0},
                                           {-21, -19, -12, -8, 8, 12, 19, 21},
                                           {-11, -9, 9, 11, 0, 0, 0, 0},
                                           {-10, -1, 1, 10, 0, 0, 0, 0},
                                           {-11, -10, -9, -1, 1, 9, 10, 11},
                                           {-11, -10, -9, -1, 1, 9, 10, 11}}};

class Board {
  private:
    PieceColor sideToPlay; // 0 = white, 1 = black
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
    vector<Move> gameList;
    stack<GameState> gameStateStack;

    // TODO: add map of zobrist keys to handle three-fold repetition
    // TODO: handle game end conditions (draw, lose)

    bool isAttacked(ui square) {
        // Superpiece
        int from;
        for (int pieceType = 1; pieceType < 6; ++pieceType) {
            for (ui j = 0; j < numOffsets[pieceType]; ++j) {
                for (ui k = 1;; ++k) {
                    from =
                        mailbox[mailbox64[square] + k * offsets[pieceType][j]];
                    // Reached friendly piece or edge of board
                    if (from == -1 ||
                        (board[from] != P_EMPTY &&
                         getPieceColor(board[from]) == sideToPlay))
                        break;

                    if (getPieceType(board[from]) ==
                            (PieceType)(pieceType + 1) &&
                        getPieceColor(board[from]) != sideToPlay)
                        return true;

                    if (!doesSlide[pieceType]) break;
                }
            }
        }
        // Check pawn attacks
        int leftCaptureOffset = sideToPlay == PC_WHITE ? 9 : -11;
        int rightCaptureOffset = sideToPlay == PC_WHITE ? 11 : -9;

        for (auto captureOffset : {leftCaptureOffset, rightCaptureOffset}) {
            from = mailbox[mailbox64[square] + captureOffset];
            if (from != -1 && getPieceType(board[from]) == PT_PAWN &&
                getPieceColor(board[from]) != sideToPlay)
                return true;
        }

        // Check En Passant attacks
        if (gameStateStack.top().epSquare == (int)square) {
            for (auto captureOffset : {-1, 1}) {
                from = mailbox[mailbox64[square] + captureOffset];
                if (from != -1 && getPieceType(board[from]) == PT_PAWN &&
                    getPieceColor(board[from]) != sideToPlay)
                    return true;
            }
        }

        return false;
    }

    bool isInCheck() {
        int kingSquare = -1;
        for (ui i = 0; i < 64; ++i) {
            if ((sideToPlay == PC_WHITE && board[i] == P_WKING) ||
                (sideToPlay == PC_BLACK && board[i] == P_BKING)) {
                kingSquare = i;
                break;
            }
        }
        if (kingSquare == -1) throw runtime_error("King not found");
        return isAttacked(kingSquare);
    }

    unordered_map<string, Move> generatePseudoLegalMoves() {
        unordered_map<string, Move> moves;

        // Pawn offsets
        int pushOffset = sideToPlay == PC_WHITE ? 10 : -10;
        int leftCaptureOffset = sideToPlay == PC_WHITE ? 9 : -11;
        int rightCaptureOffset = sideToPlay == PC_WHITE ? 11 : -9;

        for (ui from = 0; from < 64; ++from) {
            PieceType pieceType = getPieceType(board[from]);
            if (pieceType == PT_EMPTY) continue;
            if (getPieceColor(board[from]) == sideToPlay) {
                int to;
                if (pieceType == PT_PAWN) {
                    // Push
                    to = mailbox[mailbox64[from] + pushOffset];
                    if (to != -1 && getPieceType(board[to]) == PT_EMPTY) {
                        // Promotion
                        if ((sideToPlay == PC_BLACK && to < 8) ||
                            (sideToPlay == PC_WHITE && to >= 56)) {
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
                        if ((sideToPlay == PC_BLACK && from >= 48 &&
                             from < 56) ||
                            (sideToPlay == PC_WHITE && from >= 8 &&
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
                            getPieceColor(board[to]) != sideToPlay) {
                            // Promotion
                            if ((sideToPlay == PC_BLACK && to < 8) ||
                                (sideToPlay == PC_WHITE && to >= 56)) {
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
                            to = epSquare + (sideToPlay == PC_WHITE ? 8 : -8);
                            Move m(from, to, MT_CAPTURE_EP, board[from],
                                   board[epSquare]);
                            moves.emplace(m.getCoordinateNotation(), m);
                        }
                    }
                } else {
                    for (ui j = 0; j < numOffsets[pieceType - 1]; ++j) {
                        for (ui k = 1;; ++k) {
                            to = mailbox[mailbox64[from] +
                                         k * offsets[pieceType - 1][j]];
                            if (to == -1) break; // Move would go off the board

                            if (getPieceType(board[to]) == PT_EMPTY) {
                                Move m(from, to, MT_QUIET, board[from],
                                       board[to]);
                                moves.emplace(m.getCoordinateNotation(), m);
                            } else if (getPieceColor(board[to]) == sideToPlay) {
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
                        vector<ui> kingSquares;
                        bool pathEmpty;

                        if (state.canKingsideCastle(sideToPlay)) {
                            if (sideToPlay == PC_WHITE)
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
                        if (state.canQueensideCastle(sideToPlay)) {
                            if (sideToPlay == PC_WHITE)
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
                            if (sideToPlay == PC_WHITE &&
                                getPieceType(board[1]) != PT_EMPTY)
                                pathEmpty = false;

                            if (sideToPlay == PC_BLACK &&
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
    Board() : sideToPlay(PC_WHITE) { gameStateStack.emplace(); }

    void flipSides() { sideToPlay = (PieceColor)!sideToPlay; }

    PieceColor getSideToPlay() const { return sideToPlay; }

    // No legality checks for now. Let's assume the move is legal
    void makeMove(const Move& move) {
        Piece piece;
        GameState newState = gameStateStack.top();
        newState.epSquare = -1;

        if (move.getFromPieceType() == PT_PAWN) {
            newState.halfmoveClock = 0;
            piece = move.getFromPiece();

            if (move.isDoublePawnPush()) {
                newState.epSquare = move.getToSquare();
            }

            if (move.isEnpassantCapture()) {
                int offset = sideToPlay == PC_WHITE ? -8 : 8;
                ui epSquare = move.getToSquare() + offset;
                newState.epCapturedPiece = board[epSquare];
                board[epSquare] = P_EMPTY;
            }

            if (move.isKnightPromotion())
                piece = sideToPlay == PC_WHITE ? P_WKNIGHT : P_BKNIGHT;
            else if (move.isBishopPromotion())
                piece = sideToPlay == PC_WHITE ? P_WBISHOP : P_BBISHOP;
            else if (move.isRookPromotion())
                piece = sideToPlay == PC_WHITE ? P_WROOK : P_BROOK;
            else if (move.isQueenPromotion())
                piece = sideToPlay == PC_WHITE ? P_WQUEEN : P_BQUEEN;
        } else {
            newState.halfmoveClock += 1;
            piece = move.getFromPiece();

            if (move.getFromPieceType() == PT_KING) {
                newState.removeKingsideCastlingRights(sideToPlay);
                newState.removeQueensideCastlingRights(sideToPlay);

                if (move.isKingSideCastle()) {
                    newState.halfmoveClock = 0;
                    board[move.getToSquare() - 1] =
                        sideToPlay == PC_WHITE ? P_WROOK : P_BROOK;
                    board[move.getToSquare() + 1] = P_EMPTY;
                }

                if (move.isQueenSideCastle()) {
                    newState.halfmoveClock = 0;
                    board[move.getToSquare() + 1] =
                        sideToPlay == PC_WHITE ? P_WROOK : P_BROOK;
                    board[move.getToSquare() - 2] = P_EMPTY;
                }
            }

            if (move.getFromPieceType() == PT_ROOK) {
                if (move.getFromSquare() == 7)
                    newState.removeKingsideCastlingRights(PC_WHITE);
                if (move.getFromSquare() == 0)
                    newState.removeQueensideCastlingRights(PC_WHITE);
                if (move.getFromSquare() == 63)
                    newState.removeKingsideCastlingRights(PC_BLACK);
                if (move.getFromSquare() == 56)
                    newState.removeQueensideCastlingRights(PC_BLACK);
            }
        }

        board[move.getToSquare()] = piece;
        board[move.getFromSquare()] = P_EMPTY;
        gameStateStack.push(newState);
        gameList.push_back(move);
    }

    void unmakeMove() {
        GameState state = gameStateStack.top();
        gameStateStack.pop();
        Move move = gameList.back();
        gameList.pop_back();

        if (move.isEnpassantCapture()) {
            int offset = sideToPlay == PC_WHITE ? -8 : 8;
            board[move.getToSquare() + offset] = state.epCapturedPiece;
        }
        if (move.isKingSideCastle()) {
            board[move.getToSquare() - 1] = P_EMPTY;
            board[move.getToSquare() + 1] =
                sideToPlay == PC_WHITE ? P_WROOK : P_BROOK;
        }
        if (move.isQueenSideCastle()) {
            board[move.getToSquare() + 1] = P_EMPTY;
            board[move.getToSquare() - 2] =
                sideToPlay == PC_WHITE ? P_WROOK : P_BROOK;
        }

        board[move.getFromSquare()] = move.getFromPiece();
        board[move.getToSquare()] = move.getToPiece();
    }

    // Possible optimization: work with linked list to efficiently remove
    // illegal moves
    unordered_map<string, Move> generateMoves() {
        unordered_map<string, Move> pseudo = generatePseudoLegalMoves();
        unordered_map<string, Move> legal;

        for (auto& [notation, move] : pseudo) {
            makeMove(move);
            if (!isInCheck()) legal.emplace(notation, move);
            unmakeMove();
        }

        return legal;
    }

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

    Move parseMove(const string& move,
                   const unordered_map<string, Move>& legalMoves) const {
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

        if (getPieceColor(board[from]) != sideToPlay)
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

#include <iostream>
#include <vector>

#include "board/board.h"
#include "utils/piece_utils.h"

std::vector<int> generatePawnMoves(const Board& board, int piecePosition) {
    std::vector<int> moves;
    bool isWhite = board.board[piecePosition] > 0;

    // White moves up the board; black moves down
    int direction = isWhite ? 8 : -8;

    // Forward move
    // We assume that pawn is not on the first or last rank, since it wouldn't be a pawn anymore
    if (board.board[piecePosition + direction] == EMPTY) {
        moves.push_back(piecePosition + direction);
    }

    // Left capture
    int leftCaptureDirection = piecePosition + direction - 1;
    if (piecePosition % 8 != 0 && isOpponentPiece(leftCaptureDirection, isWhite)) {
        moves.push_back(leftCaptureDirection);
    }

    // Right capture
    int rightCaptureDirection = piecePosition + direction + 1;
    if (piecePosition % 8 != 7 && isOpponentPiece(rightCaptureDirection, isWhite)) {
        moves.push_back(rightCaptureDirection);
    }

    // Handle moving forward two tiles if pawn hasn't moved
    bool isStartingRow = (isWhite && piecePosition >= 8 && piecePosition <= 15) ||
                         (!isWhite && piecePosition >= 47 && piecePosition <= 55);
    if (isStartingRow) {
        bool pathClear = board.board[piecePosition + direction] == EMPTY &&
                         board.board[piecePosition + 2 * direction] == EMPTY;
        if (pathClear) {
            moves.push_back(piecePosition + 2 * direction);
        }
    }

    // TODO: Handle en-pessant
    // TODO: Handle promotions

    return moves;
}

std::vector<int> generateRookMoves(const Board& board, int piecePosition) {
    std::vector<int> moves;
    bool isWhite = board.board[piecePosition] > 0;

    // Left
    int rowStart = (piecePosition / 8) * 8;
    for (int position = piecePosition - 1; position >= rowStart; --position) {
        if (board.board[position] != EMPTY) {
            if (isOpponentPiece(position, isWhite)) moves.push_back(position);
            break;
        }
        moves.push_back(position);
    }

    // Right
    int rowEnd = rowStart + 8;
    for (int position = piecePosition + 1; position < rowEnd; ++position) {
        if (board.board[position] != EMPTY) {
            if (isOpponentPiece(position, isWhite)) moves.push_back(position);
            break;
        }
        moves.push_back(position);
    }

    // Up
    for (int position = piecePosition - 8; position >= 0; position -= 8) {
        if (board.board[position] != EMPTY) {
            if (isOpponentPiece(position, isWhite)) moves.push_back(position);
            break;
        }
        moves.push_back(position);
    }

    // Down
    for (int position = piecePosition + 8; position < 64; position += 8) {
        if (board.board[position] != EMPTY) {
            if (isOpponentPiece(position, isWhite)) moves.push_back(position);
            break;
        }
        moves.push_back(position);
    }

    return moves;
}

std::vector<int> generateBishopMoves(const Board& board, int piecePosition) {
    std::vector<int> moves;
    bool isWhite = board.board[piecePosition] > 0;

    // Up left diagonal
    for (int position = piecePosition - 9; position >= 0; position -= 9) {
        if (board.board[position] != EMPTY) {
            if (isOpponentPiece(position, isWhite)) moves.push_back(position);
            break;
        }
        moves.push_back(position);
    }

    // Up right diagonal
    for (int position = piecePosition - 7; position >= 0; position -= 7) {
        if (board.board[position] != EMPTY) {
            if (isOpponentPiece(position, isWhite)) moves.push_back(position);
            break;
        }
        moves.push_back(position);
    }

    // Down left diagonal
    for (int position = piecePosition + 7; position < 64; ++position) {
        if (board.board[position] != EMPTY) {
            if (isOpponentPiece(position, isWhite)) moves.push_back(position);
            break;
        }
        moves.push_back(position);
    }

    // Down right diagonal
    for (int position = piecePosition + 9; position < 64; ++position) {
        if (board.board[position] != EMPTY) {
            if (isOpponentPiece(position, isWhite)) moves.push_back(position);
            break;
        }
        moves.push_back(position);
    }

    return moves;
}

std::vector<int> generateKnightMoves(const Board& board, int piecePosition) {
    std::vector<int> moves;
    bool isWhite = board.board[piecePosition] > 0;

    const int POSSIBLE_MOVES[] = {
        piecePosition - 17, piecePosition - 15,
        piecePosition - 10, piecePosition + 6,
        piecePosition - 6, piecePosition + 10,
        piecePosition + 15, piecePosition + 17
    };
    
    for (const int& move : POSSIBLE_MOVES) {
        if (move >= 0 && move < 64 &&
            (board.board[move] == EMPTY || isOpponentPiece(board.board[move], isWhite))) {
            moves.push_back(move);
        }
    }

    return moves;
}

std::vector<int> generateQueenMoves(const Board& board, int piecePosition) {
    std::vector<int> moves;
    bool isWhite = board.board[piecePosition] > 0;
    
    std::vector<int> rookMoves = generateRookMoves(board, piecePosition);
    moves.insert(moves.end(), rookMoves.begin(), rookMoves.end());

    std::vector<int> bishopMoves = generateBishopMoves(board, piecePosition);
    moves.insert(moves.end(), bishopMoves.begin(), bishopMoves.end());

    return moves;
}

std::vector<int> generateKingMoves(const Board& board, int piecePosition) {
    std::vector<int> moves;
    bool isWhite = board.board[piecePosition] > 0;

    const int POSSIBLE_MOVES[] = {
        piecePosition - 9, piecePosition - 8, piecePosition - 7,
        piecePosition - 1, piecePosition + 1,
        piecePosition + 7, piecePosition + 8, piecePosition + 9
    };

    for (const int& move : POSSIBLE_MOVES) {
        if (move >= 0 && move < 64 &&
            (board.board[move] == EMPTY || isOpponentPiece(board.board[move], isWhite))) {
            moves.push_back(move);
        }
    }

    // TODO: handle castling
    // TODO: handle illegal moves (that put king in check)

    return moves;
}
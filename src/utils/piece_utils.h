#ifndef PIECE_UTILS_H
#define PIECE_UTILS_H

inline bool isOpponentPiece(int piece, bool isWhite) {
    return isWhite ? piece < 0 : piece > 0;
}

inline bool isOwnPiece(int piece, bool isWhite) {
    return !isOpponentPiece(piece, isWhite);
}

#endif
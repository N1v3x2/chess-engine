#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Piece.h"
#include <string>
#include <unordered_map>

namespace graphics {

constexpr const char* CLEAR = "\x1b[2J";
constexpr const char* HOME = "\x1b[H";
constexpr const char* HIDE = "\x1b[?25l";
constexpr const char* SHOW = "\x1b[?25h";
constexpr const char* RESET = "\x1b[0m";
constexpr const char* RESET_FOREGROUND = "\x1b[39m";
constexpr const char* RESET_BACKGROUND = "\x1b[49m";
constexpr const char* INVERT = "\x1b[7m";
constexpr const char* LIGHT_SQUARE = "\033[48;2;222;184;135m";
constexpr const char* LIGHT_TEXT = "\x1b[38;5;255m";
constexpr const char* DARK_SQUARE = "\033[48;2;190;120;75m";
constexpr const char* DARK_TEXT = "\x1b[38;5;0m";

const std::unordered_map<PieceType, std::string> glyph = {
    {PT_KING, "♚"},   {PT_QUEEN, "♛"}, {PT_ROOK, "♜"}, {PT_BISHOP, "♝"},
    {PT_KNIGHT, "♞"}, {PT_PAWN, "♟"},  {PT_EMPTY, " "}};

} // namespace graphics

#endif // !GRAPHICS_H

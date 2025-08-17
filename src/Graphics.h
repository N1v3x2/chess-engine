#include "Piece.h"

#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

namespace term {
constexpr const char* CLEAR = "\x1b[2J";
constexpr const char* HOME = "\x1b[H";
constexpr const char* HIDE = "\x1b[?25l";
constexpr const char* SHOW = "\x1b[?25h";
constexpr const char* RESET = "\x1b[0m";
constexpr const char* INVERT = "\x1b[7m";
} // namespace term

const unordered_map<Piece, string> glyph = {
    {P_WKING, "♔"},   {P_WQUEEN, "♕"},  {P_WROOK, "♖"},   {P_WBISHOP, "♗"},
    {P_WKNIGHT, "♘"}, {P_WPAWN, "♙"},   {P_BKING, "♚"},   {P_BQUEEN, "♛"},
    {P_BROOK, "♜"},   {P_BBISHOP, "♝"}, {P_BKNIGHT, "♞"}, {P_BPAWN, "♟"},
    {P_EMPTY, " "}};

#ifndef MOVE_PARSER_H
#define MOVE_PARSER_H

#include "Move.h"
#include <string>

using std::string;

using ui = unsigned int;

ui convertStringToSquare(string square) {
    return 8 * (square[1] - '1') + (square[0] - 'a');
}

#endif // !MOVE_PARSER_H

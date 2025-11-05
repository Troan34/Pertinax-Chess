#pragma once
#include "ChessThings/Board.h"

class MoveOrdering 
{
private:
	GuessStruct* MovesArray;

	std::array<std::array<Move, 2>, 50> KillerMoves;
};
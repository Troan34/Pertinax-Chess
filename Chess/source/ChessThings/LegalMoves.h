#pragma once
#include <array>
#include <cmath>
#include <vector>
#include "Board.h"


struct MOVE
{
	unsigned int PieceType;
	std::vector<unsigned int> TargetSquares;
};


class GenerateLegalMoves
{
private:
	const int OffsetForDirections[8] = {8, -8, -1, 1, 7, -7, 9, -9};
	std::array<std::array<unsigned int, 8>, 64>NumOfSquaresUntilEdge;
	std::array<MOVE, 64> moves;
	std::array<unsigned int, 64> m_BoardSquare;

public:
	GenerateLegalMoves(std::array<unsigned int, 64> BoardSquare);
	void GenerateMoves(bool isNextMoveForWhite);
	void SliderMoveGen(unsigned int PieceType, unsigned int BoardSquarePos, bool isNextMoveForWhite);
};
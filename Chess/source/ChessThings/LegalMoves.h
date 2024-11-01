#pragma once
#include <array>
#include <cmath>
#include <vector>
#include "Board.h"


struct MOVE
{
	unsigned int PieceType;//the piecetype on the target square
	std::vector<unsigned int> TargetSquares;
};


class GenerateLegalMoves
{
private:
	const int OffsetForDirections[8] = {8, -8, -1, 1, 7, -7, 9, -9};
	std::array<std::array<unsigned int, 8>, 64>NumOfSquaresUntilEdge;
	std::array<MOVE, 64> moves;
	std::array<unsigned int, 64> m_BoardSquare;
	const int OffsetForKnight[8] = {17, 10, -6, 10, -17, 6, 15};//these aren't in a specific order(for my future-self debugging)

public:
	GenerateLegalMoves(std::array<unsigned int, 64> BoardSquare);
	void GenerateMoves(bool isNextMoveForWhite);
	void SliderMoveGen(int BoardSquarePos, bool isNextMoveForWhite);
	void KnightMoveGen(int BoardSquarePos, bool isNextMoveForWhite);
	void PawnMoveGen(int BoardSquarePos, bool isNextMoveForWhite);
};
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

struct canCastle
{
	bool HasWhiteLongRookMoved, HasWhiteShortRookMoved, HasBlackLongRookMoved, HasBlackShortRookMoved, HasWhiteKingMoved, HasBlackKingMoved;
};

class GenerateLegalMoves
{
private:
	const int OffsetForDirections[8] = {8, -8, -1, 1, 7, -7, 9, -9};
	std::array<std::array<unsigned int, 8>, 64>NumOfSquaresUntilEdge;
	std::array<MOVE, 64> moves;
	std::array<unsigned int, 64> m_BoardSquare;
	canCastle CanCastle;

	const int OffsetForKnight[8] = {17, 10, -6, 10, -17, 6, 15};//these aren't in a specific order(for my future-self debugging)
	const int OffsetForWhitePawn[3] = {7, 8, 9};
	const int OffsetForBlackPawn[3] = {-7, -8, -9};
public:
	GenerateLegalMoves(std::array<unsigned int, 64Ui64> BoardSquare, std::array<unsigned int, 64> previousBoardSquare, canCastle CanCastle, bool isNextMoveForWhite);
	GenerateLegalMoves(std::array<unsigned int, 64Ui64> BoardSquare, canCastle CanCastle, bool isNextMoveForWhite);
	void GenerateMoves(bool isNextMoveForWhite);
	void SliderMoveGen(int BoardSquarePos, bool isNextMoveForWhite);
	void KnightMoveGen(int BoardSquarePos, bool isNextMoveForWhite);
	void PawnMoveGen(int BoardSquarePos, bool isNextMoveForWhite);
	void KingMoveGen(int BoardSquarePos, bool isNextMoveForWhite);
};
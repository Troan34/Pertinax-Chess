#pragma once
#include <array>
#include <cmath>
#include <vector>
#include "Board.h"
#include <iostream>

struct MOVE
{
	unsigned int PieceType;
	std::vector<unsigned int> TargetSquares;
	std::vector<unsigned int> PinnedTargetSquares;
	std::vector<unsigned int> AttackableSquares;
	unsigned int PieceTypeUnderAttack;
	unsigned int Castle;
};

struct canCastle
{
	bool HasWhiteLongRookMoved = false, HasWhiteShortRookMoved = false, HasBlackLongRookMoved = false, HasBlackShortRookMoved = false, HasWhiteKingMoved = false, HasBlackKingMoved = false;
};

class GenerateLegalMoves
{
private:
	const int OffsetForDirections[8] = {8, -8, -1, 1, 7, -7, 9, -9};
	std::array<std::array<unsigned int, 8>, 64>NumOfSquaresUntilEdge;
	std::array<unsigned int, 64> m_BoardSquare;
	canCastle CanCastle;
	std::vector<int> OffsetsForKnight;
	bool isItCheckmate = true;

	const int OffsetForWhitePawn[3] = {7, 8, 9};
	const int OffsetForBlackPawn[3] = {-7, -8, -9};
public:
	std::array<MOVE, 64> moves;//array of Moves, every pseudolegal move
	std::array<MOVE, 64> LegalMoves;
	std::array<bool, 64> AttackedSquares;
	std::array<bool, 64> PinnedSquaresWithTheKingBeingPinned;

	/**
	* array with every abs pinned square(behind the piece pinned until the king)
	* 
	* The array is structured like a Board in which squares that are pinned (behind pinned piece)
	* receive the value of the pinning piece BoardSquarePos
	*/
	std::array<unsigned int, 64> WhichBoardSquaresAreAbsPinned;
	std::array<unsigned int, 64> CheckTargetSquares;
	unsigned int MoveNum;
	GenerateLegalMoves(std::array<unsigned int, 64Ui64> BoardSquare, std::array<unsigned int, 64> previousBoardSquare, canCastle CanCastle, bool isNextMoveForWhite, unsigned int MoveNum);
	GenerateLegalMoves(std::array<unsigned int, 64Ui64> BoardSquare, canCastle CanCastle, bool isNextMoveForWhite, unsigned int MoveNum);
	GenerateLegalMoves(std::array<unsigned int, 64Ui64> BoardSquare, canCastle CanCastle, bool isNextMoveForWhite, unsigned int MoveNum, bool isForOppositeMoves);
	void GenerateMoves(bool isNextMoveForWhite);
	void SliderMoveGen(int BoardSquarePos, bool isNextMoveForWhite);
	void KnightMoveGen(int BoardSquarePos, bool isNextMoveForWhite);
	void CreateOffesetsForKnight(int BoardSquarePos);
	void PawnMoveGen(int BoardSquarePos, bool isNextMoveForWhite);
	void KingMoveGen(int BoardSquarePos, bool isNextMoveForWhite);
	void RemoveIllegalMoves_Checks_AbsolutePins(bool isNextMoveForWhite);
	bool isA64ArrayEmpty(std::array<MOVE, 64> Array);

};
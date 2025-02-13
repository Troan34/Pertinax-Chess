#pragma once
#include <array>
#include <cmath>
#include <vector>
#include "Board.h"
#include <iostream>

struct MOVE
{
	uint8_t PieceType;
	std::vector<uint8_t> TargetSquares;
	std::array<uint8_t, 3> Promotion{65, 65, 65};//which move lets pawn promote
};

class GenerateLegalMoves
{
private:
	const int OffsetForDirections[8] = { 8, -8, -1, 1, 7, -7, 9, -9 };
	std::array<std::array<uint8_t, 8>, 64>NumOfSquaresUntilEdge;
	std::array<uint8_t, 64> m_BoardSquare;
	canCastle CanCastle;
	std::vector<uint8_t> OffsetsForKnight;
	bool isItCheckmate = true;

	const uint8_t WhitePawnDirections[3] = { 4,0,6 };
	const uint8_t BlackPawnDirections[3] = { 7,1,5 };

	const int OffsetForWhitePawn[3] = { 7, 8, 9 };
	const int OffsetForBlackPawn[3] = { -7, -8, -9 };


	void CanKingCastle_LMoves(const GenerateLegalMoves& OppositeMoves, bool& isItCheckmate, std::vector<uint8_t>::iterator& it, const uint8_t& BoardSquareOfKingToMove, const uint8_t& KingMove);
public:
	std::array<MOVE, 64> moves;//array of Moves, every legal move
	std::array<bool, 64> AttackedSquares;
	std::array<bool, 64> PinnedSquaresWithTheKingBeingPinned;

	/**
	* array with every abs pinned square(infront the piece pinned until the attacking piece + actual piece pinned + behind until the (included)king)
	*
	* The array is structured like a Board in which squares that are pinned
	* receive the value of the pinning piece BoardSquarePos
	*/
	std::array<uint8_t, 64> WhichBoardSquaresAreAbsPinned;

	/*
	* BoardSquare where the square's value is the checking piece BoardSquarePos
	* The king's BoardSquarePos is included
	*/
	std::array<uint8_t, 64> CheckTargetSquares;
	unsigned int MoveNum;
	GenerateLegalMoves(const std::array<uint8_t, 64Ui64>& BoardSquare, const std::array<uint8_t, 64>* previousBoardSquare, canCastle CanCastle, bool isNextMoveForWhite, unsigned int MoveNum, bool isForOppositeMoves);
	~GenerateLegalMoves();
	void GenerateMoves(bool isNextMoveForWhite);
	void SliderMoveGen(const uint8_t& BoardSquarePos, bool isNextMoveForWhite);
	void KnightMoveGen(const uint8_t& BoardSquarePos, bool isNextMoveForWhite);
	void CreateOffesetsForKnight(const uint8_t& BoardSquarePos);
	void PawnMoveGen(const uint8_t& BoardSquarePos, bool isNextMoveForWhite);
	void KingMoveGen(const uint8_t& BoardSquarePos, bool isNextMoveForWhite);
	void RemoveIllegalMoves(bool isNextMoveForWhite);

};
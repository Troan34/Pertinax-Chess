#pragma once
#include <array>
#include <cmath>
#include <vector>
#include "Board.h"
#include <iostream>
#include <unordered_set>

//compute NumOfSquaresUntilEdge at compile time
constexpr std::array<std::array<uint8_t, 8>, 64> fillNumOfSquaresUntilEdge()
{
	std::array<std::array<uint8_t, 8>, 64> l_NumOfSquaresUntilEdge{};
	for (uint8_t file = 0; file < 8; file++)
	{
		for (uint8_t rank = 0; rank < 8; rank++)
		{
			uint8_t numNorth = 7 - rank;
			uint8_t numSouth = rank;
			uint8_t numWest = file;
			uint8_t numEast = 7 - file;

			uint8_t squareIndex = rank * 8 + file;

			l_NumOfSquaresUntilEdge[squareIndex][0] = numNorth;
			l_NumOfSquaresUntilEdge[squareIndex][1] = numSouth;
			l_NumOfSquaresUntilEdge[squareIndex][2] = numWest;
			l_NumOfSquaresUntilEdge[squareIndex][3] = numEast;
			l_NumOfSquaresUntilEdge[squareIndex][4] = std::min(numNorth, numWest);
			l_NumOfSquaresUntilEdge[squareIndex][5] = std::min(numSouth, numEast);
			l_NumOfSquaresUntilEdge[squareIndex][6] = std::min(numNorth, numEast);
			l_NumOfSquaresUntilEdge[squareIndex][7] = std::min(numSouth, numWest);

		}
	}

	return l_NumOfSquaresUntilEdge;
}



struct MOVE
{
	uint8_t PieceType;
	std::vector<uint8_t> TargetSquares;
	std::array<uint8_t, 3> Promotion{65, 65, 65};//which move lets PAWN promote
};

class GenerateLegalMoves
{
private:
	const int OffsetForDirections[8] = { 8, -8, -1, 1, 7, -7, 9, -9 };
	static constexpr std::array<std::array<uint8_t, 8>, 64>NumOfSquaresUntilEdge = fillNumOfSquaresUntilEdge();

	std::array<uint8_t, 64> m_BoardSquare;
	canCastle CanCastle;

	bool isItCheckmate = true;
	uint8_t DoubleCheckBoardSquare = 65; //if two checks collide the last one that wants to modify the value will write to this
	bool isNextMoveForWhite;

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

	uint32_t m_NumOfLegalMoves = 0;
	unsigned int MoveNum;
	std::array<bool, 8> EnPassantFiles;

	GenerateLegalMoves(const std::array<uint8_t, 64Ui64>& BoardSquare, const std::array<uint8_t, 64>* previousBoardSquare, canCastle CanCastle, bool isNextMoveForWhite, unsigned int MoveNum, bool isForOppositeMoves);
	~GenerateLegalMoves();
	void GenerateMoves();
	void SliderMoveGen(const uint8_t& BoardSquarePos);
	void KnightMoveGen(const uint8_t& BoardSquarePos);
	std::array<uint8_t, 8>& CreateOffesetsForKnight(const uint8_t& BoardSquarePos);
	void PawnMoveGen(const uint8_t& BoardSquarePos);
	void KingMoveGen(const uint8_t& BoardSquarePos);
	void RemoveIllegalMoves();
	static void SetDoNotEnPassant(bool SetToThis);

};
#pragma once
#include <array>
#include <cmath>
#include <vector>
#include "Board.h"
#include <iostream>
#include <unordered_set>
constexpr unsigned int WHITE_PAWN = WHITE + PAWN;     //17
constexpr unsigned int WHITE_BISHOP = WHITE + BISHOP; //18
constexpr unsigned int WHITE_KNIGHT = WHITE + KNIGHT; //19
constexpr unsigned int WHITE_ROOK = WHITE + ROOK;     //20
constexpr unsigned int WHITE_QUEEN = WHITE + QUEEN;   //21
constexpr unsigned int WHITE_KING = WHITE + KING;     //22
constexpr unsigned int BLACK_PAWN = BLACK + PAWN;     //9
constexpr unsigned int BLACK_BISHOP = BLACK + BISHOP; //10
constexpr unsigned int BLACK_KNIGHT = BLACK + KNIGHT; //11
constexpr unsigned int BLACK_ROOK = BLACK + ROOK;     //12
constexpr unsigned int BLACK_QUEEN = BLACK + QUEEN;   //13
constexpr unsigned int BLACK_KING = BLACK + KING;     //14


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
	std::array<std::array<uint8_t, 8>, 64>NumOfSquaresUntilEdge;
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
	unsigned int MoveNum;
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
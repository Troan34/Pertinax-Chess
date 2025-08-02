#pragma once
#include <array>
#include <cmath>
#include <vector>
#include "Board.h"
#include <iostream>
#include <unordered_set>

constexpr int OffsetForDirections[8] = { 8, -8, -1, 1, 7, -7, 9, -9 };

//precompute NumOfSquaresUntilEdge
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
			l_NumOfSquaresUntilEdge[squareIndex][4] = min(numNorth, numWest);
			l_NumOfSquaresUntilEdge[squareIndex][5] = min(numSouth, numEast);
			l_NumOfSquaresUntilEdge[squareIndex][6] = min(numNorth, numEast);
			l_NumOfSquaresUntilEdge[squareIndex][7] = min(numSouth, numWest);

		}
	}

	return l_NumOfSquaresUntilEdge;
}

static constexpr std::array<std::array<uint8_t, 8>, 64>NumOfSquaresUntilEdge = fillNumOfSquaresUntilEdge();

//precompute Rook and Bishop masks
constexpr std::array<uint64_t, 64> ComputeRookMasks()
{
	std::array<uint64_t, 64> RookMasks{ 0 };
	for (uint8_t Index = 0; Index <= MAX_SQUARE; Index++)
	{
		for (uint8_t Direction = 0; Direction <= 3; Direction++)
		{
			if (NumOfSquaresUntilEdge[Index][Direction] <= 1) { continue; }
			for (uint8_t Scalar = 1; Scalar < NumOfSquaresUntilEdge[Index][Direction]; Scalar++)
			{
				RookMasks[Index] |= (1ULL << (Index + (OffsetForDirections[Direction] * Scalar)));
			}
		}
	}
	return RookMasks;
}

constexpr std::array<uint64_t, 64> ComputeBishopMasks()
{
	std::array<uint64_t, 64> BishopMasks{ 0 };
	for (uint8_t Index = 0; Index <= MAX_SQUARE; Index++)
	{
		for (uint8_t Direction = 4; Direction <= 7; Direction++)
		{
			if (NumOfSquaresUntilEdge[Index][Direction] <= 1) { continue; }
			for (uint8_t Scalar = 1; Scalar < NumOfSquaresUntilEdge[Index][Direction]; Scalar++)
			{
				BishopMasks[Index] |= (1ULL << (Index + (OffsetForDirections[Direction] * Scalar)));
			}
		}
	}
	return BishopMasks;
}

constexpr std::array<uint64_t,64> ROOK_MAGICS = {
0x0080001020400080ULL, 0x0040001000200040ULL, 0x0020080080002000ULL, 0x0010040080040000ULL,
0x0008020080080000ULL, 0x0004010080100000ULL, 0x0002008008000400ULL, 0x0001004080002000ULL,
0x0000800080004000ULL, 0x0000400080002000ULL, 0x0000200080001000ULL, 0x0000100080000800ULL,
0x0000080080000400ULL, 0x0000040080000200ULL, 0x0000020080000100ULL, 0x0000010080800400ULL,
0x0000008000800200ULL, 0x0000004080001000ULL, 0x0000002004000800ULL, 0x0000001002000400ULL,
0x0000000801000200ULL, 0x0000000400800100ULL, 0x0000000200400080ULL, 0x0000000100200040ULL,
0x0000000080100020ULL, 0x0000000040080010ULL, 0x0000000020040008ULL, 0x0000000010020004ULL,
0x0000000008010002ULL, 0x0000000004008001ULL, 0x0000000002004080ULL, 0x0000000001002040ULL,
0x0000000000801020ULL, 0x0000000000400810ULL, 0x0000000000200408ULL, 0x0000000000100204ULL,
0x0000000000080102ULL, 0x0000000000040081ULL, 0x0000000000020040ULL, 0x0000000000010020ULL,
0x0000000000008010ULL, 0x0000000000004008ULL, 0x0000000000002004ULL, 0x0000000000001002ULL,
0x0000000000000801ULL, 0x0000000000000400ULL, 0x0000000000000200ULL, 0x0000000000000100ULL,
0x0000000000000080ULL, 0x0000000000000040ULL, 0x0000000000000020ULL, 0x0000000000000010ULL,
0x0000000000000008ULL, 0x0000000000000004ULL, 0x0000000000000002ULL, 0x0000000000000001ULL,
0x0000000000008000ULL, 0x0000000000004000ULL, 0x0000000000002000ULL, 0x0000000000001000ULL,
0x0000000000000800ULL, 0x0000000000000400ULL, 0x0000000000000200ULL, 0x0000000000000100ULL
};

constexpr std::array<uint64_t, 64> BISHOP_MAGICS = {
0x0002020202020200ULL, 0x0002020202020000ULL, 0x0004010202000000ULL, 0x0004040080000000ULL,
0x0001104000000000ULL, 0x0000821040000000ULL, 0x0000410410400000ULL, 0x0000104104104000ULL,
0x0000040404040400ULL, 0x0000020202020200ULL, 0x0000040102020000ULL, 0x0000040400800000ULL,
0x0000011040000000ULL, 0x0000008210400000ULL, 0x0000004104104000ULL, 0x0000002082082000ULL,
0x0004000808080800ULL, 0x0002000404040400ULL, 0x0001000202020200ULL, 0x0000800802004000ULL,
0x0000400400802000ULL, 0x0000200201001000ULL, 0x0000400082080800ULL, 0x0000200041040400ULL,
0x0002080010101000ULL, 0x0001040008080800ULL, 0x0000208004010400ULL, 0x0000104000802080ULL,
0x0000082000401040ULL, 0x0000041000202080ULL, 0x0000040800102000ULL, 0x0000020400081000ULL,
0x0000020200040800ULL, 0x0000010100020400ULL, 0x0000008080010200ULL, 0x0000004040008000ULL,
0x0000004040004000ULL, 0x0000008080002000ULL, 0x0000008101000400ULL, 0x0000008102000200ULL,
0x0000010104000100ULL, 0x0000010080800080ULL, 0x0000020080400040ULL, 0x0000020040200020ULL,
0x0000040020100010ULL, 0x0000040008080008ULL, 0x0000080004040004ULL, 0x0000080002020002ULL,
0x0000100001010001ULL, 0x0000200000808000ULL, 0x0000400000404000ULL, 0x0000800000202000ULL,
0x0001000000101000ULL, 0x0002000000080800ULL, 0x0004000000040400ULL, 0x0008000000020200ULL,
0x0010000000010100ULL, 0x0020000000008080ULL, 0x0040000000004040ULL, 0x0080000000002020ULL,
0x0100000000001010ULL, 0x0200000000000808ULL, 0x0400000000000404ULL, 0x0800000000000202ULL
};

constexpr std::array<uint64_t, 64> ROOK_MASKS = ComputeRookMasks();

constexpr std::array<uint64_t, 64> BISHOP_MASKS = ComputeBishopMasks();

constexpr std::array<std::array<uint64_t, 4096>, 64> ComputeRookAttacks()
{
	std::array<std::array<uint64_t, 4096>, 64> RookAttacks{ 0 };
	for (uint8_t Index = 0; Index <= MAX_SQUARE; Index++)
	{

	}
}

constexpr std::array<std::array<uint64_t, 4096>, 64> ROOK_ATTACKS = ComputeRookAttacks();

constexpr std::array<std::array<uint64_t, 512>, 64> BISHOP_ATTACKS;



struct MOVE
{
	std::vector<uint8_t> TargetSquares;//this has to be changed to something more bitboard-ish
	std::array<uint8_t, 3> Promotion{65, 65, 65};//which move lets PAWN promote
	uint8_t PieceType;
};

struct MOVE_BIT
{
	bit::BitBoard64 TargetSquares;
	std::array<uint8_t, 3> Promotion{ 65, 65, 65 };
	uint8_t PieceType;
};

class GenerateLegalMoves
{
private:

	bit::BitPosition m_BoardSquare;
	

	canCastle CanCastle;

	bool isItCheckmate = true;
	uint8_t DoubleCheckBoardSquare = 65; //if two checks collide the last one that wants to modify the value will write to this
	bool isNextMoveForWhite;

	static constexpr uint8_t WhitePawnDirections[3] = { 4,0,6 };
	static constexpr uint8_t BlackPawnDirections[3] = { 7,1,5 };

	static constexpr int OffsetForWhitePawn[3] = { 7, 8, 9 };
	static constexpr int OffsetForBlackPawn[3] = { -7, -8, -9 };

	void CanKingCastle_LMoves(const GenerateLegalMoves& OppositeMoves, bool& isItCheckmate, const uint8_t& BoardSquareOfKingToMove, const uint8_t KingMove);
public:
	std::array<MOVE_BIT, 64> moves;//array of Moves, every legal move
	bit::BitBoard64 AttackedSquares;
	bit::BitBoard64 PinnedSquaresWithTheKingBeingPinned;

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
	std::array<bool, 8> EnPassantFiles{false};

	GenerateLegalMoves(const bit::BitPosition& BoardSquare, const bit::BitPosition* previousBoardSquare, canCastle CanCastle, bool isNextMoveForWhite, unsigned int MoveNum, bool isForOppositeMoves);
	~GenerateLegalMoves();
	void GenerateMoves();
	void SliderMoveGen(const uint8_t BoardSquarePos);
	void KnightMoveGen(const uint8_t BoardSquarePos);
	std::array<uint8_t, 8>& CreateOffesetsForKnight(const uint8_t BoardSquarePos);
	void PawnMoveGen(const uint8_t BoardSquarePos);
	void KingMoveGen(const uint8_t BoardSquarePos);
	void RemoveIllegalMoves();
	static void SetDoNotEnPassant(bool SetToThis);
	bool IsMoveLegal(const Move& CheckedMove) const;

};
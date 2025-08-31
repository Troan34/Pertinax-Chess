#pragma once
#include <array>
#include <cmath>
#include <vector>
#include "Board.h"
#include <iostream>
#include <unordered_set>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

constexpr int OffsetForDirections[8] = { 8, -8, -1, 1, 7, -7, 9, -9 };
constexpr enum DIRECTIONS
{
	N,S,W,E,NW,SE,NE,SW
};

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
constexpr std::array<uint64_t, 64> ComputeRookMasks(bool IncludeBorder)
{
	std::array<uint64_t, 64> RookMasks{ 0 };
	for (uint8_t Index = 0; Index <= MAX_SQUARE; Index++)
	{
		for (uint8_t Direction = 0; Direction <= 3; Direction++)
		{
			if (IncludeBorder)
			{
				if (NumOfSquaresUntilEdge[Index][Direction] < 1) { continue; }
				for (uint8_t Scalar = 1; Scalar <= NumOfSquaresUntilEdge[Index][Direction]; Scalar++)
				{
					RookMasks[Index] |= (1ULL << (Index + (OffsetForDirections[Direction] * Scalar)));
				}
			}
			else
			{
				if (NumOfSquaresUntilEdge[Index][Direction] <= 1) { continue; }
				for (uint8_t Scalar = 1; Scalar < NumOfSquaresUntilEdge[Index][Direction]; Scalar++)
				{
					RookMasks[Index] |= (1ULL << (Index + (OffsetForDirections[Direction] * Scalar)));
				}
			}
		}
	}
	return RookMasks;
}

constexpr std::array<uint64_t, 64> ComputeBishopMasks(bool IncludeBorder)
{
	std::array<uint64_t, 64> BishopMasks{ 0 };
	for (uint8_t Index = 0; Index <= MAX_SQUARE; Index++)
	{
		for (uint8_t Direction = 4; Direction <= 7; Direction++)
		{
			if (IncludeBorder)
			{
				if (NumOfSquaresUntilEdge[Index][Direction] < 1) { continue; }
				for (uint8_t Scalar = 1; Scalar <= NumOfSquaresUntilEdge[Index][Direction]; Scalar++)
				{
					BishopMasks[Index] |= (1ULL << (Index + (OffsetForDirections[Direction] * Scalar)));
				}
			}
			else
			{
				if (NumOfSquaresUntilEdge[Index][Direction] <= 1) { continue; }
				for (uint8_t Scalar = 1; Scalar < NumOfSquaresUntilEdge[Index][Direction]; Scalar++)
				{
					BishopMasks[Index] |= (1ULL << (Index + (OffsetForDirections[Direction] * Scalar)));
				}
			}
		}
	}
	return BishopMasks;
}

static std::array<uint64_t, 64> ROOK_MAGICS;
static std::array<uint64_t, 64> BISHOP_MAGICS;
static constexpr std::array<uint64_t, 64> ROOK_MASKS = ComputeRookMasks(false);
static constexpr std::array<uint64_t, 64> BISHOP_MASKS = ComputeBishopMasks(false);

static constexpr std::array<uint8_t, 64> ROOK_SHIFTS = {
  12, 11, 11, 11, 11, 11, 11, 12, 11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11, 12, 11, 11, 11, 11, 11, 11, 12
};

static constexpr std::array<uint8_t, 64> BISHOP_SHIFTS = {
  6, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 7, 7, 7, 7, 5, 5, 5, 5, 7, 9, 9, 7, 5, 5,
  5, 5, 7, 9, 9, 7, 5, 5, 5, 5, 7, 7, 7, 7, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 6
};

//these just include the last board square instead of ignoring it
static constexpr std::array<uint64_t, 64> ROOK_LEGAL_MASKS = ComputeRookMasks(true);
static constexpr std::array<uint64_t, 64> BISHOP_LEGAL_MASKS = ComputeBishopMasks(true);

/// <summary>Expands a compact bitboard representation into a full bitboard.</summary>
/// <param name="bits">Compact blockers (technically the limit is 2^12, the fun may be used beyond it's limits)</param>
/// <param name="mask">The mask on which the bits will be expanded</param>
/// <returns>A uint64_t bitboard with blockers expanded to the positions specified by the mask.</returns>
constexpr uint64_t expand_bits_to_mask(uint64_t bits, uint64_t mask) {
	uint64_t result = 0;
	int i = 0;
	while (mask) {
		uint8_t Square;
		bit::pop_lsb(mask, Square);
		if (bits & (1ULL << i++))
		{
			result |= (1ULL << Square);
		}
	}
	return result;
}

/// <summary>Computes every attack for the rook, MAKE SURE IT IS NEVER CALLED AT RUNTIME</summary>
uint64_t ComputeRookAttacks(uint8_t BoardSquare, uint16_t Blocker);

/// <summary>Computes every attack for the bishop, MAKE SURE IT IS NEVER CALLED AT RUNTIME</summary>
/// <returns>Value corresponding to BISHOP_ATTACKS</returns>
uint64_t ComputeBishopAttacks(uint8_t BoardSquare, uint16_t Blocker);

void MagicRookFinder(uint8_t BoardSquare);
void MagicBishopFinder(uint8_t BoardSquare);

uint16_t mult_rightShift(uint64_t BlockerBits, uint64_t Magic, uint8_t RelevantBitNum);

//plain magic bitboard
static std::array<std::array<uint64_t, 4096>, 64> ROOK_ATTACKS{};
//plain magic bitboard
static std::array<std::array<uint64_t, 512>, 64> BISHOP_ATTACKS{};

///<summary>Computes ATTACKS IF NOT found in disk, then saves it to disk</summary>
void ComputeHeavy();


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

	//Bitboard with squares under attack set
	bit::BitBoard64 AttackedSquares;

	/*
	Bit Board with the bits behind a king facing its checker
	Used to stop king from walking behind himself and end up in check anyways
	*/
	bit::BitBoard64 SquaresBehindCheckedKing;

	/**
	* array with every abs pinned square(whole xray attack until (included)king)
	*
	* The array is structured like a Board in which squares that are part of the pinning xray
	* receive the value of the pinning piece's BoardSquarePos
	*/
	std::array<uint8_t, 64> WhichBoardSquaresAreAbsPinned;

	/*
	* BoardSquare where the square's value is the checking piece's BoardSquarePos
	* The king's BoardSquarePos is included
	*/
	std::array<uint8_t, 64> CheckTargetSquares;

	uint32_t m_NumOfLegalMoves = 0;
	unsigned int MoveNum;
	std::array<bool, 8> EnPassantFiles{false};

	GenerateLegalMoves(const bit::BitPosition& BoardSquare, const bit::BitPosition* previousBoardSquare, canCastle CanCastle, bool isNextMoveForWhite, unsigned int MoveNum, bool isForOppositeMoves);
	GenerateLegalMoves(const std::array<uint8_t, 64>& BoardSquare, const std::array<uint8_t, 64>* previousBoardSquare, canCastle CanCastle, bool isNextMoveForWhite, unsigned int MoveNum, bool isForOppositeMoves);
	~GenerateLegalMoves();
	void GenerateMoves();
	void MagicSliderMoveGen(const uint8_t BoardSquarePos);
	void KnightMoveGen(const uint8_t BoardSquarePos);
	std::array<uint8_t, 8>& CreateOffesetsForKnight(const uint8_t BoardSquarePos);
	void PawnMoveGen(const uint8_t BoardSquarePos);
	void KingMoveGen(const uint8_t BoardSquarePos);
	void RemoveIllegalMoves();
	static void SetDoNotEnPassant(bool SetToThis);
	bool IsMoveLegal(const Move& CheckedMove) const;

};


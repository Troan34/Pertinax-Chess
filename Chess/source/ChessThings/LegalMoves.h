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

constexpr std::array<uint64_t, 64> ROOK_MAGICS = { 721209281088651681, 576461855063539968, 146371420312568705, 1099513724934, 146371390499454976, 1317350247415943172, 164420968817623040, 144836467705782416, 39655568311856, 5278518929960992800, 1152921504673955840, 288266943509602432, 9817918655925584000, 4523400525512704, 2199207804928, 36028938904932352, 8214600904897202193, 90117837036717188, 140806342066304, 144704534898409600, 2309924951232626688, 292736174819475466, 9335965330402492486, 4647716020186301440, 16305563934478565408, 1233986384872671264, 18159399143448, 4522291326091264, 181832835223814152, 57213259350528, 1162492891843528218, 1730094809165103232, 9232379313964187688, 9223372037190846464, 2598717722485325824, 9548211752701790208, 274946326552, 549764235264, 2115586, 9077980617769028, 27021614944125248, 216529230039614474, 2324141356722831360, 5192652573680633856, 97169365297664, 145529161373450368, 2364830163205627904, 4578435540189217, 3530894677773386755, 306244912377200640, 564049465049088, 9007336697888768, 1161964990700208128, 19808929448456, 54078397137027073, 2341872012698322694, 1152921504615319552, 90107177322151936, 16140905462609486340, 4620979649051230208, 54650130241946112, 756604759040854144, 1152930302847388674, 1270155832407359488 };
constexpr std::array<uint64_t, 64> BISHOP_MAGICS = {
	11551171746203699220, 576460752328589320, 9799832789177335810, 576602108267331844, 9223378633958622216, 144537617436789768, 4612262166817406979, 2305848523951704128, 4688563890775654528, 1134979472424980, 18577382873040512, 1189271636063428648, 4611699213137353010, 9295500017022280844, 864709820152910176, 153126923353031680, 6958061424824287236, 2251937252639008, 140737492549760, 144247164369895680, 599119488106106888, 11529215114792140816, 2308094809196200000, 4618599747545206824, 211127841587216, 9147936747327490, 288230431986356224, 16149908265965715972, 36077180093988912, 9044582650151178, 1152921646341816452, 2594671657734373376, 1198661192617411592, 211174985616448, 690493570695168, 324260281272238080, 4620693252058648576, 6917680762435142144, 657563066430783626, 2450522659172123680, 11531471243934957570, 274894721040, 565423854714880, 360289078366830596, 648518382848708624, 9011605909557248, 2308094809095047512, 79857400676480, 76561348284391432, 5188146771670339648, 144185557096863240, 9368648379002782080, 2882312696122834944, 268698625, 72207199214501888, 36510535680, 9259546519703593216, 1688867048521730, 2310346755473997824, 573945069707264, 7012109056652411904, 4611690966229717377, 9223513345573781505, 4647721498415529984 };
static constexpr std::array<uint64_t, 64> ROOK_MASKS = ComputeRookMasks(false);
static constexpr std::array<uint64_t, 64> BISHOP_MASKS = ComputeBishopMasks(false);

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
void ComputeRookAttacks(std::array<std::array<uint64_t, 4096>, 64>& RookAttacks);

/// <summary>Computes every attack for the bishop, MAKE SURE IT IS NEVER CALLED AT RUNTIME</summary>
/// <returns>Value corresponding to BISHOP_ATTACKS</returns>
void ComputeBishopAttacks(std::array<std::array<uint64_t, 512>, 64>& BishopAttacks);

uint64_t MagicFinder(uint8_t BoardSquare, bool IsRook);

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


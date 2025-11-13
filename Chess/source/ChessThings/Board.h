#pragma once
#include <string>
#include <unordered_map>
#include <array>
#include <Renderer.h>
#include <cmath>
#include <iostream>
#include <chrono>
#include <print>
#include <random> 
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <algorithm>
#include <type_traits>

#ifndef WHITE_TURN
#define WHITE_TURN(x) (!(x % 2))
#endif

enum SQUARES
{
	a1, b1, c1, d1, e1, f1, g1, h1,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a8, b8, c8, d8, e8, f8, g8, h8
};

enum BOARD_FILE
{
	FILE_A = 0x8080808080808080,
	FILE_B = 0x4040404040404040,
	FILE_C = 0x2020202020202020,
	FILE_D = 0x0101010101010101,
	FILE_E = 0x0808080808080808,
	FILE_F = 0x0404040404040404,
	FILE_G = 0x0202020202020202,
	FILE_H = 0x0101010101010101,
};

enum BOARD_RANK
{
	RANK_1 = 0x00000000000000FF,
	RANK_2 = 0x000000000000FF00,
	RANK_3 = 0x0000000000FF0000,
	RANK_4 = 0x00000000FF000000,
	RANK_5 = 0x000000FF00000000,
	RANK_6 = 0x0000FF0000000000,
	RANK_7 = 0x00FF000000000000,
	RANK_8 = 0xFF00000000000000,
};

enum CastlingIndices
{
	WhiteShort, WhiteLong, BlackShort, BlackLong
};

constexpr uint8_t MAX_SQUARE = 63;

constexpr uint64_t BORDER_MASK = FILE_A | FILE_H | RANK_1 | RANK_8;

constexpr uint8_t NONE = 0;
constexpr uint8_t PAWN = 1;
constexpr uint8_t BISHOP = 2;
constexpr uint8_t KNIGHT = 3;
constexpr uint8_t ROOK = 4;
constexpr uint8_t QUEEN = 5;
constexpr uint8_t KING = 6;

constexpr uint8_t WHITE = 16;
constexpr uint8_t BLACK = 8;

static std::unordered_map <char, unsigned int> PieceTypeFromChar =
{
	{'k', KING}, {'q', QUEEN}, {'b', BISHOP}, {'r', ROOK}, {'p', PAWN}, {'n', KNIGHT}
};

constexpr uint8_t WHITE_PAWN = WHITE + PAWN;     //17
constexpr uint8_t WHITE_BISHOP = WHITE + BISHOP; //18
constexpr uint8_t WHITE_KNIGHT = WHITE + KNIGHT; //19
constexpr uint8_t WHITE_ROOK = WHITE + ROOK;     //20
constexpr uint8_t WHITE_QUEEN = WHITE + QUEEN;   //21
constexpr uint8_t WHITE_KING = WHITE + KING;     //22
constexpr uint8_t BLACK_PAWN = BLACK + PAWN;     //9
constexpr uint8_t BLACK_BISHOP = BLACK + BISHOP; //10
constexpr uint8_t BLACK_KNIGHT = BLACK + KNIGHT; //11
constexpr uint8_t BLACK_ROOK = BLACK + ROOK;     //12
constexpr uint8_t BLACK_QUEEN = BLACK + QUEEN;   //13
constexpr uint8_t BLACK_KING = BLACK + KING;     //14

//ID, TT, search, time management...
constexpr unsigned int LOWER_BOUND = 0; //CUT-NODE (>=beta)
constexpr unsigned int UPPER_BOUND = 1; //ALL-NODE (<=alpha)
constexpr unsigned int EXACT = 2; //PV-NODE
constexpr unsigned int TIME_INTERVAL_IN_NODES = 2048;//tunable

constexpr std::string_view STARTPOS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

constexpr uint8_t NULL_OPTION = 65; //The number i use to mean 'not assigned' or 'doesn't exist'

constexpr int MAX_PV_LENGTH = 50;

static std::random_device seed;
static std::default_random_engine rng(seed());
static std::uniform_int_distribution<uint64_t> dist;

static uint64_t Random64Bit()
{
	return dist(rng);
}

struct CastlingAbility
{
	bool WhiteShort = false;
	bool WhiteLong = false;
	bool BlackShort = false;
	bool BlackLong = false;

	bool at(uint8_t index) const
	{
		switch (index)
		{
		case CastlingIndices::WhiteShort:
			return WhiteShort;
			break;
		case CastlingIndices::WhiteLong:
			return WhiteLong;
			break;
		case CastlingIndices::BlackShort:
			return BlackShort;
			break;
		case CastlingIndices::BlackLong:
			return BlackLong;
			break;
		default:
			ASSERT(false);
		}
	}
};

struct GuessStruct
{
	int16_t GuessedEval = 0;
	uint8_t BoardSquare = NULL_OPTION;
	uint8_t Move = NULL_OPTION;
	uint8_t PromotionType = NULL_OPTION;
	

	GuessStruct(uint8_t BoardSquare, uint8_t Move, uint8_t PromotionType, int32_t GuessedEval)
		: BoardSquare(BoardSquare),
		Move(Move),
		PromotionType(PromotionType),
		GuessedEval(GuessedEval) {
	}
};

struct Move
{
	uint8_t s_BoardSquare = NULL_OPTION;
	uint8_t s_Move = NULL_OPTION;
	uint8_t s_PromotionType = NULL_OPTION;

	Move(const uint8_t& boardSquare, const uint8_t& move, const uint8_t& promotionType)
		: s_BoardSquare(boardSquare), s_Move(move), s_PromotionType(promotionType)
	{
	}
	Move(const uint8_t& boardSquare, const uint8_t& move)
		: s_BoardSquare(boardSquare), s_Move(move)
	{
	}
	Move();
	
	bool operator ==(const Move& a) const
	{
		if (s_BoardSquare == a.s_BoardSquare and s_Move == a.s_Move and s_PromotionType == a.s_PromotionType)
			return true;
		else
			return false;
	}

	explicit Move(const GuessStruct& a)
		:s_BoardSquare(a.BoardSquare), s_Move(a.Move), s_PromotionType(a.PromotionType)
	{ }

	bool IsNull() const noexcept
	{
		if (s_BoardSquare == NULL_OPTION and s_Move == NULL_OPTION and s_PromotionType == NULL_OPTION)
		{
			return true;
		}
		return false;
	}
};

struct Timer
{
	std::chrono::milliseconds WTime;
	std::chrono::milliseconds BTime;
	std::chrono::milliseconds WIncrement;
	std::chrono::milliseconds BIncrement;

	Timer(std::chrono::milliseconds WTime_inms, std::chrono::milliseconds BTime_inms, std::chrono::milliseconds WIncrement_inms, std::chrono::milliseconds BIncrement_inms)
		:WTime(WTime_inms), BTime(BTime_inms), WIncrement(WIncrement_inms), BIncrement(BIncrement_inms)
	{
	}
	Timer(){}
};

struct pv_line
{
	uint8_t NumOfMoves = 0; //Number of moves on line
	std::array<Move, MAX_PV_LENGTH> moves; //the moves
};

struct UCIInfoes
{
	uint8_t* Depth = nullptr;
	uint64_t* NumOfNodes = nullptr;
	uint64_t* NpS = nullptr;
	uint16_t* HashFull = nullptr;
	int32_t* Score = 0;
	pv_line* PV;
};

struct SearchResult
{
	int Eval;
	std::vector<Move> PV;

	SearchResult(const int32_t& f_Eval)
	{
		Eval = f_Eval;
	}
	SearchResult(const int32_t& f_Eval, const std::vector<Move>& Pv)
	{
		Eval = f_Eval;
		PV = Pv;
	}
};

struct Position
{
	std::array<uint8_t, 64> BoardSquare;
	std::array<uint8_t, 64> PrevBoardSquare;
	CastlingAbility CanCastle;
	uint16_t MoveNum;
};

class Board
{
private:
	std::array<uint8_t, 64> BoardSquare;
	const std::string FEN;
	size_t IndexOfSideToMove;//before side to move
	size_t IndexOfCastling;//before castling ability
	size_t IndexOfEnPassant;//before en passant
	size_t IndexOfHalfmoveClock;//before halfmove clock
	size_t IndexOfFullMoveCounter;//end of FEN


public:
	Board(const std::string& FenString);
	std::array<uint8_t, 64> GetPositionFromFEN();
	static std::array<uint8_t, 64> GetPositionFromFEN(const std::string& FenString);
	uint32_t MoveNum();
	CastlingAbility GetCanCastle();
	uint8_t GetPawnMoveSquare();
	static uint8_t ALG2BoardSquareConverter(const std::string& ALG);
	static Move LongALG2Move(const std::string& ALG);
	static std::string Move2ALG(Move move);
	static constexpr uint8_t GetPieceType2Uncolored(uint8_t PieceType);
	static constexpr bool IsPieceColorWhite(uint8_t BoardSquareValue);
	static char PieceType2letter(const uint8_t& PieceType);
	static void WillCanCastleChange(const uint8_t BoardSquareItMovedFrom, CastlingAbility& Castle);
	static bool WillCanCastleChange(const uint8_t& PieceTypeThatMoved, const uint8_t BoardSquareItMovedFrom, const uint8_t BoardSquareItMovedTo);
	static void MakeMove(Move move, std::array<uint8_t, 64>& BoardSquare, uint8_t EnpassantIndex, CastlingAbility Castle);
	static void MakeMove(Move move, std::array<uint8_t, 64>& BoardSquare, std::array<uint8_t, 64>& previousBoardSquare, CastlingAbility& Castle);
	static std::array<uint8_t, 64> PrevBoardSquareFromEP(const std::array<uint8_t, 64>& BoardSquare, uint8_t EPBoardsquare);
	static std::string GetPrintableFromArrayOfMoves(const std::array<Move, MAX_PV_LENGTH>& Moves);
	static uint8_t PieceType2Compact(uint8_t PieceType);
};

template<typename T> inline std::vector<T> GetVecTail(const std::vector<T>& Vec)
{
	if (!Vec.empty())
		return std::vector<T>(Vec.begin() + 1, Vec.end());
	else
		return std::vector<T>();
}
template<typename T> inline void PushBackVec(std::vector<T>& Vec, const std::vector<T>& DataVec)
{
	return (void)Vec.insert(Vec.end(), DataVec.begin(), DataVec.end());
}

inline bool IsStringANum(std::string& String)
{
	bool NumStarted = false;
	bool Warning = false;
	for (auto Char : String)
	{
		if (!isdigit(Char))
		{
			if (Char == ' ' and NumStarted)
			{
				if (!Warning)
				{
					Warning = true;
				}
				else {
					return false;
				}
			}
		}
		else
		{
			NumStarted = true;
		}
	}
	return true;
}

namespace bit//bit management
{

	//pop least significant bit, return false if mask was empty
	constexpr inline bool pop_lsb(uint64_t& Mask, uint8_t& Index)
	{
		Index = std::countr_zero(Mask);
		if (Index < 64)
		{
			Mask ^= (1ULL << Index);
		}
		else { return false; }
		return true;
	}

	/// <summary>
	/// Just finds lsb, TO BE USED WITH A MASK WITH popcnt == 0, WILL ASSERT
	/// </summary>
	/// <param name="Mask">bitboard the set bit will be seached</param>
	/// <param name="Index">bit index will be written to this</param>
	/// <returns>false if Mask is empty</returns>
	constexpr inline bool FindBit(const uint64_t& Mask, uint8_t& Index)
	{
		ASSERT(std::popcount(Mask) == 1);
		Index = std::countr_zero(Mask);
		if (Index < 64) { return true; }
		else { return false; }

	}

	/// <summary>
	/// Uses tzcnt to give the index of the lsb, use this if you NEED to search after the Index,
	/// you should instead make a copy of Bits and use pop_lsb
	/// </summary>
	/// <param name="Index">Setting this means that it will search for the least significant bit AFTER the index, NOT ZERO INCLUDED (e.g. bit[1] is actually bit[0])</param>
	/// <param name="Bits">bits to be scanned</param>
	/// <returns>The least significant bit index, returns 64 if not set</returns>
	constexpr inline uint8_t lsb(size_t Bits, uint8_t Index = 0) noexcept
	{
		ASSERT(Index < 64);
		size_t Mask = (1ULL << Index) - 1;
		Bits = Mask ^ (Bits | Mask);
		return std::countr_zero(Bits);
	}

	class BitManager
	{
	private:
		uint64_t& m_Data;
		uint8_t m_Index;
	public:
		constexpr BitManager(uint64_t& Data, uint8_t Index) : m_Data(Data), m_Index(Index) {}

		//assignment operator
		constexpr BitManager& operator=(bool value);

		//read-only member
		constexpr operator bool() const {
			return (m_Data >> m_Index) & 0b1;
		}

		BitManager& operator=(const BitManager& a) = delete;
	};

	/// <summary>
	/// Simulates a chess board using a 64bit number,
	/// Member functions help this class to integrate in the existing codebase, such as the operator[] accessing
	/// </summary>
	class BitBoard64 {
		uint64_t Bits = 0;
	public:
		//set a bit, logic uses the BitManager operator=
		constexpr BitManager operator[](uint8_t Index)
		{
			if (Index > 63)
			{
				throw std::out_of_range("Indexed bit out of range");
			}
			return BitManager(Bits, Index);
		}

		constexpr BitBoard64(uint64_t bits) : Bits(bits) {}
		constexpr BitBoard64() {}

		//get bit at index
		constexpr bool operator[](uint8_t Index) const
		{
			if (Index > 63)
			{
				throw std::out_of_range("Indexed bit out of range");
			}
			return (Bits >> Index) & 0b1;
		}

		constexpr void fill(bool Value) noexcept
		{
			if (Value) { Bits = UINT64_MAX; }
			else { Bits = 0; }
		}

		//Sets the bit in the parameter as true
		constexpr void inline SetToTrue(uint8_t Index)
		{
			if (Index > 63)
			{
				throw std::out_of_range("Indexed bit out of range");
			}
			Bits |= 1ULL << Index;
		}

		//this should be replaced with SetToTrue
		constexpr void push_back(uint8_t Index) { SetToTrue(Index); }

		constexpr void clear() noexcept
		{
			Bits = 0;
		}

		constexpr inline uint8_t popcnt() const noexcept
		{
			return std::popcount(Bits);
		}

		constexpr bool empty() const noexcept
		{
			return (popcnt() == 0);
		}

		constexpr uint64_t ReadBits() const
		{
			return Bits;
		}

		//vv Bitwise op vv

		constexpr BitBoard64 operator|(BitBoard64 Operand) const
		{
			return Bits | Operand.Bits;
		}
		constexpr BitBoard64 operator&(BitBoard64 Operand) const
		{
			return Bits & Operand.Bits;
		}
		constexpr BitBoard64 operator^(BitBoard64 Operand) const
		{
			return Bits ^ Operand.Bits;
		}

		constexpr BitBoard64& operator|=(BitBoard64 Operand)
		{
			Bits |= Operand.Bits;
			return *this;
		}
		constexpr BitBoard64& operator&=(BitBoard64 Operand)
		{
			Bits &= Operand.Bits;
			return *this;
		}
		constexpr BitBoard64& operator^=(BitBoard64 Operand)
		{
			Bits ^= Operand.Bits;
			return *this;
		}

		constexpr BitBoard64 operator|(uint64_t Operand) const
		{
			return Bits | Operand;
		}
		constexpr BitBoard64 operator&(uint64_t Operand) const
		{
			return Bits & Operand;
		}
		constexpr BitBoard64 operator^(uint64_t Operand) const
		{
			return Bits ^ Operand;
		}
		constexpr operator uint64_t() const { return Bits; }
		constexpr operator uint64_t&() { return Bits; }
	};

	class BitPosManager
	{
	private:
		std::array<BitBoard64, 6>& m_Positions;
		std::array<BitBoard64, 2>& m_Colors;
		uint8_t m_Index;
	public:
		BitPosManager(std::array<BitBoard64, 6>& PosBitBoards, std::array<BitBoard64, 2>& ColorBitBoards, uint8_t Index)
			: m_Positions(PosBitBoards), m_Colors(ColorBitBoards), m_Index(Index) {
		}

		//assignment operator
		BitPosManager& operator=(uint8_t PieceType);

		//read-only member
		operator uint8_t() const;

		BitPosManager& operator=(const BitPosManager& a) = delete;
	};

	class BitPosition
	{
	public:
		std::array<BitBoard64, 6> PiecePositions;//Ascending order: Pawn, Bishop, Knight, Rook, Queen, King (as defined)
		std::array<BitBoard64, 2> ColorPositions;//0th is White, 1st is Black

		BitPosition() {}
		BitPosition(const std::array<uint8_t, 64>& OldBoardSquare);

		//Get PieceType (standard way) from BoardSquare
		uint8_t operator[](uint8_t BoardSquare) const;

		BitPosManager operator[](uint8_t Index);

		uint8_t popcnt() const noexcept;

		inline bool empty() const noexcept
		{
			return (popcnt() == 0);
		}
		
		BitBoard64 find(uint8_t PieceType);

	};

	class EP
	{
	private:
		uint8_t EnPassant = 0;

	public:

		inline void SetEP(uint8_t File)
		{
			EnPassant |= (1ULL << File);
		}

		inline void Reset() { EnPassant = 0; }

		inline bool ReadEp(uint8_t File) const { return EnPassant & (0x10 >> File); }
	};

	struct Position
	{
		bit::BitPosition BoardSquare;
		CastlingAbility CanCastle;
		uint16_t MoveNum;
		EP EnPassant;
	};

}


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

constexpr enum SQUARES
{
	a1, a2, a3, a4, a5, a6, a7, a8,
	b1, b2, b3, b4, b5, b6, b7, b8,
	c1, c2, c3, c4, c5, c6, c7, c8,
	d1, d2, d3, d4, d5, d6, d7, d8,
	e1, e2, e3, e4, e5, e6, e7, e8,
	f1, f2, f3, f4, f5, f6, f7, f8,
	g1, g2, g3, g4, g5, g6, g7, g8,
	h1, h2, h3, h4, h5, h6, h7, h8
};
constexpr uint8_t MAX_SQUARE = 63;

constexpr uint64_t BORDER_MASK = 0b1111111110000001100000011000000110000001100000011000000111111111;

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

static std::random_device seed;
static std::default_random_engine rng(seed());
static std::uniform_int_distribution<uint64_t> dist;

static uint64_t Random64Bit()
{
	return dist(rng);
}

struct canCastle
{
	bool HasWhiteLongRookMoved = false, HasWhiteShortRookMoved = false, HasBlackLongRookMoved = false, HasBlackShortRookMoved = false, HasWhiteKingMoved = false, HasBlackKingMoved = false;

};

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
		case 0:
			return WhiteShort;
			break;
		case 1:
			return WhiteLong;
			break;
		case 2:
			return BlackShort;
			break;
		case 3:
			return BlackLong;
			break;
		default:
			ASSERT(false);
		}
	}
};

struct GuessStruct
{
	uint8_t BoardSquare = NULL_OPTION;
	uint8_t Move = NULL_OPTION;
	uint8_t PromotionType = NULL_OPTION;
	int16_t GuessedEval;

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

struct UCIInfoes
{
	uint8_t* Depth = nullptr;
	uint64_t* NumOfNodes = nullptr;
	uint64_t* NpS = nullptr;
	uint16_t* HashFull = nullptr;
	int32_t* Score = 0;
	std::vector<Move>* PV;
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
	canCastle GetCanCastle();
	uint8_t GetPawnMoveSquare();
	static uint8_t ALG2BoardSquareConverter(const std::string& ALG);
	static Move LongALG2Move(const std::string& ALG);
	static std::string Move2ALG(Move move);
	static inline uint8_t GetPieceType2Uncolored(const uint8_t& PieceType);
	static CastlingAbility canCastle2CastlingAbility(const canCastle& Castle);
	static inline bool IsPieceColorWhite(const uint8_t& BoardSquareValue);
	static char PieceType2letter(const uint8_t& PieceType);
	static void WillCanCastleChange(const uint8_t& PieceTypeThatMoved, const uint8_t& BoardSquareItMovedFrom, const uint8_t& BoardSquareItMovedTo, canCastle& Castle);
	static bool WillCanCastleChange(const uint8_t& PieceTypeThatMoved, const uint8_t& BoardSquareItMovedFrom, const uint8_t& BoardSquareItMovedTo);
	static void MakeMove(Move move, std::array<uint8_t, 64>& BoardSquare, uint8_t EnpassantIndex, canCastle Castle);
	static void MakeMove(Move move, std::array<uint8_t, 64>& BoardSquare, std::array<uint8_t, 64>& previousBoardSquare, canCastle& Castle);
	static std::array<uint8_t, 64> PrevBoardSquareFromEP(const std::array<uint8_t, 64>& BoardSquare, uint8_t EPBoardsquare);
	static std::string GetPrintableFromVecOfMoves(std::vector<Move> Moves);
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

namespace bit//bit management
{

	//pop least significant bit, return false if mask was empty
	constexpr inline bool pop_lsb(uint64_t& Mask, uint8_t& Index)
	{
		unsigned long Index_ = std::countr_zero(Mask);
		if (Index_ < 64)
		{
			Mask ^= (1ULL << Index_);
			Index = Index_;
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

	class BitManager
	{
	private:
		uint64_t& m_Data;
		uint8_t m_Index;
	public:
		BitManager(uint64_t& Data, uint8_t Index) : m_Data(Data), m_Index(Index) {}

		//assignment operator
		inline BitManager& operator=(bool value);

		//read-only member
		operator bool() const {
			return (m_Data >> m_Index) & 0b1;
		}

		BitManager& operator=(const BitManager& a) = delete;
	};

	//this class might be made strange, but it's like this to achieve a kind of bit array (and to replace the array of bools without refactoring the code)
	class BitBoard64 {
		uint64_t Bits = 0;
	public:
		//set a bit, logic uses the BitManager operator=
		BitManager operator[](uint8_t Index)
		{
			if (Index > 63)
			{
				throw std::out_of_range("Indexed bit out of range");
			}
			return BitManager(Bits, Index);
		}

		BitBoard64(uint64_t bits) : Bits(bits) {}
		BitBoard64() {}

		//get bit at index
		bool operator[](uint8_t Index) const
		{
			if (Index > 63)
			{
				throw std::out_of_range("Indexed bit out of range");
			}
			return (Bits >> Index) & 0b1;
		}

		inline void fill(bool Value) noexcept
		{
			if (Value) { Bits = UINT64_MAX; }
			else { Bits = 0; }
		}

		//Sets the bit in the parameter as true
		void SetToTrue(uint8_t Index)
		{
			if (Index > 63)
			{
				throw std::out_of_range("Indexed bit out of range");
			}
			Bits |= 1ULL << Index;
		}

		//this should be replaced with SetToTrue
		inline void push_back(uint8_t Index) { SetToTrue(Index); }

		inline void clear() noexcept
		{
			fill(false);
		}

		inline uint8_t popcnt() const noexcept
		{
			return std::popcount(Bits);
		}

		inline bool empty() const noexcept
		{
			return (popcnt() == 0);
		}

		inline uint64_t ReadBits() const
		{
			return Bits;
		}

		//vv Bitwise op vv

		inline BitBoard64 operator|(BitBoard64 Operand) const
		{
			return Bits | Operand.Bits;
		}
		inline BitBoard64 operator&(BitBoard64 Operand) const
		{
			return Bits & Operand.Bits;
		}
		inline BitBoard64 operator^(BitBoard64 Operand) const
		{
			return Bits ^ Operand.Bits;
		}

		inline BitBoard64& operator|=(BitBoard64 Operand)
		{
			Bits |= Operand.Bits;
			return *this;
		}
		inline BitBoard64& operator&=(BitBoard64 Operand)
		{
			Bits &= Operand.Bits;
			return *this;
		}
		inline BitBoard64& operator^=(BitBoard64 Operand)
		{
			Bits ^= Operand.Bits;
			return *this;
		}

		inline operator uint64_t() { return Bits; }
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

}
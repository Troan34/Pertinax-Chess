#pragma once
#include <string>
#include <unordered_map>
#include <array>
#include <Renderer.h>
#include <cmath>
#include <iostream>
#include <chrono>
#include <print>

static constexpr unsigned int NONE = 0;
static constexpr unsigned int PAWN = 1;
static constexpr unsigned int BISHOP = 2;
static constexpr unsigned int KNIGHT = 3;
static constexpr unsigned int ROOK = 4;
static constexpr unsigned int QUEEN = 5;
static constexpr unsigned int KING = 6;

static constexpr unsigned int WHITE = 16;
static constexpr unsigned int BLACK = 8;

static std::unordered_map <char, unsigned int> PieceTypeFromChar =
{
	{'k', KING}, {'q', QUEEN}, {'b', BISHOP}, {'r', ROOK}, {'p', PAWN}, {'n', KNIGHT}
};

static constexpr unsigned int WHITE_PAWN = WHITE + PAWN;     //17
static constexpr unsigned int WHITE_BISHOP = WHITE + BISHOP; //18
static constexpr unsigned int WHITE_KNIGHT = WHITE + KNIGHT; //19
static constexpr unsigned int WHITE_ROOK = WHITE + ROOK;     //20
static constexpr unsigned int WHITE_QUEEN = WHITE + QUEEN;   //21
static constexpr unsigned int WHITE_KING = WHITE + KING;     //22
static constexpr unsigned int BLACK_PAWN = BLACK + PAWN;     //9
static constexpr unsigned int BLACK_BISHOP = BLACK + BISHOP; //10
static constexpr unsigned int BLACK_KNIGHT = BLACK + KNIGHT; //11
static constexpr unsigned int BLACK_ROOK = BLACK + ROOK;     //12
static constexpr unsigned int BLACK_QUEEN = BLACK + QUEEN;   //13
static constexpr unsigned int BLACK_KING = BLACK + KING;     //14

//ID, TT, search, time management...
static constexpr unsigned int LOWER_BOUND = 0; //CUT-NODE (>=beta)
static constexpr unsigned int UPPER_BOUND = 1; //ALL-NODE (<=alpha)
static constexpr unsigned int EXACT = 2; //PV-NODE
static constexpr unsigned int TIME_INTERVAL_IN_NODES = 2048;//tunable

static constexpr std::string_view STARTPOS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

static constexpr uint8_t NULL_OPTION = 65; //The number i use to mean 'not assigned' or 'doesn't exist'

//this might be made strange, but it's like this to achieve a kind of bit array (and to replace the array of bools without refactoring the code)
struct BitBoard64 {
	uint64_t Bits = 0;

	//set a bit, logic uses the BitManager operator=
	BitManager operator[](uint8_t Index)
	{
		if (Index > 63)
		{
			throw std::out_of_range("Indexed bit out of range");
		}
		return BitManager(Bits, Index);
	}

	//get bit from index
	bool operator[](uint8_t Index) const
	{
		if (Index > 63)
		{
			throw std::out_of_range("Indexed bit out of range");
		}
		return (Bits >> Index) & 0b1;
	}

	void fill(bool Value) noexcept
	{
		if (true) { Bits = UINT64_MAX; }
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

};

struct BitPosition
{
	std::array<BitBoard64, 6> PiecePositions;//Ascending order: Pawn, Bishop, Knight, Rook, Queen, King (as defined)
	std::array<BitBoard64, 2> ColorPositions;//0th is White, 1st is Black

	//Get PieceType (normal standard way) from BoardSquare
	uint8_t operator[](uint8_t BoardSquare) const
	{
		uint8_t PieceType = NONE;
		uint8_t color = 16;

		for (const BitBoard64& BitColor : ColorPositions)
		{
			if (BitColor[BoardSquare] == false) { color -= 8; continue; }
			else
			{
				uint8_t count = 0;
				for (const BitBoard64& BitPieceType : PiecePositions)
				{
					if (BitPieceType[BoardSquare] == false) { count++;  continue; }
					else
					{
						PieceType = color | (++count);
					}
				}
			}
			
		}

		return PieceType;
	}

	BitManager operator[](uint8_t Index)
	{
		uint64_t Bits;
		uint8_t color = 16;
		for (BitBoard64& BitColor : ColorPositions)
		{

		}

	}
};

class BitManager
{
	uint64_t& m_Data;
	std::optional<uint64_t&> m_Data2;
	uint8_t m_Index;
public:
	BitManager(uint64_t& Data, uint8_t Index) : m_Data(Data), m_Index(Index) {}
	BitManager(uint64_t& Data, uint64_t& Data2, uint8_t Index) : m_Data(Data), m_Data2(Data2), m_Index(Index) {}

	BitManager& operator=(bool value) {
		if (value) {
			m_Data |= (1ULL << m_Index);
			if (m_Data2.has_value())
			{
				m_Data2.value() |= (1ULL << m_Index);
			}
		}
		else {
			m_Data &= ~(1ULL << m_Index);
			if (m_Data2.has_value())
			{
				m_Data2.value() |= (1ULL << m_Index);
			}
		}
		return *this;
	}

	operator bool() const {
		return (m_Data >> m_Index) & 0b1;
	}

	BitManager& operator=(const BitManager& a) = delete;
};

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
	BitBoard64 BoardSquare_NEW;
	const std::string FEN;
	size_t IndexOfSideToMove;//before side to move
	size_t IndexOfCastling;//before castling ability
	size_t IndexOfEnPassant;//before en passant
	size_t IndexOfHalfmoveClock;//before halfmove clock
	size_t IndexOfFullMoveCounter;//end of FEN


public:
	Board(const std::string& FenString);
	std::array<uint8_t, 64> GetPositionFromFEN();
	BitBoard64 GetBitBoardFromFEN();
	static std::array<uint8_t, 64> GetPositionFromFEN(const std::string& FenString);
	uint32_t MoveNum();
	canCastle GetCanCastle();
	uint8_t GetPawnMoveSquare();
	static uint8_t ALG2BoardSquareConverter(const std::string& ALG);
	static Move LongALG2Move(const std::string& ALG);
	static std::string Move2ALG(Move move);
	static uint8_t GetPieceType2Uncolored(const uint8_t& PieceType);
	static CastlingAbility canCastle2CastlingAbility(const canCastle& Castle);
	static bool IsPieceColorWhite(const uint8_t& BoardSquareValue);
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

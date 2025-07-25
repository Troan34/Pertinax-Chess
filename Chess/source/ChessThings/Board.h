#pragma once
#include <string>
#include <unordered_map>
#include <array>
#include <Renderer.h>
#include <cmath>
#include <iostream>
#include <chrono>

static constexpr unsigned int NONE = 0;
static constexpr unsigned int PAWN = 1;
static constexpr unsigned int BISHOP = 2;
static constexpr unsigned int KNIGHT = 3;
static constexpr unsigned int ROOK = 4;
static constexpr unsigned int QUEEN = 5;
static constexpr unsigned int KING = 6;

static constexpr unsigned int WHITE = 16;
static constexpr unsigned int BLACK = 8;

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
static constexpr unsigned int TIME_INTERVAL_IN_NODES = 2048;//optimizable 



static constexpr std::string_view STARTPOS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";


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
	uint8_t BoardSquare;
	uint8_t Move;
	uint8_t PromotionType = 65;
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
	uint8_t s_BoardSquare;
	uint8_t s_Move;
	uint8_t s_PromotionType = 65;

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
	uint32_t* NpS = nullptr;
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

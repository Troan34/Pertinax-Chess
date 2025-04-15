#pragma once
#include <string>
#include <unordered_map>
#include <array>
#include <Renderer.h>

constexpr unsigned int NONE = 0;
constexpr unsigned int PAWN = 1;
constexpr unsigned int BISHOP = 2;
constexpr unsigned int KNIGHT = 3;
constexpr unsigned int ROOK = 4;
constexpr unsigned int QUEEN = 5;
constexpr unsigned int KING = 6;

constexpr unsigned int WHITE = 16;
constexpr unsigned int BLACK = 8;

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

	bool operator[](uint8_t index)
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

class Board
{
private:
	std::array<uint8_t, 64> BoardSquare;
	const std::string FEN;
	size_t IndexOfSideToMove;//before side to move
	size_t IndexOfCastling;//before castling ability
	size_t IndexOfEnPassant;//before en passant
	size_t IndexOfHalfmoveClock;//before halfmove clock
	size_t IndexOfFullMoveCounter;//before halfmove clock


public:
	Board(const std::string& FenString);
	std::array<uint8_t, 64> GetPositionFromFEN();
	uint32_t MoveNum();
	canCastle GetCanCastle();
	uint32_t GetPawnMoveSquare();
	uint32_t ALG2BoardSquareConverter(const std::string& ALG);
	static CastlingAbility canCastle2CastlingAbility(const canCastle& Castle);
	static bool IsPieceColorWhite(const uint8_t& BoardSquareValue);
	static char PieceType2letter(const uint8_t& PieceType);
	static void WillCanCastleChange(const uint8_t& PieceTypeThatMoved, const uint8_t& BoardSquareItMovedFrom, const uint8_t& BoardSquareItMovedTo, canCastle& Castle);
};

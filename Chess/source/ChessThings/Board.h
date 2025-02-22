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

struct canCastle
{
	bool HasWhiteLongRookMoved = false, HasWhiteShortRookMoved = false, HasBlackLongRookMoved = false, HasBlackShortRookMoved = false, HasWhiteKingMoved = false, HasBlackKingMoved = false;
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
	static bool IsPieceColorWhite(const uint8_t& BoardSquareValue);
	static char PieceType2letter(const uint8_t& PieceType);
};

#pragma once
#include <string>
#include <unordered_map>
#include <array>
#include <Renderer.h>

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
	static bool IsPieceColorWhite(unsigned int BoardSquareValue);
	static char PieceType2letter(const uint8_t& PieceType);
};

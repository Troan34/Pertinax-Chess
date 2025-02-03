#pragma once
#include <string>
#include <unordered_map>
#include <array>

class Board
{
private:
	std::array<unsigned int, 64> BoardSquare;
	const std::string FEN;
	size_t IndexOfSideToMove;//before side to move
	size_t IndexOfCastling;//before castling ability
	size_t IndexOfEnPassant;//before en passant
	size_t IndexOfHalfmoveClock;//before halfmove clock
	size_t IndexOfFullMoveCounter;//before halfmove clock
public:
	Board(const std::string& FenString);
	std::array<unsigned int, 64> GetPositionFromFEN();
	uint32_t MoveNum();
	static bool IsPieceColorWhite(unsigned int BoardSquareValue);
};

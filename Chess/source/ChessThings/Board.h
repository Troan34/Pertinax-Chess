#pragma once
#include <string>
#include <unordered_map>
#include <array>

class Board
{
private:
	std::array<unsigned int, 64> BoardSquare;
public:
	Board();
	std::array<unsigned int, 64> GetPositionFromFEN(std::string FenString);
	static bool IsPieceColorWhite(unsigned int BoardSquareValue);
};

#include "Board.h"
#include <iostream>
class Piece
{
public:
	const unsigned int None = 0;
	const unsigned int Pawn = 1;
	const unsigned int Bishop = 2;
	const unsigned int Knight = 3;
	const unsigned int Rook = 4;
	const unsigned int Queen = 5;
	const unsigned int King = 6;

	const unsigned int White = 16;
	const unsigned int Black = 8;
};
Board::Board(const std::string& FenString)
	:BoardSquare(), FEN(FenString), IndexOfSideToMove(FEN.find(' ')),
	IndexOfCastling(FEN.find(' ', IndexOfSideToMove + 1)),
	IndexOfEnPassant(FEN.find(' ', IndexOfCastling + 1)),
	IndexOfHalfmoveClock(FEN.find(' ', IndexOfEnPassant + 1)),
	IndexOfFullMoveCounter(FEN.find(' ', IndexOfHalfmoveClock + 1))
{

}
std::array<unsigned int, 64> Board::GetPositionFromFEN()
{
	Piece piece;
	std::unordered_map <char, unsigned int> PieceTypeFromChar =
	{
		{'k', piece.King}, {'q', piece.Queen}, {'b', piece.Bishop}, {'r', piece.Rook}, {'p', piece.Pawn}, {'n', piece.Knight}
	};
	std::string FenPiecePlacement = FEN.substr(0, IndexOfSideToMove);
	
	unsigned int file = 0, rank = 7;
	for (char character : FenPiecePlacement)
	{
		if (character == '/')
		{
			file = 0;
			rank -= 1;
		}
		else if(isdigit(character))
		{
			file += character - '0';
		}
		else
		{
			unsigned int PieceColor = isupper(character) ? piece.White : piece.Black;
			unsigned int PieceType = PieceTypeFromChar[tolower(character)];
			BoardSquare[static_cast<std::array<unsigned int, 64Ui64>::size_type>(rank) * 8 + file] = PieceType | PieceColor;
			file += 1;

		}
	}
	return BoardSquare;
}

uint32_t Board::MoveNum()
{
	std::string FullMove = FEN.substr(IndexOfFullMoveCounter + 1, std::string::npos);

	if (FEN.at(IndexOfSideToMove + 1) == 'w')
	{
		return (std::stoul(FullMove) * 2) - 2;
	}
	else
		return (std::stoul(FullMove) * 2) - 1;
}

bool Board::IsPieceColorWhite(unsigned int BoardSquareValue)
{
	if (BoardSquareValue >= 17)
		return true;
	try
	{
		if (BoardSquareValue == 0)
		{
			throw std::runtime_error("Value == 0(not a color) Error");
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Caught exception: " << e.what() << '\n';
	}
	return false;
}

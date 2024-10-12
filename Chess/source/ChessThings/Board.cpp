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

	const unsigned int White = 8;
	const unsigned int Black = 16;
};
Board::Board()
	:BoardSquare()
{

}
std::array<unsigned int, 64> Board::GetPositionFromFEN(std::string FenString)
{
	Piece piece;
	std::unordered_map <char, unsigned int> PieceTypeFromChar =
	{
		{'k', piece.King}, {'q', piece.Queen}, {'b', piece.Bishop}, {'r', piece.Rook}, {'p', piece.Pawn}, {'n', piece.Knight}
	};
	std::string FenPiecePlacement = FenString.substr(0, FenString.find(' '));
	
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

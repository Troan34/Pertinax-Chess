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
std::array<uint8_t, 64> Board::GetPositionFromFEN()
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

canCastle Board::GetCanCastle()
{
	canCastle CanCastle{};
	const std::string CastlingAbility = FEN.substr(++IndexOfCastling, IndexOfEnPassant - IndexOfCastling);
	bool K = 0;
	bool k = 0;
	bool Q = 0;
	bool q = 0;
	for (char castleCharacter : CastlingAbility)
	{
		if (castleCharacter == '-')
			break;

		if (castleCharacter == 'K')
		{
			++K;
			continue;
		}
		else if (castleCharacter == 'k')
		{
			++k;
			continue;
		}
		else if (castleCharacter == 'Q')
		{
			++Q;
			continue;
		}
		else if (castleCharacter == 'q')
		{
			++q;
			continue;
		}
	}

	if (K or Q)
	{
		if (!K and Q)
			CanCastle.HasWhiteShortRookMoved = true;
		else if (!Q and K)
			CanCastle.HasWhiteLongRookMoved = true;
	}
	else
	{
		CanCastle.HasWhiteKingMoved = true;
		CanCastle.HasWhiteLongRookMoved = true;
		CanCastle.HasWhiteShortRookMoved = true;
	}

	if (k or q)
	{
		if (!k and q)
			CanCastle.HasBlackShortRookMoved = true;
		else if (!q and k)
			CanCastle.HasBlackLongRookMoved = true;
	}
	else
	{
		CanCastle.HasBlackKingMoved = true;
		CanCastle.HasBlackLongRookMoved = true;
		CanCastle.HasBlackShortRookMoved = true;
	}
	return CanCastle;
}

uint32_t Board::GetPawnMoveSquare()
{
	const std::string PawnMove = FEN.substr(++IndexOfEnPassant, IndexOfHalfmoveClock - IndexOfEnPassant);

	if (PawnMove == "-")
		return 65;

	return ALG2BoardSquareConverter(PawnMove);
}

uint32_t Board::ALG2BoardSquareConverter(const std::string& ALG)
{
	uint8_t file = 0;
	uint8_t rank = 0;
	switch (ALG[0])
	{
	case 'a':
		break;
	case 'b':
		file = 1;
		break;
	case 'c':
		file = 2;
		break;
	case 'd':
		file = 3;
		break;
	case 'e':
		file = 4;
		break;
	case 'f':
		file = 5;
		break;
	case 'g':
		file = 6;
		break;
	case 'h':
		file = 7;
		break;
	default:
		break;
	}
	rank = ALG[1] - '0' - 1;

	return rank * 8 + file;
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

char Board::PieceType2letter(const uint8_t& PieceType)
{
	switch (PieceType)
	{
	case 9:
		return 'p';
		break;
	case 10:
		return 'b';
		break;
	case 11:
		return 'n';
		break;
	case 12:
		return 'r';
		break;
	case 13:
		return 'q';
		break;
	case 14:
		return 'k';
		break;
	case 17:
		return 'P';
		break;
	case 18:
		return 'B';
		break;
	case 19:
		return 'N';
		break;
	case 20:
		return 'R';
		break;
	case 21:
		return 'Q';
		break;
	case 22:
		return 'K';
		break;
	default:
		ASSERT(false);
	}
}

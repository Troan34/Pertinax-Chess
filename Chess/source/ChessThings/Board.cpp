#include "Board.h"
#include <iostream>


Board::Board(const std::string& FenString)
	:BoardSquare(), FEN(FenString), IndexOfSideToMove(FEN.find(' ')),
	IndexOfCastling(FEN.find(' ', IndexOfSideToMove + 1)),
	IndexOfEnPassant(FEN.find(' ', IndexOfCastling + 1)),
	IndexOfHalfmoveClock(FEN.find(' ', IndexOfEnPassant + 1)),
	IndexOfFullMoveCounter(FEN.find(' ', IndexOfHalfmoveClock + 1))
{

}


//VV from fen to normal values funs VV
std::array<uint8_t, 64> Board::GetPositionFromFEN()
{
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
			unsigned int PieceColor = isupper(character) ? WHITE : BLACK;
			unsigned int PieceType = PieceTypeFromChar[tolower(character)];
			BoardSquare[static_cast<std::array<unsigned int, 64Ui64>::size_type>(rank) * 8 + file] = PieceType | PieceColor;
			file += 1;

		}
	}
	return BoardSquare;
}

BitBoard64 Board::GetBitBoardFromFEN()
{
	std::string FenPiecePlacement = FEN.substr(0, IndexOfSideToMove);

	unsigned int file = 0, rank = 7;
	for (char character : FenPiecePlacement)
	{
		if (character == '/')
		{
			file = 0;
			rank -= 1;
		}
		else if (isdigit(character))
		{
			file += character - '0';
		}
		else
		{
			unsigned int PieceColor = isupper(character) ? WHITE : BLACK;
			unsigned int PieceType = PieceTypeFromChar[tolower(character)];
			BoardSquare_NEW[static_cast<std::array<unsigned int, 64Ui64>::size_type>(rank) * 8 + file] = PieceType | PieceColor;
			file += 1;

		}
	}
	return BoardSquare_NEW;
}

std::array<uint8_t, 64> Board::GetPositionFromFEN(const std::string& Fen)
{
	size_t IndexOfSideToMove = Fen.find(' ');
	size_t IndexOfCastling = Fen.find(' ', IndexOfSideToMove + 1);
	size_t IndexOfEnPassant = Fen.find(' ', IndexOfCastling + 1);
	size_t IndexOfHalfmoveClock = Fen.find(' ', IndexOfEnPassant + 1);
	size_t IndexOfFullMoveCounter = Fen.find(' ', IndexOfHalfmoveClock + 1);

	std::array<uint8_t, 64> BoardSquare;
	BoardSquare.fill(0);


	std::string FenPiecePlacement = Fen.substr(0, IndexOfSideToMove);

	unsigned int file = 0, rank = 7;
	for (char character : FenPiecePlacement)
	{
		if (character == '/')
		{
			file = 0;
			rank -= 1;
		}
		else if (isdigit(character))
		{
			file += character - '0';
		}
		else
		{
			unsigned int PieceColor = isupper(character) ? WHITE : BLACK;
			unsigned int PieceType = PieceTypeFromChar[tolower(character)];
			BoardSquare[static_cast<std::array<unsigned int, 64Ui64>::size_type>(rank) * 8 + file] = PieceType | PieceColor;
			file += 1;

		}
	}
	return BoardSquare;
}

//get the fullmove num(doubled to be compatible with the rest)
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

//get castling abilities
canCastle Board::GetCanCastle()
{
	canCastle CanCastle{};
	const std::string CastlingAbility = FEN.substr(++IndexOfCastling, IndexOfEnPassant - IndexOfCastling - 1);
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
			K = true;
			continue;
		}
		else if (castleCharacter == 'k')
		{
			k = true;
			continue;
		}
		else if (castleCharacter == 'Q')
		{
			Q = true;
			continue;
		}
		else if (castleCharacter == 'q')
		{
			q = true;
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

//get the boardsquare where an e.p. is possible, 65 if not
uint8_t Board::GetPawnMoveSquare()
{
	const std::string PawnMove = FEN.substr(++IndexOfEnPassant, IndexOfHalfmoveClock - IndexOfEnPassant - 1);

	if (PawnMove == "-")
		return 65;

	return ALG2BoardSquareConverter(PawnMove);
}


//VV general (mostly traduction) funs VV
uint8_t Board::ALG2BoardSquareConverter(const std::string& ALG)
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

Move Board::LongALG2Move(const std::string& ALG)
{
	Move TranslatedMove;

	TranslatedMove.s_BoardSquare = ALG2BoardSquareConverter(ALG.substr(0, 2));
	TranslatedMove.s_Move = ALG2BoardSquareConverter(ALG.substr(2, 2));
	
	if (!isdigit(ALG.back()))
	{
		//this WON'T work, make other functions check if the promotiontype is their color
		switch (ALG.back())
		{
		case 'q':
			TranslatedMove.s_PromotionType = QUEEN;
			break;
		case 'r':
			TranslatedMove.s_PromotionType = ROOK;
			break;
		case 'b':
			TranslatedMove.s_PromotionType = BISHOP;
			break;
		case 'n':
			TranslatedMove.s_PromotionType = KNIGHT;
			break;
		default:
			ASSERT(false);
		}
	}

	return TranslatedMove;
}

std::string Board::Move2ALG(Move move)
{
	std::string ALG;
	char column = move.s_BoardSquare % 8 + 'a';
	char row = move.s_BoardSquare/8 + '0' + 1;
	ALG += column;//fun fact, += is just push_back
	ALG += row;
	column = move.s_Move % 8 + 'a';
	row = move.s_Move / 8 + '0' + 1;
	ALG += column;
	ALG += row;
	if (move.s_PromotionType != 65)
	{
		ALG += PieceType2letter(GetPieceType2Uncolored(move.s_PromotionType) + 9);//This is some traduction gibberish for UCI
	}

	return ALG;
}

uint8_t Board::GetPieceType2Uncolored(const uint8_t& PieceType)
{
	if (PieceType > 16)
		return(PieceType - 16);
	else if (PieceType > 8)
		return(PieceType - 8);
	else
		ASSERT(false);
}

CastlingAbility Board::canCastle2CastlingAbility(const canCastle& Castle)
{
	CastlingAbility CastlingAbility;

	CastlingAbility.WhiteLong = !Castle.HasWhiteLongRookMoved and !Castle.HasWhiteKingMoved;
	CastlingAbility.WhiteShort = !Castle.HasWhiteShortRookMoved and !Castle.HasWhiteKingMoved;
	CastlingAbility.BlackLong = !Castle.HasBlackLongRookMoved and !Castle.HasBlackKingMoved;
	CastlingAbility.BlackShort = !Castle.HasBlackShortRookMoved and !Castle.HasBlackKingMoved;

	return CastlingAbility;
}

bool Board::IsPieceColorWhite(const uint8_t& BoardSquareValue)
{
	if (BoardSquareValue == 0)
		ASSERT(false);
	return (BoardSquareValue >= 17);
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

void Board::WillCanCastleChange(const uint8_t& PieceType, const uint8_t& BoardSquareNumItMovedFrom, const uint8_t& BoardSquareItMovedTo, canCastle& Castle)
{
	if (BoardSquareNumItMovedFrom == 4 and PieceType == WHITE_KING)
	{
		Castle.HasWhiteKingMoved = true;
		if (BoardSquareItMovedTo == 6)
		{
			Castle.HasWhiteShortRookMoved = true;
		}
		else if (BoardSquareItMovedTo == 2)
		{
			Castle.HasWhiteLongRookMoved = true;
		}
	}
	else if (PieceType == BLACK_KING and BoardSquareNumItMovedFrom == 60)
	{
		Castle.HasBlackKingMoved = true;
		if (BoardSquareItMovedTo == 62)
		{
			Castle.HasBlackShortRookMoved = true;
		}
		else if (BoardSquareItMovedTo == 58)
		{
			Castle.HasBlackLongRookMoved = true;
		}
	}
	else if (PieceType == WHITE_ROOK and BoardSquareNumItMovedFrom == 0 or BoardSquareItMovedTo == 0)
		Castle.HasWhiteLongRookMoved = true;
	else if (PieceType == WHITE_ROOK and BoardSquareNumItMovedFrom == 7 or BoardSquareItMovedTo == 7)
		Castle.HasWhiteShortRookMoved = true;
	else if (PieceType == BLACK_ROOK and BoardSquareNumItMovedFrom == 63 or BoardSquareItMovedTo == 63)
		Castle.HasBlackShortRookMoved = true;
	else if (PieceType == BLACK_ROOK and BoardSquareNumItMovedFrom == 56 or BoardSquareItMovedTo == 56)
		Castle.HasBlackLongRookMoved = true;
}

bool Board::WillCanCastleChange(const uint8_t& PieceType, const uint8_t& BoardSquareNumItMovedFrom, const uint8_t& BoardSquareItMovedTo)
{
	if (BoardSquareNumItMovedFrom == 4 and PieceType == WHITE_KING)
	{
		return true;
	}
	else if (PieceType == BLACK_KING and BoardSquareNumItMovedFrom == 60)
	{
		return true;
	}
	else if (PieceType == WHITE_ROOK and BoardSquareNumItMovedFrom == 0 or BoardSquareItMovedTo == 0)
		return true;
	else if (PieceType == WHITE_ROOK and BoardSquareNumItMovedFrom == 7 or BoardSquareItMovedTo == 7)
		return true;
	else if (PieceType == BLACK_ROOK and BoardSquareNumItMovedFrom == 63 or BoardSquareItMovedTo == 63)
		return true;
	else if (PieceType == BLACK_ROOK and BoardSquareNumItMovedFrom == 56 or BoardSquareItMovedTo == 56)
		return true;

	return false;
}

void Board::MakeMove(Move move, std::array<uint8_t, 64>& BoardSquare, uint8_t EnpassantIndex, canCastle Castle)
{
	Board::WillCanCastleChange(BoardSquare[move.s_BoardSquare], move.s_BoardSquare, move.s_Move, Castle);
	BoardSquare[move.s_Move] = BoardSquare[move.s_BoardSquare];

	uint8_t PieceTypeToPromoteTo = 65;

	if (move.s_PromotionType < 9)
	{
		if (Board::IsPieceColorWhite(BoardSquare[move.s_BoardSquare]) and move.s_PromotionType != 65)
			PieceTypeToPromoteTo = move.s_PromotionType + WHITE;
		else
			PieceTypeToPromoteTo = move.s_PromotionType + BLACK;
	}
	else
	{
		PieceTypeToPromoteTo = move.s_PromotionType;
	}

	//castling
	if (BoardSquare[move.s_BoardSquare] == WHITE_KING or BoardSquare[move.s_BoardSquare] == BLACK_KING)
	{
		if (move.s_BoardSquare - move.s_Move == -2)
		{
			if (move.s_BoardSquare == 4)
			{
				BoardSquare[5] = WHITE_ROOK;
				BoardSquare[7] = 0;
			}
			else
			{
				BoardSquare[61] = BLACK_ROOK;
				BoardSquare[63] = 0;
			}
		}
		if (move.s_BoardSquare - move.s_Move == 2)
		{
			if (move.s_BoardSquare == 4)
			{
				BoardSquare[3] = WHITE_ROOK;
				BoardSquare[0] = 0;
			}
			else
			{
				BoardSquare[59] = BLACK_ROOK;
				BoardSquare[56] = 0;
			}
		}

	}

	//promoting and en passant
	if (PieceTypeToPromoteTo != 65)
	{
		BoardSquare[move.s_Move] = PieceTypeToPromoteTo;
	}
	else//optimization
	{
		//White en passant
		if (BoardSquare[move.s_BoardSquare] == WHITE_PAWN)
		{
			if (move.s_BoardSquare == EnpassantIndex)
			{
				BoardSquare[move.s_Move - 8] = 0;
			}
		}
		//Black en passant
		if (BoardSquare[move.s_BoardSquare] == BLACK_PAWN)
		{
			if (move.s_BoardSquare == EnpassantIndex)
			{
				BoardSquare[move.s_Move + 8] = 0;
			}
		}
	}

	BoardSquare[move.s_BoardSquare] = 0;

	/*not having this causes a bug where a pawn enpassanting does an illegal move
	GenerateLegalMoves::SetDoNotEnPassant(false);
	if (abs(move.s_BoardSquare - move.s_Move) == 16 and LegalMoves.WhichBoardSquaresAreAbsPinned[move.s_Move] != 65)
	{
		GenerateLegalMoves::SetDoNotEnPassant(true);
	}
	*/
}

void Board::MakeMove(Move move, std::array<uint8_t, 64>& BoardSquare, std::array<uint8_t, 64>& previousBoardSquare, canCastle& Castle)
{
	previousBoardSquare = BoardSquare;
	Board::WillCanCastleChange(BoardSquare[move.s_BoardSquare], move.s_BoardSquare, move.s_Move, Castle);
	BoardSquare[move.s_Move] = BoardSquare[move.s_BoardSquare];

	uint8_t PieceTypeToPromoteTo = 65;

	if (move.s_PromotionType < 9)
	{
		if (Board::IsPieceColorWhite(BoardSquare[move.s_BoardSquare]) and move.s_PromotionType != 65)
			PieceTypeToPromoteTo = move.s_PromotionType + WHITE;
		else
			PieceTypeToPromoteTo = move.s_PromotionType + BLACK;
	}
	else
	{
		PieceTypeToPromoteTo = move.s_PromotionType;
	}

	//castling
	if (BoardSquare[move.s_BoardSquare] == WHITE_KING or BoardSquare[move.s_BoardSquare] == BLACK_KING)
	{
		if (move.s_BoardSquare - move.s_Move == -2)
		{
			if (move.s_BoardSquare == 4)
			{
				BoardSquare[5] = WHITE_ROOK;
				BoardSquare[7] = 0;
			}
			else
			{
				BoardSquare[61] = BLACK_ROOK;
				BoardSquare[63] = 0;
			}
		}
		if (move.s_BoardSquare - move.s_Move == 2)
		{
			if (move.s_BoardSquare == 4)
			{
				BoardSquare[3] = WHITE_ROOK;
				BoardSquare[0] = 0;
			}
			else
			{
				BoardSquare[59] = BLACK_ROOK;
				BoardSquare[56] = 0;
			}
		}

	}

	//promoting and en passant
	if (PieceTypeToPromoteTo != 65)
	{
		BoardSquare[move.s_Move] = PieceTypeToPromoteTo;
	}
	else//optimization
	{
		//White en passant
		if (BoardSquare[move.s_BoardSquare] == WHITE_PAWN)
		{
			if (previousBoardSquare[move.s_Move] == 0)
			{
				if (move.s_BoardSquare - move.s_Move == -7 or move.s_BoardSquare - move.s_Move == -9)
				{
					BoardSquare[move.s_Move - 8] = 0;
				}
			}
		}
		//Black en passant
		if (BoardSquare[move.s_BoardSquare] == BLACK_PAWN)
		{
			if (previousBoardSquare[move.s_Move] == 0)
			{
				if (move.s_BoardSquare - move.s_Move == 7 or move.s_BoardSquare - move.s_Move == 9)
				{
					BoardSquare[move.s_Move + 8] = 0;
				}
			}
		}
	}

	BoardSquare[move.s_BoardSquare] = 0;

	/*not having this causes a bug where a pawn enpassanting does an illegal move
	GenerateLegalMoves::SetDoNotEnPassant(false);
	if (abs(move.s_BoardSquare - move.s_Move) == 16 and LegalMoves.WhichBoardSquaresAreAbsPinned[move.s_Move] != 65)
	{
		GenerateLegalMoves::SetDoNotEnPassant(true);
	}
	*/
}

std::array<uint8_t, 64> Board::PrevBoardSquareFromEP(const std::array<uint8_t, 64>& BoardSquare, uint8_t EPBoardsquare)
{
	ASSERT(!(EPBoardsquare == 65));
	std::array<uint8_t, 64> previousBoardSquare = BoardSquare;

	previousBoardSquare[EPBoardsquare] = 0;
	if (Board::IsPieceColorWhite(BoardSquare[EPBoardsquare]))
	{
		previousBoardSquare[EPBoardsquare - 8] = WHITE_PAWN;
	}
	else 
	{
		previousBoardSquare[EPBoardsquare + 8] = BLACK_PAWN;
	}

	return previousBoardSquare;
}

std::string Board::GetPrintableFromVecOfMoves(std::vector<Move> Moves)
{
	std::string print;

	for (Move MOVE : Moves)
	{
		print += Move2ALG(MOVE);
		print += ' ';
	}

	return print;
}

Move::Move()
{
}

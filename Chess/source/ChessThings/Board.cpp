#include "Board.h"
#include <iostream>

//vvv Board DEFINITIONS vvv//

Board::Board(const std::string& FenString)
	:BoardSquare(), FEN(FenString), IndexOfSideToMove(FEN.find(' ')),
	IndexOfCastling(FEN.find(' ', IndexOfSideToMove + 1)),
	IndexOfEnPassant(FEN.find(' ', IndexOfCastling + 1)),
	IndexOfHalfmoveClock(FEN.find(' ', IndexOfEnPassant + 1)),
	IndexOfFullMoveCounter(FEN.find(' ', IndexOfHalfmoveClock + 1))
{

}

//from fen to normal values funs
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
CastlingAbility Board::GetCanCastle()
{
	CastlingAbility CanCastle{};
	const std::string CastlingString = FEN.substr(++IndexOfCastling, IndexOfEnPassant - IndexOfCastling - 1);
	bool K = 0;
	bool k = 0;
	bool Q = 0;
	bool q = 0;
	for (char castleCharacter : CastlingString)
	{
		switch (castleCharacter)
		{
		case('K'):
			CanCastle.WhiteShort = true;
			break;
		case('k'):
			CanCastle.BlackShort = true;
			break;
		case('Q'):
			CanCastle.WhiteLong = true;
			break;
		case('q'):
			CanCastle.BlackLong = true;
			break;
		case('-'):
			break;
		default:
			std::cerr << "Castling for position/fen decoding is not standard. Wrong char\n";
			__debugbreak();
		}
	}

	return CanCastle;
}

//get the boardsquare where an e.p. is possible, NULL_OPTION if not
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
		ALG += PieceType2letter(move.s_PromotionType);//This is some traduction gibberish for UCI
	}

	return ALG;
}

uint8_t constexpr inline Board::GetPieceType2Uncolored(uint8_t PieceType)
{
	return PieceType & 0b00000111;
#ifdef _DEBUG
		ASSERT(false);
#endif
}


bool constexpr Board::IsPieceColorWhite(uint8_t BoardSquareValue)
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

void Board::WillCanCastleChange(const uint8_t BoardSquareNumItMovedFrom, CastlingAbility& Castle)
{
	switch (BoardSquareNumItMovedFrom)
	{
	case e1:
		Castle.WhiteLong = false;
		Castle.WhiteShort = false;
		break;
	case a1:
		Castle.WhiteLong = false;
		break;
	case h1:
		Castle.WhiteShort = false;
		break;
	case e8:
		Castle.BlackLong = false;
		Castle.BlackShort = false;
		break;
	case a8:
		Castle.BlackLong = false;
		break;
	case h8:
		Castle.BlackShort = false;
		break;
	}
	
}

bool Board::WillCanCastleChange(const uint8_t& PieceType, const uint8_t BoardSquareNumItMovedFrom, const uint8_t BoardSquareItMovedTo)
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

void Board::MakeMove(Move move, std::array<uint8_t, 64>& BoardSquare, uint8_t EnpassantIndex, CastlingAbility Castle)
{
	Board::WillCanCastleChange(move.s_BoardSquare, Castle);
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

void Board::MakeMove(Move move, std::array<uint8_t, 64>& BoardSquare, std::array<uint8_t, 64>& previousBoardSquare, CastlingAbility& Castle)
{
	previousBoardSquare = BoardSquare;
	Board::WillCanCastleChange(move.s_BoardSquare, Castle);
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

std::string Board::GetPrintableFromArrayOfMoves(const std::array<Move, MAX_PV_LENGTH>& Moves)
{
	std::string print;

	for (Move MOVE : Moves)
	{
		if (MOVE.IsNull()) { break; }
		print += Move2ALG(MOVE);
		print += ' ';
	}

	return print;
}

uint8_t Board::PieceType2Compact(uint8_t PieceType)
{
	if (PieceType < WHITE)
	{
		return PieceType - 9;
	}
	else if (PieceType < 23)
	{
		return PieceType - 11;
	}
	else
	{
		ASSERT(false);
	}
}

bit::EP constexpr Board::PrevPosition2EP(const std::array<uint8_t, 64>& BoardSquare, const std::array<uint8_t, 64>& PreviousBoardSquare, bool ZeroIfWhite)
{
	bit::BitPosition BitBoardSquare(BoardSquare);
	bit::BitPosition BitPrevBoardSquare(PreviousBoardSquare);

	auto Pawns = BitBoardSquare.ColorPositions[ZeroIfWhite] & BitBoardSquare.PiecePositions[PAWN - 1];
	auto PrevPawns = BitPrevBoardSquare.ColorPositions[ZeroIfWhite] & BitPrevBoardSquare.PiecePositions[PAWN - 1];

	auto StartRank = RANK_2 << (ZeroIfWhite * 40);
	auto DoubleForwardRank = RANK_4 << (ZeroIfWhite * 8);
	
	//TODO: maybe rewrite, think more about how we could know when there is and en passant file and relative bugs
	if (((Pawns.ReadBits() & StartRank) != (PrevPawns.ReadBits() & StartRank))
		and ((Pawns.ReadBits() & DoubleForwardRank) != (PrevPawns.ReadBits() & DoubleForwardRank)))
	{

	}
}

//^^^ Board DEFINITIONS ^^^//

Move::Move()
{
}

namespace bit {
	

	constexpr BitManager& BitManager::operator=(bool value)
	{
		if (value) {
			m_Data |= (1ULL << m_Index);
		}
		else {
			m_Data &= ~(1ULL << m_Index);
		}
		return *this;
	}

	BitPosManager& BitPosManager::operator=(uint8_t PieceType)
	{
		if (PieceType == 0)//remove piece from certain position
		{
			for (auto& a : m_Colors) { a[m_Index] = 0; }
			for (auto& a : m_Positions) { a[m_Index] = 0; }
			return *this;
		}

		bool IsWhite = !Board::IsPieceColorWhite(PieceType);

		m_Colors[IsWhite][m_Index] = true;//place piece in it's color
		m_Colors[!IsWhite][m_Index] = false;//remove the opposite color piece that might've been there

		for (auto& a : m_Positions)
		{
			a[m_Index] = false;//removes any piece that occupied the slot
		}

		m_Positions[Board::GetPieceType2Uncolored(PieceType)][m_Index] = true;

		return *this;
	}

	BitPosManager::operator uint8_t() const
	{
		uint8_t PieceType = NONE;
		uint8_t Color = NONE;

		if (m_Colors[0][m_Index]) { Color = WHITE; }
		else if (m_Colors[1][m_Index]) { Color = BLACK; }
		else { return 0; }//There is no piece at that index

		for (; PieceType <= 5; PieceType++)
		{
			if (m_Positions[PieceType][m_Index])
			{
				PieceType++;
				break;
			}
		}

		return Color | PieceType;
	}

	BitPosManager BitPosition::operator[](uint8_t Index)
	{
		if (Index > 63)
		{
			throw std::out_of_range("Indexed bit out of range");
		}
		return BitPosManager(PiecePositions, ColorPositions, Index);
	}

	uint8_t BitPosition::operator[](uint8_t BoardSquare) const
	{
		if (BoardSquare > 63)
		{
			throw std::out_of_range("Indexed bit out of range");
		}
		uint8_t PieceType = NONE;
		uint8_t color = 16;

		for (const BitBoard64& BitColor : ColorPositions)
		{
			if (BitColor[BoardSquare] == false) { color -= 8; continue; }
			else
			{
				uint8_t count = 0;
				for (const BitBoard64& BitPieceType : PiecePositions)
				{
					if (BitPieceType[BoardSquare] == false) { count++;  continue; }
					else
					{
						PieceType = color | (++count);
					}
				}
			}

		}

		return PieceType;
	}

	uint8_t BitPosition::popcnt() const noexcept
	{
		uint8_t popcount = 0;
		popcount += ColorPositions[0].popcnt();
		popcount += ColorPositions[1].popcnt();
		return popcount;
	}

	BitBoard64 BitPosition::find(uint8_t PieceType)
	{
		return (Board::IsPieceColorWhite(PieceType) ? ColorPositions[0] : ColorPositions[1]) & PiecePositions[Board::GetPieceType2Uncolored(PieceType) - 1];//peak unreadability
	}


	BitPosition::BitPosition(const std::array<uint8_t, 64>& OldBoardSquare)
	{
		for (uint8_t BoardSquarePos = 0; BoardSquarePos <= MAX_SQUARE; BoardSquarePos++)
		{
			uint8_t PieceType = OldBoardSquare[BoardSquarePos];
			if (PieceType == 0) { continue; }

			ColorPositions[!Board::IsPieceColorWhite(PieceType)][BoardSquarePos] = true;
			PiecePositions[Board::GetPieceType2Uncolored(PieceType) - 1][BoardSquarePos] = true;
		}
	}


}



#include "LegalMoves.h"

//static constexpr std::array<std::array<uint8_t, 8>, 64> CreateOffesetsForPawn()

static std::array<MOVE_BIT, 64> OppositeMoves;
static bool DoNotEnPassant; //for weird RemoveIllegalMoves things
//static constexpr std::array<bit::BitBoard64, 64> PawnTable{ CreateOffesetsForPawn() };


GenerateLegalMoves::GenerateLegalMoves(const bit::Position& Position, bool PseudoLegalFlag)
{
	ChessPosition = Position;

	isNextMoveForWhite = WHITE_TURN(Position.MoveNum);
	ZeroIfWhite = !isNextMoveForWhite;

	GenerateMoves();
	if (!PseudoLegalFlag)
	{
		RemoveIllegalMoves();
	}

}

GenerateLegalMoves::GenerateLegalMoves(const Position& Position, bool PseudoLegalFlag)
{
	//converts from array to bitboard
	ChessPosition = {Position.BoardSquare, Position.CanCastle, Position.MoveNum, Position.EnPassant };

	isNextMoveForWhite = WHITE_TURN(Position.MoveNum);
	ZeroIfWhite = !isNextMoveForWhite;

	GenerateMoves();
	if (!PseudoLegalFlag)
	{
		RemoveIllegalMoves();
	}

}


void GenerateLegalMoves::GenerateMoves()
{
	AttackedSquares.fill(false);
	SquaresBehindCheckedKing.fill(false);
	WhichBoardSquaresAreAbsPinned.fill(65);
	CheckTargetSquares.fill(65);
	uint8_t BoardSquarePos = 0;
	for (; BoardSquarePos <= MAX_SQUARE; BoardSquarePos++)
	{ 
		uint8_t i = ChessPosition.BoardSquare[BoardSquarePos];
		if (i == 0 or (Board::IsPieceColorWhite(i) != isNextMoveForWhite))[[likely]]
		{
			continue;
		}

		switch (i)
		{
		case WHITE_BISHOP:
		case WHITE_ROOK:
		case WHITE_QUEEN:
		case BLACK_BISHOP:
		case BLACK_ROOK:
		case BLACK_QUEEN:
			MagicSliderMoveGen(BoardSquarePos);
			break;
		case WHITE_PAWN:
		case BLACK_PAWN:
			PawnMoveGen(BoardSquarePos);
			break;
		case WHITE_KING:
		case BLACK_KING:
			KingMoveGen(BoardSquarePos);
			break;
		case WHITE_KNIGHT:
		case BLACK_KNIGHT:
			KnightMoveGen(BoardSquarePos);
			break;
		default:
			break;
		}
	}
}

void GenerateLegalMoves::MagicSliderMoveGen(const uint8_t BoardSquarePos)
{
	//NOTE: to simplify only the queen section is documented

	//vv Declare Frequent use variables vv
	uint8_t PieceTypeUncolored = Board::GetPieceType2Uncolored(ChessPosition.BoardSquare[BoardSquarePos]);
	uint8_t PieceType = ChessPosition.BoardSquare[BoardSquarePos];
	bool IsWhite = Board::IsPieceColorWhite(PieceType);
	uint8_t OpponentKingType = IsWhite ? BLACK_KING : WHITE_KING;
	uint8_t BishopShift = BISHOP_SHIFTS[BoardSquarePos];
	uint8_t RookShift = ROOK_SHIFTS[BoardSquarePos];

	bit::BitBoard64 blockers = ChessPosition.BoardSquare.ColorPositions[0] | ChessPosition.BoardSquare.ColorPositions[1];
	bit::BitBoard64 Attacks(0);

	switch (PieceTypeUncolored)
	{
	case(ROOK):
	{
		auto Key = mult_right_shift(blockers.ReadBits() & ROOK_MASKS[BoardSquarePos], ROOK_MAGICS[BoardSquarePos], RookShift);

		Attacks = ROOK_ATTACKS[ROOK_SQUARE_TO_ATTACKS_INDEX[BoardSquarePos] + Key];

		AttackedSquares |= Attacks;

		Attacks ^= (Attacks & ChessPosition.BoardSquare.ColorPositions[!IsWhite]);

		break;
	}
	case (BISHOP):
	{
		uint16_t BishopAttackIndex = mult_right_shift(blockers.ReadBits() & BISHOP_MASKS[BoardSquarePos], BISHOP_MAGICS[BoardSquarePos], BishopShift);

		Attacks = BISHOP_ATTACKS[BISHOP_SQUARE_TO_ATTACKS_INDEX[BoardSquarePos] + BishopAttackIndex];

		AttackedSquares |= Attacks;

		Attacks ^= (Attacks & ChessPosition.BoardSquare.ColorPositions[!IsWhite]);

		break;
	}
	case(QUEEN):
	{

		//Get Indeces from the mult-right shift
		uint16_t RookAttackIndex = mult_right_shift(blockers.ReadBits() & ROOK_MASKS[BoardSquarePos], ROOK_MAGICS[BoardSquarePos], RookShift);
		uint16_t BishopAttackIndex = mult_right_shift(blockers.ReadBits() & BISHOP_MASKS[BoardSquarePos], BISHOP_MAGICS[BoardSquarePos], BishopShift);

		bit::BitBoard64 RookAttack = ROOK_ATTACKS[ROOK_SQUARE_TO_ATTACKS_INDEX[BoardSquarePos] + RookAttackIndex];
		bit::BitBoard64 BishopAttack = BISHOP_ATTACKS[BISHOP_SQUARE_TO_ATTACKS_INDEX[BoardSquarePos] + BishopAttackIndex];

		AttackedSquares |= (RookAttack | BishopAttack);

		//removes attacks to own pieces
		RookAttack ^= RookAttack & ChessPosition.BoardSquare.ColorPositions[!IsWhite];
		BishopAttack ^= BishopAttack & ChessPosition.BoardSquare.ColorPositions[!IsWhite];

		Attacks = RookAttack | BishopAttack;

		break;
	}
	default:
		__debugbreak();
		break;
	}

	//found check
	if ((Attacks & ChessPosition.BoardSquare.find(OpponentKingType)) != 0)
	{
		//These are here to not have nested control flow
		uint8_t MaxDir;
		uint8_t Direction = 0;
		switch (PieceTypeUncolored)
		{
		case(ROOK):
			MaxDir = 4;
			break;
		case(BISHOP):
			MaxDir = 8;
			Direction = 4;
			break;
		case(QUEEN):
			MaxDir = 8;
			break;
		}

		std::array<uint8_t, 7> GuessXrayCheck;//fill this with board squares of xray attack, if no king is found, revert it
		GuessXrayCheck.fill(65);
		//Search for the xray attack that leads to check, add it to CheckTargetSquares (also do SquaresBehindCheckedKing)
		for (; Direction < MaxDir; Direction++)
		{
			for (uint8_t Scalar = 1; Scalar <= NumOfSquaresUntilEdge[BoardSquarePos][Direction]; Scalar++)
			{
				GuessXrayCheck[Scalar - 1] = BoardSquarePos + (Scalar * OffsetForDirections[Direction]);
				if (ChessPosition.BoardSquare[BoardSquarePos + (Scalar * OffsetForDirections[Direction])] == OpponentKingType)
				{
					Scalar += 1;
					//finish the loop, fill SquaresBehindCheckedKing
					for (; Scalar <= NumOfSquaresUntilEdge[BoardSquarePos][Direction]; Scalar++)
					{
						SquaresBehindCheckedKing[BoardSquarePos + (Scalar * OffsetForDirections[Direction])] = true;
					}
					goto OutsideCheckLoop;//this will skip GuessXrayCheck reversal
				}
			}
			GuessXrayCheck.fill(65);
		}

	OutsideCheckLoop://fill CheckTargetSquares from GuessXrayCheck
		for (auto& CheckBoardSquare : GuessXrayCheck)
		{
			if (CheckBoardSquare == 65) { break; }
			if (CheckTargetSquares[CheckBoardSquare] == NULL_OPTION) [[likely]]
			{
				CheckTargetSquares[CheckBoardSquare] = BoardSquarePos;
			}
			else
			{
				DoubleCheckBoardSquare = BoardSquarePos;
			}

		}
	}
	//possible pin, logic for finding and saving is the same as above, DO NOT LET CHECKS IN HERE
	else if (((ROOK_LEGAL_MASKS[BoardSquarePos] | BISHOP_LEGAL_MASKS[BoardSquarePos]) & ChessPosition.BoardSquare.find(OpponentKingType)) != 0)
	{
		//These are here to not have nested control flow
		uint8_t MaxDir;
		uint8_t Direction = 0;
		uint8_t PiecesEncoutered = 0; //keeps count of how many non-OppositeKing pieces we scanned, if > 1 stop direction and reset
		switch (PieceTypeUncolored)
		{
		case(ROOK):
			MaxDir = 4;
			break;
		case(BISHOP):
			MaxDir = 8;
			Direction = 4;
			break;
		case(QUEEN):
			MaxDir = 8;
			break;
		}

		std::array<uint8_t, 7> GuessPinBoardSquares;
		GuessPinBoardSquares.fill(65);
		for (; Direction < MaxDir; Direction++)
		{
			for (uint8_t Scalar = 1; Scalar <= NumOfSquaresUntilEdge[BoardSquarePos][Direction]; Scalar++)
			{
				GuessPinBoardSquares[Scalar - 1] = BoardSquarePos + (Scalar * OffsetForDirections[Direction]);

				if (PiecesEncoutered > 1)
				{
					break;
				}

				if (ChessPosition.BoardSquare[BoardSquarePos + (Scalar * OffsetForDirections[Direction])] == OpponentKingType)
				{
					goto OutsidePinLoop;
				}
				else if (ChessPosition.BoardSquare[BoardSquarePos + (Scalar * OffsetForDirections[Direction])] != 0)
				{
					PiecesEncoutered++;
				}
			}
			PiecesEncoutered = 0;
			GuessPinBoardSquares.fill(65);
		}

	OutsidePinLoop:
		for (auto& PinSquare : GuessPinBoardSquares)
		{
			if (PinSquare == 65) { break; }
			WhichBoardSquaresAreAbsPinned[PinSquare] = BoardSquarePos;
		}
	}

	moves[BoardSquarePos].TargetSquares = Attacks;
	moves[BoardSquarePos].PieceType = PieceType;
}

//Knight
void GenerateLegalMoves::KnightMoveGen(const uint8_t BoardSquarePos)
{
	//avoiding branches by using ZeroIfWhite as index
	auto Attacks = KnightTable[BoardSquarePos];

	AttackedSquares |= Attacks;

	Attacks ^= (Attacks & ChessPosition.BoardSquare.ColorPositions[ZeroIfWhite]);

	moves[BoardSquarePos].PieceType = ChessPosition.BoardSquare[BoardSquarePos];
	moves[BoardSquarePos].TargetSquares = Attacks;

	//Because we know we are only attacking enemy pieces we don't need to make extra operations
	if ((Attacks & ChessPosition.BoardSquare.PiecePositions[KING - 1]) != 0)
	{
		if (CheckTargetSquares[std::countr_zero(ChessPosition.BoardSquare.PiecePositions[KING - 1].ReadBits())] != 65)
			DoubleCheckBoardSquare = BoardSquarePos;
		else
			CheckTargetSquares[std::countr_zero(ChessPosition.BoardSquare.PiecePositions[KING - 1].ReadBits())] = BoardSquarePos;
	}
}

//PAWN
void GenerateLegalMoves::PawnMoveGen(const uint8_t BoardSquarePos)
{
#ifdef _DEBUG
	if (BoardSquarePos < 8 or BoardSquarePos > 55)
	{
		ASSERT(false);
	}
#endif
	uint8_t PieceType = ChessPosition.BoardSquare[BoardSquarePos];

	moves[BoardSquarePos].PieceType = PieceType;

	AttackedSquares = AttackedSquares | PAWN_CAPTURES[ZeroIfWhite][BoardSquarePos];

	if(((isNextMoveForWhite and BoardSquarePos >= a2 and BoardSquarePos <= h2) or (!isNextMoveForWhite and BoardSquarePos >= a7 and BoardSquarePos <= h7))
		and (ChessPosition.BoardSquare[BoardSquarePos + ForwardAttacks[ZeroIfWhite]] == 0))
	{
		moves[BoardSquarePos].TargetSquares[BoardSquarePos + ForwardAttacks[ZeroIfWhite]] = true;
		moves[BoardSquarePos].TargetSquares[BoardSquarePos + (ForwardAttacks[ZeroIfWhite] * 2)] = true;//we are in the 2nd or the 7th, so we are able to do double push
	}
	else
	{
		moves[BoardSquarePos].TargetSquares[BoardSquarePos + ForwardAttacks[ZeroIfWhite]] = true;
	}

	moves[BoardSquarePos].TargetSquares = moves[BoardSquarePos].TargetSquares & ~(ChessPosition.BoardSquare.ColorPositions[0] | ChessPosition.BoardSquare.ColorPositions[1]);

	bit::BitBoard64 Capturable = ChessPosition.BoardSquare.ColorPositions[isNextMoveForWhite];

	moves[BoardSquarePos].TargetSquares = moves[BoardSquarePos].TargetSquares | (PAWN_CAPTURES[ZeroIfWhite][BoardSquarePos] & Capturable);

	if (BoardSquarePos >= a7 and isNextMoveForWhite)
	{
		moves[BoardSquarePos].Promotion.SetPromotionByBits(moves[BoardSquarePos].TargetSquares.ReadBits() >> std::countr_zero(moves[BoardSquarePos].TargetSquares.ReadBits()), BoardSquarePos);//we slam the pawn attacks at the bottom
	}
	else if (BoardSquarePos <= h2 and !isNextMoveForWhite)
	{
		moves[BoardSquarePos].Promotion.SetPromotionByBits(moves[BoardSquarePos].TargetSquares.ReadBits() >> std::countr_zero(moves[BoardSquarePos].TargetSquares.ReadBits()), BoardSquarePos);
	}

	if ((BoardSquarePos <= 39 and BoardSquarePos >= 32) or (BoardSquarePos <= 31 and BoardSquarePos >= 24))//en passant
	{
		if (ChessPosition.EnPassant.ReadEp((BoardSquarePos % 8) - 1)) //left check
		{
			moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + (isNextMoveForWhite ? 7 : -7));
			EnPassantFiles[(BoardSquarePos % 8) - 1] = true;
		}
		else if (ChessPosition.EnPassant.ReadEp((BoardSquarePos % 8) + 1)) //right check
		{
			moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + (isNextMoveForWhite ? 9 : -9));
			EnPassantFiles[(BoardSquarePos % 8) + 1] = true;
		}
	}

	//this is the bitboard of the king's position, if under attack
	auto KingSquare = moves[BoardSquarePos].TargetSquares.ReadBits()
		& ChessPosition.BoardSquare.ColorPositions[isNextMoveForWhite] /*isNextMoveForWhite is the negation of zero if true*/
		& ChessPosition.BoardSquare.PiecePositions[5];
	//set Check
	if (KingSquare != 0)
	{
		if (CheckTargetSquares[std::countr_zero(KingSquare)] != 65)
			DoubleCheckBoardSquare = BoardSquarePos;
		else
			CheckTargetSquares[std::countr_zero(KingSquare)] = BoardSquarePos;
	}
	
}

//King
void GenerateLegalMoves::KingMoveGen(const uint8_t BoardSquarePos)
{
	uint8_t PieceType = ChessPosition.BoardSquare[BoardSquarePos];

	moves[BoardSquarePos].PieceType = PieceType;

	moves[BoardSquarePos].TargetSquares = KING_ATTACKS[BoardSquarePos] & ~ChessPosition.BoardSquare.ColorPositions[ZeroIfWhite];

	AttackedSquares |= KING_ATTACKS[BoardSquarePos];

	if (PieceType == WHITE_KING)//castling
	{
		if (ChessPosition.CanCastle.WhiteLong and ChessPosition.BoardSquare[1] == 0 and ChessPosition.BoardSquare[2] == 0 and ChessPosition.BoardSquare[3] == 0)
		{
			moves[4].TargetSquares.push_back(2);
		}
		if (ChessPosition.CanCastle.WhiteShort and ChessPosition.BoardSquare[5] == 0 and ChessPosition.BoardSquare[6] == 0)
		{
			moves[4].TargetSquares.push_back(6);
		}
	}
	else//castling
	{
		if (ChessPosition.CanCastle.BlackLong and ChessPosition.BoardSquare[57] == 0 and ChessPosition.BoardSquare[58] == 0 and ChessPosition.BoardSquare[59] == 0)
		{
			moves[60].TargetSquares.push_back(58);
		}
		if (ChessPosition.CanCastle.BlackShort and ChessPosition.BoardSquare[61] == 0 and ChessPosition.BoardSquare[62] == 0)
		{
			moves[60].TargetSquares.push_back(62);
		}
	}
}

void GenerateLegalMoves::RemoveIllegalMoves()
{
	uint8_t BoardSquareOfKingToMove = 0;

	//find out where the King is
	bit::FindBit(ChessPosition.BoardSquare.find(isNextMoveForWhite ? WHITE_KING : BLACK_KING), BoardSquareOfKingToMove);

	ChessPosition.MoveNum++;
	GenerateLegalMoves OppositeMoves(ChessPosition, true);
	ChessPosition.MoveNum--;

	OppositeAttackedSquares = OppositeMoves.AttackedSquares;

	//fill IndexOfPieceChecking
	uint8_t count = 0;
	uint8_t CheckingSquare = NULL_OPTION;
	uint8_t CheckingSquare2 = NULL_OPTION;
	size_t NumberOfChecks = 0;
 	for (const uint8_t& Square : OppositeMoves.CheckTargetSquares)
	{
		if (Square != NULL_OPTION)
		{
			if (CheckingSquare == NULL_OPTION)
			{
				CheckingSquare = Square;
				NumberOfChecks++;
			}
			else if (CheckingSquare != Square)
			{
				CheckingSquare2 = Square;
				NumberOfChecks++;
			}
		}
	}
	if (NumberOfChecks == 1 and OppositeMoves.DoubleCheckBoardSquare != NULL_OPTION)//makes sure that double checks are not ignored
	{
		NumberOfChecks++;
	}


	//moves for King
	if (NumberOfChecks >= 0)
	{
		//when double checked only moves made by king can be legal, so clear the others
		if (NumberOfChecks == 2)
		{
			for (MOVE_BIT& Piece : moves)
			{
				if (Piece.PieceType != ChessPosition.BoardSquare[BoardSquareOfKingToMove])
				{
					Piece.TargetSquares.clear();
				}
			}
		}

		auto MovesCopy = moves[BoardSquareOfKingToMove].TargetSquares;
		uint8_t KingMove = 0;
		while (true)
		{
			if (!bit::pop_lsb(MovesCopy, KingMove)) { break; }

			//checks if possible move square is under attack
			if (OppositeMoves.AttackedSquares[KingMove] == true)
			{
				moves[BoardSquareOfKingToMove].TargetSquares[KingMove] = false;
				continue;
			}
			//checks if possible move is going to be under attack when it's made
			else if (OppositeMoves.SquaresBehindCheckedKing[KingMove] == true)
			{
				moves[BoardSquareOfKingToMove].TargetSquares[KingMove] = false;
				continue;
			}
			//checks that it can castle
			else if ((BoardSquareOfKingToMove - KingMove == 2) or (BoardSquareOfKingToMove - KingMove == -2))
			{
				CanKingCastle_LMoves(OppositeMoves, isItCheckmate, BoardSquareOfKingToMove, KingMove);
			}
			else
			{
				isItCheckmate = false;
			}
		}
	}

	if (NumberOfChecks < 2)
	{
		count = 0;
		for (MOVE_BIT& Piece : moves)
		{//this Piece.PieceType == 0 might be risky, but only if piece has moves and Piecetype = 0 is either no piece or no moves for the piece
			if (Piece.PieceType == WHITE_KING or Piece.PieceType == BLACK_KING or Piece.TargetSquares.popcnt() == 0)
			{
				count++;
				continue;
			}

			//if in check and count is under abs pin, unable to move
			if (OppositeMoves.WhichBoardSquaresAreAbsPinned[count] != 65 and NumberOfChecks == 1)
			{
				Piece.TargetSquares.clear();
				count++;
				continue;
			}
			else
			{
				auto MovesCopy = Piece.TargetSquares;
				uint8_t Move = 0;
				while (true)
				{
					if (!bit::pop_lsb(MovesCopy, Move)) { break; }

					//for those things that happen with pawns
					if (Piece.PieceType == BLACK_PAWN or Piece.PieceType == WHITE_PAWN)
					{
						if (DoNotEnPassant and ChessPosition.BoardSquare[Move] == 0 and (abs(count - Move) == NW or abs(count - Move) == NE))
						{
							Piece.TargetSquares[Move] = false;
							continue;
						}
					}

					if ((OppositeMoves.WhichBoardSquaresAreAbsPinned[count] != 65 and NumberOfChecks == 0) or NumberOfChecks == 1)
					{
						//if move is an en passant add +-8 to move
						if ((Piece.PieceType == BLACK_PAWN or Piece.PieceType == WHITE_PAWN) and (abs(count - Move) == NW or abs(count - Move) == NE) and ChessPosition.BoardSquare[Move] == 0)
						{
							Piece.PieceType == WHITE_PAWN ? Move -= 8 : Move += 8;
						}

						if (NumberOfChecks == 1)
						{
							//checks if Move neither captures piece nor doesn't block check
							if (Move != CheckingSquare and OppositeMoves.CheckTargetSquares[Move] != CheckingSquare)
							{
								Piece.TargetSquares[Move] = false;
								continue;
							}
							else
							{
								isItCheckmate = false;
							}
						}
						else
						{	//checks if move causes an illegal check
							if (OppositeMoves.WhichBoardSquaresAreAbsPinned[count] != OppositeMoves.WhichBoardSquaresAreAbsPinned[Move] and OppositeMoves.WhichBoardSquaresAreAbsPinned[count] != Move)
							{
								Piece.TargetSquares[Move] = false;
								continue;
							}
							else
							{
								isItCheckmate = false;
							}
						}

					}
				}
			}

			count++;
		}
	}

	for (MOVE_BIT& Piece : moves)
	{
		if (Piece.TargetSquares == 0) { continue; }
		if ((Piece.PieceType == WHITE_PAWN or Piece.PieceType == BLACK_PAWN) and ((Piece.Promotion.Promotion & PromotionMask) != 0)) [[unlikely]]
		{
			m_NumOfLegalMoves += std::popcount(static_cast<uint64_t>(Piece.TargetSquares)) * 4;
		}
		else
		{
			m_NumOfLegalMoves += Piece.TargetSquares.popcnt();
		}
	}

	if (isItCheckmate == true and NumberOfChecks == 0)//stalemate
	{
		isItCheckmate = false;
	}
}

//fun for LegalMoves, checks if castling is permitted
void GenerateLegalMoves::CanKingCastle_LMoves(const GenerateLegalMoves& OppositeMoves, bool& isItCheckmate, const uint8_t& BoardSquareOfKingToMove, const uint8_t KingMove)
{
	if (Board::IsPieceColorWhite(ChessPosition.BoardSquare[BoardSquareOfKingToMove]))
	{
		if (BoardSquareOfKingToMove - KingMove == 2)
		{
			if ((OppositeMoves.AttackedSquares[4] != false) or (OppositeMoves.AttackedSquares[3] != false) or (OppositeMoves.AttackedSquares[2] != false))
			{
				moves[BoardSquareOfKingToMove].TargetSquares[KingMove] = false;
			}
			else
			{
				isItCheckmate = false;
			}
		}
		else if (BoardSquareOfKingToMove - KingMove == -2)
		{
			if ((OppositeMoves.AttackedSquares[4] != false) or (OppositeMoves.AttackedSquares[5] != false) or (OppositeMoves.AttackedSquares[6] != false))
			{
				moves[BoardSquareOfKingToMove].TargetSquares[KingMove] = false;
			}
			else
			{
				isItCheckmate = false;
			}
		}
	}
	else
	{
		if (BoardSquareOfKingToMove - KingMove == 2)
		{
			if ((OppositeMoves.AttackedSquares[58] != false) or (OppositeMoves.AttackedSquares[59] != false) or (OppositeMoves.AttackedSquares[60] != false))
			{
				moves[BoardSquareOfKingToMove].TargetSquares[KingMove] = false;
			}
			else
			{
				isItCheckmate = false;
			}
		}
		else if (BoardSquareOfKingToMove - KingMove == -2)
		{
			if ((OppositeMoves.AttackedSquares[60] != false) or (OppositeMoves.AttackedSquares[61] != false) or (OppositeMoves.AttackedSquares[62] != false))
			{
				moves[BoardSquareOfKingToMove].TargetSquares[KingMove] = false;
			}
			else
			{
				isItCheckmate = false;
			}
		}
	}
}

void GenerateLegalMoves::SetDoNotEnPassant(bool SetToThis)
{
	DoNotEnPassant = SetToThis;
}

bool GenerateLegalMoves::IsMoveLegal(const Move& CheckedMove) const
{
	if (moves[CheckedMove.s_BoardSquare].TargetSquares[CheckedMove.s_Move] == true and (moves[CheckedMove.s_BoardSquare].Promotion.Promotion == 0 and (CheckedMove.s_PromotionType != 65 and moves[CheckedMove.s_BoardSquare].Promotion.Promotion != 0) or (CheckedMove.s_PromotionType == 65 and moves[CheckedMove.s_BoardSquare].Promotion.Promotion == 0)))
	{
		return true;
	}
	return false;
}

uint32_t GenerateLegalMoves::GetNumOfTacticalMoves() const
{
	uint32_t NumOfTacticalMoves = 0;
	for (const MOVE_BIT& Piece : moves)
	{
		auto MovesCopy = Piece.TargetSquares;
		uint8_t Move = 0;
		while (true)
		{
			if (!bit::pop_lsb(MovesCopy, Move)) { break; }

			if ((Piece.TargetSquares[Move] and ChessPosition.BoardSquare[Move] != 0) or ((Piece.Promotion.Promotion & PromotionMask) != 0))
				NumOfTacticalMoves++;
		}
	}
	return NumOfTacticalMoves;
}


//^^^^^^ GenerateLegalMoves ^^^^^^^

[[nodiscard]] bit::BitBoard64 AttacksTo(const bit::Position& ChessPosition, uint8_t SquarePos)
{
	auto IsWhite = WHITE_TURN(ChessPosition.MoveNum);

	bit::BitBoard64 Attackers;

	//In this section we "fake" being a queen, so we check who is attacking us by "attacking them"
	uint64_t Blockers = (ChessPosition.BoardSquare.ColorPositions[0] | ChessPosition.BoardSquare.ColorPositions[1]) & (ROOK_MASKS[SquarePos] | BISHOP_MASKS[SquarePos]);
	auto RookKey = mult_right_shift(Blockers, ROOK_MAGICS[SquarePos], ROOK_SHIFTS[SquarePos]);
	auto BishopKey = mult_right_shift(Blockers, BISHOP_MAGICS[SquarePos], BISHOP_SHIFTS[SquarePos]);

	bit::BitBoard64 RookAttack = ROOK_ATTACKS[ROOK_SQUARE_TO_ATTACKS_INDEX[SquarePos] + RookKey];
	bit::BitBoard64 BishopAttack = BISHOP_ATTACKS[BISHOP_SQUARE_TO_ATTACKS_INDEX[SquarePos] + BishopKey];

	Attackers = RookAttack | BishopAttack;

	//In this section we fake being a knight
	Attackers |= KnightTable[SquarePos];

	//In this section we fake being a pawn



	Attackers &= ChessPosition.BoardSquare.ColorPositions[IsWhite];//Remove anything that isn't an opposite piece

	return Attackers;
}

void PromotionByte::SetPromotionSide(uint8_t Side, uint8_t BoardSquare)
{
	switch (Side)
	{
	case 0:
		Promotion |= LeftPromotionMask;
		break;
	case 1:
		Promotion |= CenterPromotionMask;
		break;
	case 2:
		Promotion |= RightPromotionMask;
		break;
	default:
		ASSERT(false);
	}
	Promotion |= BoardSquare << 3;
}

void PromotionByte::ResetPromotionSide(uint8_t Side, uint8_t BoardSquare)
{
	switch (Side)
	{
	case 0:
		Promotion |= LeftPromotionMask;
		Promotion ^= LeftPromotionMask;
		break;
	case 1:
		Promotion |= CenterPromotionMask;
		Promotion ^= CenterPromotionMask;
		break;
	case 2:
		Promotion |= RightPromotionMask;
		Promotion ^= RightPromotionMask;
		break;
	default:
		ASSERT(false);
	}
	Promotion |= BoardSquare << 3;

}

void PromotionByte::SetPromotionByBits(uint8_t ThreeBits, uint8_t BoardSquare)
{
	Promotion |= BoardSquare << 3;
#ifdef _DEBUG
	ASSERT(ThreeBits < 8)
#endif
	Promotion |= ThreeBits;
}

#ifndef ROOKS_HAVE_BEEN_PRECOMPUTED
#ifndef BISHOPS_HAVE_BEEN_PRECOMPUTED
uint64_t ComputeRookAttacks(uint8_t BoardSquare, uint16_t Blocker)
{
	uint64_t Attacks = 0;
	uint64_t BlockerBitboard = expand_bits_to_mask(Blocker, ROOK_MASKS[BoardSquare]);//expand pattern to Bitboard
	for (uint8_t Direction = 0; Direction < 4; Direction++)//for every direction
	{
		for (uint8_t Scalar = 1; Scalar <= NumOfSquaresUntilEdge[BoardSquare][Direction]; Scalar++)//for every bsquare along the direction
		{
			uint64_t Bit_PositionBeingChecked = (1ULL << (BoardSquare + (Scalar * OffsetForDirections[Direction])));
			Attacks |= Bit_PositionBeingChecked;
			if ((BlockerBitboard & Bit_PositionBeingChecked) == Bit_PositionBeingChecked)
			{
				break;
			}
		}
	}
	return Attacks;
}

uint64_t ComputeBishopAttacks(uint8_t BoardSquare, uint16_t Blocker)
{
	uint64_t Attacks = 0;
	uint64_t BlockerBitboard = expand_bits_to_mask(Blocker, BISHOP_MASKS[BoardSquare]);
	for (uint8_t Direction = 4; Direction < 8; Direction++)
	{
		for (uint8_t Scalar = 1; Scalar <= NumOfSquaresUntilEdge[BoardSquare][Direction]; Scalar++)
		{
			uint64_t Bit_PositionBeingChecked = (1ULL << (BoardSquare + (Scalar * OffsetForDirections[Direction])));
			Attacks |= Bit_PositionBeingChecked;
			if ((BlockerBitboard & Bit_PositionBeingChecked) == Bit_PositionBeingChecked)
			{
				break;
			}
		}
	}
	return Attacks;
}

void MagicRookFinder(uint8_t BoardSquare, std::vector<uint64_t>& RookAttacks)
{
	ROOK_SQUARE_TO_ATTACKS_INDEX[BoardSquare] = RookAttacks.size();

	uint64_t MagicNum;
	bool Failed = false;

	uint8_t shift = ROOK_SHIFTS[BoardSquare];
	std::array<uint16_t, 4096> SeenKeys{ UINT16_MAX };

	for (uint64_t Try = 0; Try < 1000000; Try++)
	{
		MagicNum = Random64Bit() & Random64Bit() & Random64Bit();
		SeenKeys.fill(UINT16_MAX);
		Failed = false;
		for (uint16_t Blocker = 0; Blocker < (1ULL << shift); Blocker++)
		{
			auto Key = mult_right_shift(expand_bits_to_mask(Blocker, ROOK_MASKS[BoardSquare]), MagicNum, shift);
			if (SeenKeys[Key] != UINT16_MAX)//collision
			{
				Failed = true;
				break;
			}
			else
			{
				SeenKeys[Key] = Blocker;
			}
		}

		if (Failed == false)//found magic num
		{
			std::vector<uint64_t> RookAttacksSquare;
			RookAttacksSquare.resize(1ULL << shift);

			for (uint16_t Blocker = 0; Blocker < (1ULL << shift); Blocker++)
			{
				auto Key = mult_right_shift(expand_bits_to_mask(Blocker, ROOK_MASKS[BoardSquare]), MagicNum, shift);
				RookAttacksSquare[Key] = ComputeRookAttacks(BoardSquare, Blocker) & ROOK_LEGAL_MASKS[BoardSquare];
			}

			PushBackVec(RookAttacks, RookAttacksSquare);

			ROOK_MAGICS[BoardSquare] = MagicNum;//store magic num
			break;
		}
	}

}

void MagicBishopFinder(uint8_t BoardSquare, std::vector<uint64_t>& BishopAttacks)
{
	BISHOP_SQUARE_TO_ATTACKS_INDEX[BoardSquare] = BishopAttacks.size();

	uint64_t MagicNum;
	bool Failed = false;

	std::array<uint16_t, 512> SeenKeys{ UINT64_MAX };
	uint8_t shift = BISHOP_SHIFTS[BoardSquare];

	for (uint64_t Try = 0; Try < UINT64_MAX; Try++)
	{
		MagicNum = Random64Bit() & Random64Bit() & Random64Bit();
		SeenKeys.fill(UINT16_MAX);
		Failed = false;
		for (uint16_t Blocker = 0; Blocker < (1ULL << shift); Blocker++)
		{
			auto Key = mult_right_shift(expand_bits_to_mask(Blocker, BISHOP_MASKS[BoardSquare]), MagicNum, shift);
			if (SeenKeys[Key] != UINT16_MAX)//collision
			{
				Failed = true;
				break;
			}
			else
			{
				SeenKeys[Key] = Blocker;
			}
		}
		if (Failed == false)//found magic num
		{
			std::vector<uint64_t> BishopAttacksSquare;
			BishopAttacksSquare.resize(1ULL << shift);

			for (uint16_t Blocker = 0; Blocker < (1ULL << shift); Blocker++)
			{
				auto Key = mult_right_shift(expand_bits_to_mask(Blocker, BISHOP_MASKS[BoardSquare]), MagicNum, shift);
				BishopAttacksSquare[Key] = ComputeBishopAttacks(BoardSquare, Blocker) & BISHOP_LEGAL_MASKS[BoardSquare];
			}

			PushBackVec(BishopAttacks, BishopAttacksSquare);
			BISHOP_MAGICS[BoardSquare] = MagicNum;
			break;
		}
	}

}

void ComputeHeavy()
{
	static bool AlreadyRan = false;
	if (AlreadyRan) { return; }

	TCHAR exePath[MAX_PATH];
	GetModuleFileName(NULL, exePath, MAX_PATH);
	fs::path ExePath = fs::canonical(exePath);
	fs::path PrecomputesPath = ExePath;
	PrecomputesPath = PrecomputesPath.parent_path();
	//searches and sets the Precompute dir path to PrecomputesPath
	while (fs::exists(PrecomputesPath) and PrecomputesPath != PrecomputesPath.root_path())
	{
		for (const auto& DirEntry : fs::directory_iterator{ PrecomputesPath })
		{
			if (DirEntry.path().filename() == "Precomputes")
			{
				PrecomputesPath = DirEntry;
			}
		}
		if (PrecomputesPath.filename() == "Precomputes")
		{
			break;
		}
		PrecomputesPath = PrecomputesPath.parent_path();
	}
	
	std::fstream RookPrecomputes;
	if (!fs::exists(PrecomputesPath / "Rook_Attacks.bin") or !fs::exists(PrecomputesPath / "Rook_Magics.bin"))//if ROOK_ATTACKS or magics wasn't created
	{
		std::vector<uint64_t> RookAttacks;
		RookAttacks.reserve(1000000);
		for (uint8_t BoardSquare = 0; BoardSquare <= MAX_SQUARE; BoardSquare++)
		{
			MagicRookFinder(BoardSquare, RookAttacks);//compute magics and attacks
			std::cout << "Found Bsquare: " << toascii(BoardSquare) << " with magic " << ROOK_MAGICS[BoardSquare] << std::endl;
		}
		for (uint8_t BoardSquare = 0; BoardSquare <= MAX_SQUARE; BoardSquare++)
		{
			std::cout << ROOK_SQUARE_TO_ATTACKS_INDEX[BoardSquare] << '\n';
		}
		RookPrecomputes.open(PrecomputesPath / "Rook_Attacks.bin", std::ios::out | std::ios::binary);
		RookPrecomputes.write(reinterpret_cast<const char*>(RookAttacks.data()), (sizeof(uint64_t) * RookAttacks.size()));//save the attacks to the binary file
		RookPrecomputes.close();
		RookPrecomputes.open(PrecomputesPath / "Rook_Magics.bin", std::ios::out | std::ios::binary);
		RookPrecomputes.write(reinterpret_cast<const char*>(ROOK_MAGICS.data()), sizeof(ROOK_MAGICS));//save the magics to the binary file
	}

	
	std::fstream BishopPrecomputes;
	if (!fs::exists(PrecomputesPath / "Bishop_Attacks.bin") or !fs::exists(PrecomputesPath / "Bishop_Magics.bin"))//if BISHOP_ATTACKS or magics wasn't created
	{
		std::vector<uint64_t> BishopAttacks;
		BishopAttacks.reserve(10000);
		for (uint8_t BoardSquare = 0; BoardSquare <= MAX_SQUARE; BoardSquare++)
		{
			MagicBishopFinder(BoardSquare, BishopAttacks);//compute magics and attacks
			std::cout << "Found Bsquare: " << toascii(BoardSquare) << " with magic " << BISHOP_MAGICS[BoardSquare] << std::endl;
		}
		for (uint8_t BoardSquare = 0; BoardSquare <= MAX_SQUARE; BoardSquare++)
		{
			std::cout << BISHOP_SQUARE_TO_ATTACKS_INDEX[BoardSquare] << '\n';
		}
		BishopPrecomputes.open(PrecomputesPath / "Bishop_Attacks.bin", std::ios::out | std::ios::binary);
		BishopPrecomputes.write(reinterpret_cast<const char*>(BishopAttacks.data()), (sizeof(uint64_t) * BishopAttacks.size()));//save the table to the binary file
		BishopPrecomputes.close();
		BishopPrecomputes.open(PrecomputesPath / "Bishop_Magics.bin", std::ios::out | std::ios::binary);
		BishopPrecomputes.write(reinterpret_cast<const char*>(BISHOP_MAGICS.data()), sizeof(BISHOP_MAGICS));//save the magics to the binary file
	}

	AlreadyRan = true;

}
#endif
#endif

inline uint16_t mult_right_shift(uint64_t BlockerBits, uint64_t Magic, uint8_t RelevantBitNum)
{
	return (BlockerBits * Magic) >> (64 - RelevantBitNum);
}
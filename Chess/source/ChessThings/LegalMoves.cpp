#include "LegalMoves.h"
static bit::BitPosition m_previousBoardSquare;
static std::array<MOVE_BIT, 64> OppositeMoves;
static bool DoNotEnPassant; //for weird RemoveIllegalMoves things


GenerateLegalMoves::GenerateLegalMoves(const bit::BitPosition& BoardSquare, const bit::BitPosition* previousBoardSquare, canCastle CanCastle, bool isNextMoveForWhite, unsigned int MoveNum, bool isForOppositeMoves)
	:m_BoardSquare(BoardSquare), CanCastle(CanCastle), MoveNum(MoveNum), isNextMoveForWhite(isNextMoveForWhite)
{

	if (previousBoardSquare != nullptr)
		m_previousBoardSquare = *previousBoardSquare;


	GenerateMoves();
	if (!isForOppositeMoves)
	{
		RemoveIllegalMoves();
	}

}

GenerateLegalMoves::GenerateLegalMoves(const std::array<uint8_t, 64>& BoardSquare, const std::array<uint8_t, 64>* previousBoardSquare, canCastle CanCastle, bool isNextMoveForWhite, unsigned int MoveNum, bool isForOppositeMoves)
	:m_BoardSquare(BoardSquare), CanCastle(CanCastle), MoveNum(MoveNum), isNextMoveForWhite(isNextMoveForWhite)
{

	if (previousBoardSquare != nullptr)
		m_previousBoardSquare = *previousBoardSquare;


	GenerateMoves();
	if (!isForOppositeMoves)
	{
		RemoveIllegalMoves();
	}

}

GenerateLegalMoves::~GenerateLegalMoves()
{

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
		uint8_t i = m_BoardSquare[BoardSquarePos];
		if (((i == BLACK_BISHOP or i == BLACK_ROOK or i == BLACK_QUEEN) and !isNextMoveForWhite) or ((i == WHITE_BISHOP or i == WHITE_ROOK or i == WHITE_QUEEN) and isNextMoveForWhite))
		{
			MagicSliderMoveGen(BoardSquarePos);
		}
		else if (i == WHITE_PAWN or i == BLACK_PAWN)
		{
			PawnMoveGen(BoardSquarePos);
		}
		else if (i == WHITE_KNIGHT or i == BLACK_KNIGHT)
		{
			KnightMoveGen(BoardSquarePos);
		}
		else if (i == WHITE_KING or i == BLACK_KING)[[unlikely]]
		{
			KingMoveGen(BoardSquarePos);
		}
	}
}

void GenerateLegalMoves::MagicSliderMoveGen(const uint8_t BoardSquarePos)
{
	//NOTE: to simplify only the queen section is documented

	//vv Declare Frequent use variables vv
	uint8_t PieceTypeUncolored = Board::GetPieceType2Uncolored(m_BoardSquare[BoardSquarePos]);
	uint8_t PieceType = m_BoardSquare[BoardSquarePos];
	bool IsWhite = Board::IsPieceColorWhite(PieceType);
	uint8_t OpponentKingType = IsWhite ? BLACK_KING : WHITE_KING;
	uint8_t BishopShift = BISHOP_SHIFTS[BoardSquarePos];
	uint8_t RookShift = ROOK_SHIFTS[BoardSquarePos];
	
	bit::BitBoard64 blockers = m_BoardSquare.ColorPositions[0] | m_BoardSquare.ColorPositions[1];
	bit::BitBoard64 Attacks(0);

	if ((PieceTypeUncolored == ROOK))
	{
		auto Key = mult_rightShift(blockers.ReadBits() & ROOK_MASKS[BoardSquarePos], ROOK_MAGICS[BoardSquarePos], RookShift);

		Attacks = ROOK_ATTACKS[BoardSquarePos][Key];

		if (IsWhite) { Attacks ^= (Attacks & m_BoardSquare.ColorPositions[0]); }
		else { Attacks ^= (Attacks & m_BoardSquare.ColorPositions[1]); }

		AttackedSquares |= Attacks;
	}
	else if ((PieceTypeUncolored == BISHOP))
	{
		uint16_t BishopAttackIndex = mult_rightShift(blockers.ReadBits() & BISHOP_MASKS[BoardSquarePos], BISHOP_MAGICS[BoardSquarePos], BishopShift);

		Attacks = BISHOP_ATTACKS[BoardSquarePos][BishopAttackIndex];

		if (IsWhite) { Attacks ^= (Attacks & m_BoardSquare.ColorPositions[0]); }
		else { Attacks ^= (Attacks & m_BoardSquare.ColorPositions[1]); }

		AttackedSquares |= Attacks;
	}
	else if ((PieceTypeUncolored == QUEEN))
	{
		
		//Get Indeces from the mult-right shift
		uint16_t RookAttackIndex = mult_rightShift(blockers.ReadBits() & ROOK_MASKS[BoardSquarePos], ROOK_MAGICS[BoardSquarePos], RookShift);
		uint16_t BishopAttackIndex = mult_rightShift(blockers.ReadBits() & BISHOP_MASKS[BoardSquarePos], BISHOP_MAGICS[BoardSquarePos], BishopShift);

		bit::BitBoard64 RookAttack = ROOK_ATTACKS[BoardSquarePos][RookAttackIndex];
		bit::BitBoard64 BishopAttack = BISHOP_ATTACKS[BoardSquarePos][BishopAttackIndex];

		//removes attacks to own pieces
		if (IsWhite)
		{
			RookAttack ^= RookAttack & m_BoardSquare.ColorPositions[0];
			BishopAttack ^= BishopAttack & m_BoardSquare.ColorPositions[0];
		}
		else
		{
			RookAttack ^= RookAttack & m_BoardSquare.ColorPositions[1];
			BishopAttack ^= BishopAttack & m_BoardSquare.ColorPositions[1];
		}

		AttackedSquares |= (RookAttack | BishopAttack);

		Attacks = RookAttack | BishopAttack;
	}

	//found check
	if ((Attacks & m_BoardSquare.find(OpponentKingType)) != 0)
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
				if (m_BoardSquare[BoardSquarePos + (Scalar * OffsetForDirections[Direction])] == OpponentKingType)
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
			CheckTargetSquares[CheckBoardSquare] = BoardSquarePos;
		}
	}
	//possible pin, logic for finding and saving is the same as above, DO NOT LET CHECKS IN HERE
	else if (((ROOK_LEGAL_MASKS[BoardSquarePos] | BISHOP_LEGAL_MASKS[BoardSquarePos]) & m_BoardSquare.find(OpponentKingType)) != 0)
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

		std::array<uint8_t, 7> GuessPinBoardSquares;
		GuessPinBoardSquares.fill(65);
		for (; Direction < MaxDir; Direction++)
		{
			for (uint8_t Scalar = 1; Scalar <= NumOfSquaresUntilEdge[BoardSquarePos][Direction]; Scalar++)
			{
				GuessPinBoardSquares[Scalar - 1] = BoardSquarePos + (Scalar * OffsetForDirections[Direction]);
				if (m_BoardSquare[BoardSquarePos + (Scalar * OffsetForDirections[Direction])] == OpponentKingType)
				{
					goto OutsidePinLoop;
				}
			}
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
	if ((m_BoardSquare[BoardSquarePos] == WHITE_KNIGHT and isNextMoveForWhite) or (m_BoardSquare[BoardSquarePos] == BLACK_KNIGHT and !isNextMoveForWhite))
	{
		const std::array<uint8_t, 8> OffsetsForKnight = CreateOffesetsForKnight(BoardSquarePos);
		for (const uint8_t& i : OffsetsForKnight)
		{
			if (i == 65)
				break;

			AttackedSquares[i] = true;
			//i know that if only the first condition is met then c++ won't check the other one, if it does i'm blaming c++ and i WILL be calling it stupid
			if ((m_BoardSquare[i]) == 0 or (Board::IsPieceColorWhite(m_BoardSquare[BoardSquarePos]) != Board::IsPieceColorWhite(m_BoardSquare[i])))
			{
				moves[BoardSquarePos].PieceType = m_BoardSquare[BoardSquarePos];
				moves[BoardSquarePos].TargetSquares.push_back(i);
				if (m_BoardSquare[i] == BLACK_KING and isNextMoveForWhite or m_BoardSquare[i] == WHITE_KING and !isNextMoveForWhite)
				{
					if (CheckTargetSquares[i] != 65)
						DoubleCheckBoardSquare = BoardSquarePos;
					else
						CheckTargetSquares[i] = BoardSquarePos;
				}
			}
			
		}
	}
}

std::array<uint8_t, 8>& GenerateLegalMoves::CreateOffesetsForKnight(const uint8_t BoardSquarePos)
{
	std::array<uint8_t, 8> OffsetsForKnight;
	uint8_t count = 0;
	for (uint8_t i = 0; i <= 4; i++)
	{
		if (NumOfSquaresUntilEdge[BoardSquarePos][i] >= 1)
		{
			if (i == 0 and BoardSquarePos <= 47)
			{
				if (NumOfSquaresUntilEdge[BoardSquarePos + 16][2] >= 1)
				{
					OffsetsForKnight[count] = (BoardSquarePos + 15);
					count++;
				}
				if (NumOfSquaresUntilEdge[BoardSquarePos + 16][3] >= 1)
				{
					OffsetsForKnight[count] = (BoardSquarePos + 17);
					count++;
				}
			}
			else if (i == 1 and BoardSquarePos >= 16)
			{
				if (NumOfSquaresUntilEdge[BoardSquarePos - 16][2] >= 1)
				{
					OffsetsForKnight[count] = (BoardSquarePos - 17);
					count++;
				}
				if (NumOfSquaresUntilEdge[BoardSquarePos - 16][3] >= 1)
				{
					OffsetsForKnight[count] = (BoardSquarePos - 15);
					count++;
				}
			}
			if (NumOfSquaresUntilEdge[BoardSquarePos][2] >= 2)
			{
				if (i == 2 and BoardSquarePos >= 2)
				{
					if (NumOfSquaresUntilEdge[BoardSquarePos - 2][0] >= 1)
					{
						OffsetsForKnight[count] = (BoardSquarePos + 6);
						count++;
					}
					if (NumOfSquaresUntilEdge[BoardSquarePos - 2][1] >= 1)
					{
						OffsetsForKnight[count] = (BoardSquarePos - 10);
						count++;
					}
				}
			}
			if (NumOfSquaresUntilEdge[BoardSquarePos][3] >= 2)
			{
				if (i == 3 and BoardSquarePos <= 61)
				{
					if (NumOfSquaresUntilEdge[BoardSquarePos + 2][0] >= 1)
					{
						OffsetsForKnight[count] = (BoardSquarePos + 10);
						count++;
					}
					if (NumOfSquaresUntilEdge[BoardSquarePos + 2][1] >= 1)
					{
						OffsetsForKnight[count] = (BoardSquarePos - 6);
						count++;
					}
				}
			}
		}
	}
	for (;count < 8; count++)
	{
		OffsetsForKnight[count] = 65;
	}
	return OffsetsForKnight;
}

//PAWN
void GenerateLegalMoves::PawnMoveGen(const uint8_t BoardSquarePos)
{
#ifdef _DEBUG
	if (BoardSquarePos < 8 or BoardSquarePos > 55)
	{
		ASSERT(false);//this should be made an assert
	}
#endif
	uint8_t PieceType = m_BoardSquare[BoardSquarePos];
	if (PieceType == WHITE_PAWN and isNextMoveForWhite)//White pawn
	{
		for (const int& Offset : OffsetForWhitePawn)
		{
			if ((BoardSquarePos == 48 and Offset == 7) or (BoardSquarePos == 55 and Offset == 9))
				continue;

			uint8_t BoardPosPlusOffset = BoardSquarePos + Offset;
			uint8_t PieceTypeAtOffset = m_BoardSquare[BoardPosPlusOffset];


			if (NumOfSquaresUntilEdge[BoardSquarePos][WhitePawnDirections[Offset-7]] != 0)
			{

				if (PieceTypeAtOffset != 0 and Offset != 8)
				{
					AttackedSquares[BoardPosPlusOffset] = true;
					if (!Board::IsPieceColorWhite(PieceTypeAtOffset))
					{
						moves[BoardSquarePos].PieceType = PieceType;
						moves[BoardSquarePos].TargetSquares.push_back(BoardPosPlusOffset);

						//promotion
						if (BoardPosPlusOffset >= 56)
						{
							moves[BoardSquarePos].Promotion.SetPromotionSide(Offset - 7, BoardSquarePos);
						}
					}
					if (PieceTypeAtOffset == BLACK_KING)
					{
						if (CheckTargetSquares[BoardPosPlusOffset] != 65)
							DoubleCheckBoardSquare = BoardSquarePos;
						else
							CheckTargetSquares[BoardPosPlusOffset] = BoardSquarePos;
					}


				}
				else if (Offset == 8 and PieceTypeAtOffset == 0)
				{
					if (BoardSquarePos < 16 and BoardSquarePos >= 8 and m_BoardSquare[BoardSquarePos + 16] == 0)
					{
						moves[BoardSquarePos].PieceType = PieceType;
						moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + 16);
					}
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].TargetSquares.push_back(BoardPosPlusOffset);

					//promotion
					if (BoardPosPlusOffset >= 56)
						moves[BoardSquarePos].Promotion.SetPromotionSide(1, BoardSquarePos);
				}
				else if (PieceTypeAtOffset == 0 and Offset != 8)
				{
					moves[BoardSquarePos].PieceType = PieceType;
					AttackedSquares[BoardPosPlusOffset] = true;
				}

				
				//en passant
				if (BoardSquarePos <= 39 and BoardSquarePos >= 32 and Offset != 8 and PieceTypeAtOffset == 0 and !m_previousBoardSquare.empty() and (m_previousBoardSquare[BoardSquarePos + Offset + 8] == BLACK_PAWN) and (m_BoardSquare[BoardSquarePos + Offset - 8] == BLACK_PAWN) and !(m_previousBoardSquare[BoardSquarePos + Offset - 8] == BLACK_PAWN) and m_BoardSquare[BoardSquarePos + Offset + 8] == 0)
				{
					if (!DoNotEnPassant)
					{
						moves[BoardSquarePos].PieceType = PieceType;
						moves[BoardSquarePos].TargetSquares.push_back(BoardPosPlusOffset);
						EnPassantFiles[(BoardSquarePos + Offset) % 8] = true;
					}
				}
			}
		}
	}
	else if (PieceType == BLACK_PAWN and !isNextMoveForWhite)//Black pawn
	{
		for (const int& Offset : OffsetForBlackPawn)
		{
			if ((BoardSquarePos == 8 and Offset == -9) or (BoardSquarePos == 15 and Offset == -7))
				continue;

			uint8_t BoardPosPlusOffset = BoardSquarePos + Offset;
			uint8_t PieceTypeAtOffset = m_BoardSquare[BoardPosPlusOffset];

			


			if (NumOfSquaresUntilEdge[BoardSquarePos][BlackPawnDirections[Offset+9]] != 0)
			{
				if (PieceTypeAtOffset != 0 and Offset != -8)
				{
					AttackedSquares[BoardPosPlusOffset] = true;
					if (Board::IsPieceColorWhite(PieceTypeAtOffset))
					{
						moves[BoardSquarePos].PieceType = PieceType;
						moves[BoardSquarePos].TargetSquares.push_back(BoardPosPlusOffset);

						//promotion
						if (BoardPosPlusOffset <= 7)
						{
							moves[BoardSquarePos].Promotion.SetPromotionSide(Offset + 9, BoardSquarePos);
						}
					}
					if (PieceTypeAtOffset == WHITE_KING)
					{
						if (CheckTargetSquares[BoardPosPlusOffset] != 65)
							DoubleCheckBoardSquare = BoardSquarePos;
						else
							CheckTargetSquares[BoardPosPlusOffset] = BoardSquarePos;
					}


				}
				else if (Offset == -8 and PieceTypeAtOffset == 0)
				{
					if (BoardSquarePos <= 55 and BoardSquarePos >= 48 and m_BoardSquare[BoardSquarePos - 16] == 0)
					{
						moves[BoardSquarePos].PieceType = PieceType;
						moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos - 16);
					}
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].TargetSquares.push_back(BoardPosPlusOffset);

					//promotion
					if (BoardPosPlusOffset <= 7)
						moves[BoardSquarePos].Promotion.SetPromotionSide(1, BoardSquarePos);
				}
				else if (PieceTypeAtOffset == 0 and Offset != -8)
				{
					moves[BoardSquarePos].PieceType = PieceType;
					AttackedSquares[BoardPosPlusOffset] = true;
				}
				//en passant
				if (BoardSquarePos <= 31 and BoardSquarePos >= 24 and Offset != -8 and PieceTypeAtOffset == 0 and !m_previousBoardSquare.empty() and (m_previousBoardSquare[BoardSquarePos + Offset - 8]) == WHITE_PAWN and (m_BoardSquare[BoardSquarePos + Offset + 8]) == WHITE_PAWN and !(m_previousBoardSquare[BoardSquarePos + Offset + 8] == WHITE_PAWN) and m_BoardSquare[BoardSquarePos + Offset - 8] == 0)
				{
					if (!DoNotEnPassant)
					{
						moves[BoardSquarePos].PieceType = PieceType;
						moves[BoardSquarePos].TargetSquares.push_back(BoardPosPlusOffset);
						EnPassantFiles[(BoardSquarePos + Offset) % 8] = true;
					}
				}
			}
		}
	}
}

//King
void GenerateLegalMoves::KingMoveGen(const uint8_t BoardSquarePos)
{
	if ((m_BoardSquare[BoardSquarePos] == WHITE_KING and isNextMoveForWhite) or (m_BoardSquare[BoardSquarePos] == BLACK_KING and !isNextMoveForWhite))
	{
		uint8_t PieceType = m_BoardSquare[BoardSquarePos];
		for (uint8_t direction = 0; direction < 8; direction++)
		{
			if (NumOfSquaresUntilEdge[BoardSquarePos][direction] > 0)
			{
				AttackedSquares[BoardSquarePos + OffsetForDirections[direction]] = true;
				if (m_BoardSquare[BoardSquarePos + OffsetForDirections[direction]] == 0)
				{
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + OffsetForDirections[direction]);
					continue;
				}
				if (Board::IsPieceColorWhite(PieceType) != Board::IsPieceColorWhite(m_BoardSquare[BoardSquarePos + OffsetForDirections[direction]]))
				{
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + OffsetForDirections[direction]);
				}
			}
		}
		if (PieceType == WHITE_KING and !CanCastle.HasWhiteKingMoved)//castling
		{
			if (!CanCastle.HasWhiteLongRookMoved and m_BoardSquare[1] == 0 and m_BoardSquare[2] == 0 and m_BoardSquare[3] == 0)
			{
				moves[4].TargetSquares.push_back(2);
			}
			if (!CanCastle.HasWhiteShortRookMoved and m_BoardSquare[5] == 0 and m_BoardSquare[6] == 0)
			{
				moves[4].TargetSquares.push_back(6);
			}
		}
		if (PieceType == BLACK_KING and !CanCastle.HasBlackKingMoved)//castling
		{
			if (!CanCastle.HasBlackLongRookMoved and m_BoardSquare[57] == 0 and m_BoardSquare[58] == 0 and m_BoardSquare[59] == 0)
			{
				moves[60].TargetSquares.push_back(58);
			}
			if (!CanCastle.HasBlackShortRookMoved and m_BoardSquare[61] == 0 and m_BoardSquare[62] == 0)
			{
				moves[60].TargetSquares.push_back(62);
			}
		}
	}
}

void GenerateLegalMoves::RemoveIllegalMoves()
{	
	uint8_t IndexOfPieceChecking;
	uint8_t BoardSquareOfKingToMove = 0;

	//find out where the King is
	bit::FindBit(m_BoardSquare.find(isNextMoveForWhite ? WHITE_KING : BLACK_KING), BoardSquareOfKingToMove);
	
	auto p_prevBoardSquare = &m_previousBoardSquare;
	if (MoveNum == 0)
		p_prevBoardSquare = nullptr;

	GenerateLegalMoves OppositeMoves(m_BoardSquare,p_prevBoardSquare, CanCastle, !isNextMoveForWhite, MoveNum, true);
	OppositeAttackedSquares = OppositeMoves.AttackedSquares;

	//fill IndexOfPieceChecking
	uint8_t count = 0;
	uint8_t CheckingSquare = 65;
	size_t NumberOfChecks = 0;
 	for (const uint8_t& Square : OppositeMoves.CheckTargetSquares)
	{
		if (Square != 65)
		{
			if (CheckingSquare == Square)
			{
				continue;
			}
			else
			{
				IndexOfPieceChecking = Square;
				CheckingSquare = Square;
				NumberOfChecks++;
			}
		}
	}


	//moves for King
	if (NumberOfChecks >= 0)
	{
		//when double checked only moves made by king can be legal, so clear the others
		if (NumberOfChecks == 2)
		{
			for (MOVE_BIT& Piece : moves)
			{
				if (Piece.PieceType != m_BoardSquare[BoardSquareOfKingToMove])
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
			if (Piece.PieceType == WHITE_KING or Piece.PieceType == BLACK_KING or Piece.PieceType == 0)
			{
				count++;
				continue;
			}

			auto MovesCopy = Piece.TargetSquares;
			//if in check and count is under abs pin, unable to move
			if (OppositeMoves.WhichBoardSquaresAreAbsPinned[count] != 65 and NumberOfChecks == 1)
			{
				Piece.TargetSquares.clear();
				count++;
				continue;
			}
			else 
			{
				uint8_t Move = 0;
				while (true)
				{
					if (!bit::pop_lsb(MovesCopy, Move)) { break; }
					
					//for those things that happen with pawns
					if (Piece.PieceType == BLACK_PAWN or Piece.PieceType == WHITE_PAWN)
					{
						if (DoNotEnPassant and m_BoardSquare[Move] == 0 and (abs(count - Move) == NW or abs(count - Move) == NE))
						{
							Piece.TargetSquares[Move] = false;
							continue;
						}
					}

					if ((OppositeMoves.WhichBoardSquaresAreAbsPinned[count] != 65 and NumberOfChecks == 0) or NumberOfChecks == 1)
					{
						//if move is an en passant add +-8 to move
						if ((Piece.PieceType == BLACK_PAWN or Piece.PieceType == WHITE_PAWN) and (abs(count - Move) == NW or abs(count - Move) == NE) and m_BoardSquare[Move] == 0)
						{
							Piece.PieceType == WHITE_PAWN ? Move -= 8 : Move += 8;
						}

						if (NumberOfChecks == 1)
						{
							//checks if Move neither captures piece nor doesn't block check
							if (Move != IndexOfPieceChecking and OppositeMoves.CheckTargetSquares[Move] != IndexOfPieceChecking)
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
		if ((Piece.PieceType == WHITE_PAWN or Piece.PieceType == BLACK_PAWN) and ((Piece.Promotion.Promotion & PromotionMask) != 0))[[unlikely]]
		{
			m_NumOfLegalMoves += std::popcount((uint8_t)(Piece.Promotion.Promotion & PromotionMask)) * 4;
		}
		else
		{
			m_NumOfLegalMoves += Piece.TargetSquares.popcnt();
		}
	}
	
}

//fun for LegalMoves, checks if castling is permitted
void GenerateLegalMoves::CanKingCastle_LMoves(const GenerateLegalMoves& OppositeMoves, bool& isItCheckmate, const uint8_t& BoardSquareOfKingToMove, const uint8_t KingMove)
{
	if (Board::IsPieceColorWhite(m_BoardSquare[BoardSquareOfKingToMove]))
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
	if (moves[CheckedMove.s_BoardSquare].TargetSquares[CheckedMove.s_Move] == true and (moves[CheckedMove.s_BoardSquare].Promotion.Promotion == 0 and CheckedMove.s_PromotionType != NULL_OPTION))
	{
		return true;
	}
	return false;
}

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

void MagicRookFinder(uint8_t BoardSquare)
{
	uint64_t MagicNum;
	bool Failed = false;
	uint8_t shift = ROOK_SHIFTS[BoardSquare];
	std::array<uint16_t, 4096> SeenKeys{ UINT16_MAX };
	for (uint64_t Try = 0; Try < UINT64_MAX; Try++)
	{
		MagicNum = Random64Bit() & Random64Bit() & Random64Bit();
		SeenKeys.fill(UINT16_MAX);
		Failed = false;
		for (uint16_t Blocker = 0; Blocker < (1ULL << shift); Blocker++)
		{
			auto Key = mult_rightShift(expand_bits_to_mask(Blocker, ROOK_MASKS[BoardSquare]), MagicNum, shift);
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
			for (uint16_t Blocker = 0; Blocker < (1ULL << shift); Blocker++)
			{
				auto Key = mult_rightShift(expand_bits_to_mask(Blocker, ROOK_MASKS[BoardSquare]), MagicNum, shift);
				ROOK_ATTACKS[BoardSquare][Key] = ComputeRookAttacks(BoardSquare, Blocker) & ROOK_LEGAL_MASKS[BoardSquare];
			}
			ROOK_MAGICS[BoardSquare] = MagicNum;
			break;
		}
	}

}

void MagicBishopFinder(uint8_t BoardSquare)
{
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
			auto Key = mult_rightShift(expand_bits_to_mask(Blocker, BISHOP_MASKS[BoardSquare]), MagicNum, shift);
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
			for (uint16_t Blocker = 0; Blocker < (1ULL << shift); Blocker++)
			{
				auto Key = mult_rightShift(expand_bits_to_mask(Blocker, BISHOP_MASKS[BoardSquare]), MagicNum, shift);
				BISHOP_ATTACKS[BoardSquare][Key] = ComputeBishopAttacks(BoardSquare, Blocker) & BISHOP_LEGAL_MASKS[BoardSquare];
			}
			BISHOP_MAGICS[BoardSquare] = MagicNum;
			break;
		}
	}

}

uint16_t mult_rightShift(uint64_t BlockerBits, uint64_t Magic, uint8_t RelevantBitNum)
{
	return (BlockerBits * Magic) >> (64 - RelevantBitNum);
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
		for (uint8_t BoardSquare = 0; BoardSquare <= MAX_SQUARE; BoardSquare++)
		{
			MagicRookFinder(BoardSquare);//compute magics and attacks
			std::cout << "Found Bsquare: " << toascii(BoardSquare) << " with magic " << ROOK_MAGICS[BoardSquare] << std::endl;
		}
		RookPrecomputes.open(PrecomputesPath / "Rook_Attacks.bin", std::ios::out | std::ios::binary);
		RookPrecomputes.write(reinterpret_cast<const char*>(ROOK_ATTACKS.data()), sizeof(ROOK_ATTACKS));//save the attacks to the binary file
		RookPrecomputes.close();
		RookPrecomputes.open(PrecomputesPath / "Rook_Magics.bin", std::ios::out | std::ios::binary);
		RookPrecomputes.write(reinterpret_cast<const char*>(ROOK_MAGICS.data()), sizeof(ROOK_MAGICS));//save the magics to the binary file
	}
	else
	{
		RookPrecomputes.open(PrecomputesPath / "Rook_Attacks.bin", std::ios::in | std::ios::binary);
		RookPrecomputes.read(reinterpret_cast<char*>(ROOK_ATTACKS.data()), sizeof(ROOK_ATTACKS)); //set ROOK_ATTACKS from disk
		RookPrecomputes.close();
		RookPrecomputes.open(PrecomputesPath / "Rook_Magics.bin", std::ios::in | std::ios::binary);
		RookPrecomputes.read(reinterpret_cast<char*>(ROOK_MAGICS.data()), sizeof(ROOK_MAGICS));
	}
	
	std::fstream BishopPrecomputes;
	if (!fs::exists(PrecomputesPath / "Bishop_Attacks.bin") or !fs::exists(PrecomputesPath / "Bishop_Magics.bin"))//if BISHOP_ATTACKS or magics wasn't created
	{
		for (uint8_t BoardSquare = 0; BoardSquare <= MAX_SQUARE; BoardSquare++)
		{
			MagicBishopFinder(BoardSquare);//compute magics and attacks
			std::cout << "Found Bsquare: " << toascii(BoardSquare) << " with magic " << BISHOP_MAGICS[BoardSquare] << std::endl;
		}
		BishopPrecomputes.open(PrecomputesPath / "Bishop_Attacks.bin", std::ios::out | std::ios::binary);
		BishopPrecomputes.write(reinterpret_cast<const char*>(BISHOP_ATTACKS.data()), sizeof(BISHOP_ATTACKS));//save the table to the binary file
		BishopPrecomputes.close();
		BishopPrecomputes.open(PrecomputesPath / "Bishop_Magics.bin", std::ios::out | std::ios::binary);
		BishopPrecomputes.write(reinterpret_cast<const char*>(BISHOP_MAGICS.data()), sizeof(BISHOP_MAGICS));//save the magics to the binary file
	}
	else
	{
		BishopPrecomputes.open(PrecomputesPath / "Bishop_Attacks.bin", std::ios::in | std::ios::binary);
		BishopPrecomputes.read(reinterpret_cast<char*>(BISHOP_ATTACKS.data()), sizeof(BISHOP_ATTACKS)); //set BISHOP_ATTACKS from disk
		BishopPrecomputes.close();
		BishopPrecomputes.open(PrecomputesPath / "Bishop_Magics.bin", std::ios::in | std::ios::binary);
		BishopPrecomputes.read(reinterpret_cast<char*>(BISHOP_MAGICS.data()), sizeof(BISHOP_MAGICS));
	}

	AlreadyRan = true;

}

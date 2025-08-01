#include "LegalMoves.h"
static bit::BitPosition m_previousBoardSquare;
static std::array<MOVE_BIT, 64> OppositeMoves;
static bool DoNotEnPassant; //for weird RemoveIllegalMoves things


GenerateLegalMoves::GenerateLegalMoves(const bit::BitPosition& BoardSquare, const bit::BitPosition* previousBoardSquare, canCastle CanCastle, bool isNextMoveForWhite, unsigned int MoveNum, bool isForOppositeMoves)
	:moves(), m_BoardSquare(BoardSquare), CanCastle(CanCastle), MoveNum(MoveNum), isNextMoveForWhite(isNextMoveForWhite)
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
	PinnedSquaresWithTheKingBeingPinned.fill(false);
	WhichBoardSquaresAreAbsPinned.fill(65);
	CheckTargetSquares.fill(65);
	uint8_t BoardSquarePos = 0;
	for (; BoardSquarePos <= 64; BoardSquarePos++)
	{ 
		uint8_t i = m_BoardSquare[BoardSquarePos];
		if (((i == BLACK_BISHOP or i == BLACK_ROOK or i == BLACK_QUEEN) and !isNextMoveForWhite) or ((i == WHITE_BISHOP or i == WHITE_ROOK or i == WHITE_QUEEN) and isNextMoveForWhite))
		{
			SliderMoveGen(BoardSquarePos);
		}
		else if (i == WHITE_PAWN or i == BLACK_PAWN)
		{
			PawnMoveGen(BoardSquarePos);
		}
		else if (i == WHITE_KNIGHT or i == BLACK_KNIGHT)
		{
			KnightMoveGen(BoardSquarePos);
		}
		else if (i == WHITE_KING or i == BLACK_KING)
		{
			KingMoveGen(BoardSquarePos);
		}
		BoardSquarePos++;
	}
}

void GenerateLegalMoves::SliderMoveGen(const uint8_t BoardSquarePos)
{
	uint8_t PieceType = m_BoardSquare[BoardSquarePos];
	std::vector<uint8_t>::iterator iterator;
	std::vector<uint8_t>::iterator absPin_iter;
	MOVE_BIT& Piece = moves[BoardSquarePos];

	for (uint8_t direction = 0; direction < 8; direction++)
	{
		if ((PieceType == WHITE_ROOK or PieceType == BLACK_ROOK) and direction > 3)
			return;
		else if ((PieceType == WHITE_BISHOP or PieceType == BLACK_BISHOP) and direction < 4)
		{
			continue;
		}


		for (uint8_t i = 1; i <= NumOfSquaresUntilEdge[BoardSquarePos][direction]; i++)
		{
			uint8_t PieceTypeAtOffset = m_BoardSquare[BoardSquarePos + (OffsetForDirections[direction] * i)];
			AttackedSquares[BoardSquarePos + (OffsetForDirections[direction] * i)] = true;

			if (PieceTypeAtOffset == 0)
			{
				Piece.PieceType = PieceType;
				Piece.TargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * i));
				continue;
			}
			else if (Board::IsPieceColorWhite(PieceType) != Board::IsPieceColorWhite(PieceTypeAtOffset))
			{
				Piece.PieceType = PieceType;
				Piece.TargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * i));

				//calculate checking squares
				if ((PieceTypeAtOffset == BLACK_KING and isNextMoveForWhite) or (PieceTypeAtOffset == WHITE_KING and !isNextMoveForWhite))
				{
					iterator = Piece.TargetSquares.end();

					for (int j = 0; -i + j < 0; j++)
					{
						if (CheckTargetSquares[*(iterator - i + j)] != 65)
							DoubleCheckBoardSquare = BoardSquarePos;
						else
							CheckTargetSquares[*(iterator - i + j)] = BoardSquarePos;
					}
				}

				std::vector<uint8_t> PinnedTargetSquares;

				//calculate pinnable squares
				for (uint8_t j = i + 1; j <= NumOfSquaresUntilEdge[BoardSquarePos][direction]; j++)
				{
					uint8_t PieceTypeAtOffsetBehind = m_BoardSquare[BoardSquarePos + (OffsetForDirections[direction] * j)];

					PinnedTargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * j));
					if ((PieceTypeAtOffset == BLACK_KING and isNextMoveForWhite) or (PieceTypeAtOffset == WHITE_KING and !isNextMoveForWhite))
						PinnedSquaresWithTheKingBeingPinned[BoardSquarePos + (OffsetForDirections[direction] * j)] = true;

					//calculate abs pins
					if ((PieceTypeAtOffsetBehind == BLACK_KING and isNextMoveForWhite) or (PieceTypeAtOffsetBehind == WHITE_KING and !isNextMoveForWhite))
					{
						absPin_iter = Piece.TargetSquares.end();

						bool DoNotRun = false;
						for (auto k = PinnedTargetSquares.begin(); k < PinnedTargetSquares.end() - 1; k++)
						{
							if (m_BoardSquare[*(k)] != 0)
							{
								DoNotRun = true;
							}
						}

						if (!DoNotRun)
						{
							WhichBoardSquaresAreAbsPinned[BoardSquarePos + (OffsetForDirections[direction] * i)] = BoardSquarePos;
							for (uint8_t k = 0; k < i; k++)
							{
								WhichBoardSquaresAreAbsPinned[*(absPin_iter - k - 1)] = BoardSquarePos;
							}
							for (uint8_t PinnedSquare : PinnedTargetSquares)
							{
								WhichBoardSquaresAreAbsPinned[PinnedSquare] = BoardSquarePos;
							}
							DoNotRun = false;
							break;
						}

					}

				}
				break;
			}
			else
				break;
		}
	}
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
	if (BoardSquarePos < 8 or BoardSquarePos > 55)
	{
		return;//this should be made an assert
	}
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
							moves[BoardSquarePos].Promotion[Offset - 7] = BoardPosPlusOffset;
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
						moves[BoardSquarePos].Promotion[1] = BoardPosPlusOffset;
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

	if (PieceType == BLACK_PAWN and !isNextMoveForWhite)//Black pawn
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
							moves[BoardSquarePos].Promotion[Offset + 9] = BoardPosPlusOffset;
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
						moves[BoardSquarePos].Promotion[1] = BoardPosPlusOffset;
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
	if (isNextMoveForWhite)
	{
		for (BoardSquareOfKingToMove = 0; BoardSquareOfKingToMove < 64; BoardSquareOfKingToMove++)
		{
			if (m_BoardSquare[BoardSquareOfKingToMove] == WHITE_KING)
				break;
		}

	}
	else
	{
		for (BoardSquareOfKingToMove = 0; BoardSquareOfKingToMove < 64; BoardSquareOfKingToMove++)
		{
			if (m_BoardSquare[BoardSquareOfKingToMove] == BLACK_KING)
				break;
		}
	}

	
	auto p_prevBoardSquare = &m_previousBoardSquare;
	if (MoveNum == 0)
		p_prevBoardSquare = nullptr;

	GenerateLegalMoves OppositeMoves(m_BoardSquare,p_prevBoardSquare, CanCastle, !isNextMoveForWhite, MoveNum, true);


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
				CheckingSquare++;
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


		for (uint8_t KingMove = 0; KingMove <= MAX_SQUARE; KingMove++)
		{
			if (!moves[BoardSquareOfKingToMove].TargetSquares[KingMove]) { continue; }

			//checks if possible move square is under attack
			if (OppositeMoves.AttackedSquares[KingMove] == true)
			{
				moves[BoardSquareOfKingToMove].TargetSquares[KingMove] = false;
				continue;
			}
			//checks if possible move is going to be under attack when it's made
			else if (OppositeMoves.PinnedSquaresWithTheKingBeingPinned[KingMove] == true)
			{
				moves[BoardSquareOfKingToMove].TargetSquares[KingMove] = false;
				continue;
			}
			//checks that it can castle
			else if ((BoardSquareOfKingToMove - KingMove == 2) or (BoardSquareOfKingToMove - KingMove == -2))//checked for weird behaviours, rvalue(i think)
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

			//checks if under abs pin
			if (OppositeMoves.WhichBoardSquaresAreAbsPinned[count] != 65 and NumberOfChecks == 1)
			{
				Piece.TargetSquares.clear();
				count++;
				continue;
			}
			else 
			{
				for (uint8_t Move = 0; Move <= MAX_SQUARE; Move++)
				{
					if (!Piece.TargetSquares[Move]) { continue; }
					//for those things that happen with pawns
					if (Piece.PieceType == BLACK_PAWN or Piece.PieceType == WHITE_PAWN)
					{
						if (DoNotEnPassant and m_BoardSquare[Move] == 0 and (abs(count - Move) == 7 or abs(count - Move) == 9))
						{
							Piece.TargetSquares[Move] = false;
							continue;
						}
					}

					if ((OppositeMoves.WhichBoardSquaresAreAbsPinned[count] != 65 and NumberOfChecks == 0) or NumberOfChecks == 1)
					{
						//change the for loop Move so that en passant makes sense
						if ((Piece.PieceType == BLACK_PAWN or Piece.PieceType == WHITE_PAWN) and (abs(count - Move) == 7 or abs(count - Move) == 9) and m_BoardSquare[Move] == 0)
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
		m_NumOfLegalMoves += Piece.TargetSquares.popcnt();
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
	bool MoveFound = false;
	
	for (uint8_t count = 0; count != 64; count++)
	{
		if (CheckedMove.s_BoardSquare != count)
			continue;

		if (moves[count].TargetSquares[CheckedMove.s_Move] != 0)
		{
			MoveFound = true;
			break;
		}
	}

	return MoveFound;
}

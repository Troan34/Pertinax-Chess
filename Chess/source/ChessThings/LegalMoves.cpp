#include "LegalMoves.h"
static std::array<unsigned int, 64> m_previousBoardSquare;
static std::array<MOVE, 64> OppositeMoves;
GenerateLegalMoves::GenerateLegalMoves(std::array<unsigned int, 64Ui64> BoardSquare, std::array<unsigned int, 64> previousBoardSquare, canCastle CanCastle, bool isNextMoveForWhite, unsigned int MoveNum)
	:moves(), m_BoardSquare(BoardSquare), CanCastle(CanCastle), MoveNum(MoveNum)
{
	for (int file = 0; file < 8; file++)
	{
		for (int rank = 0; rank < 8; rank++)
		{
			int numNorth = 7 - rank;
			int numSouth = rank;
			int numWest = file;
			int numEast = 7 - file;

			unsigned int squareIndex = rank * 8 + file;

			NumOfSquaresUntilEdge[squareIndex][0] = numNorth;
			NumOfSquaresUntilEdge[squareIndex][1] = numSouth;
			NumOfSquaresUntilEdge[squareIndex][2] = numWest;
			NumOfSquaresUntilEdge[squareIndex][3] = numEast;
			NumOfSquaresUntilEdge[squareIndex][4] = std::min(numNorth, numWest);
			NumOfSquaresUntilEdge[squareIndex][5] = std::min(numSouth, numEast);
			NumOfSquaresUntilEdge[squareIndex][6] = std::min(numNorth, numEast);
			NumOfSquaresUntilEdge[squareIndex][7] = std::min(numSouth, numWest);

		}
	}
	GenerateMoves(isNextMoveForWhite);
	RemoveIllegalMoves_Checks_AbsolutePins(isNextMoveForWhite);
	if (!isItCheckmate)
	{
		if (!isA64ArrayEmpty(LegalMoves))
			moves = LegalMoves;
	}
	else
		std::cout << "Checkmate" << '\n';
	m_previousBoardSquare = previousBoardSquare;
}

GenerateLegalMoves::GenerateLegalMoves(std::array<unsigned int, 64Ui64> BoardSquare, canCastle CanCastle, bool isNextMoveForWhite, unsigned int MoveNum)
	:moves(), m_BoardSquare(BoardSquare), CanCastle(CanCastle), MoveNum(MoveNum)
{
	for (int file = 0; file < 8; file++)
	{
		for (int rank = 0; rank < 8; rank++)
		{
			int numNorth = 7 - rank;
			int numSouth = rank;
			int numWest = file;
			int numEast = 7 - file;

			unsigned int squareIndex = rank * 8 + file;

			NumOfSquaresUntilEdge[squareIndex][0] = numNorth;
			NumOfSquaresUntilEdge[squareIndex][1] = numSouth;
			NumOfSquaresUntilEdge[squareIndex][2] = numWest;
			NumOfSquaresUntilEdge[squareIndex][3] = numEast;
			NumOfSquaresUntilEdge[squareIndex][4] = std::min(numNorth, numWest);
			NumOfSquaresUntilEdge[squareIndex][5] = std::min(numSouth, numEast);
			NumOfSquaresUntilEdge[squareIndex][6] = std::min(numNorth, numEast);
			NumOfSquaresUntilEdge[squareIndex][7] = std::min(numSouth, numWest);

		}
	}
	GenerateMoves(isNextMoveForWhite);
	RemoveIllegalMoves_Checks_AbsolutePins(isNextMoveForWhite);
	if (!isItCheckmate)
	{
		if (!isA64ArrayEmpty(LegalMoves))
			moves = LegalMoves;
	}
	else
		std::cout << "Checkmate" << '\n';
}

GenerateLegalMoves::GenerateLegalMoves(std::array<unsigned int, 64Ui64> BoardSquare, canCastle CanCastle, bool isNextMoveForWhite, unsigned int MoveNum, bool isForOppositeMoves)
	:moves(), m_BoardSquare(BoardSquare), CanCastle(CanCastle), MoveNum(MoveNum)
{
	for (int file = 0; file < 8; file++)
	{
		for (int rank = 0; rank < 8; rank++)
		{
			int numNorth = 7 - rank;
			int numSouth = rank;
			int numWest = file;
			int numEast = 7 - file;

			unsigned int squareIndex = rank * 8 + file;

			NumOfSquaresUntilEdge[squareIndex][0] = numNorth;
			NumOfSquaresUntilEdge[squareIndex][1] = numSouth;
			NumOfSquaresUntilEdge[squareIndex][2] = numWest;
			NumOfSquaresUntilEdge[squareIndex][3] = numEast;
			NumOfSquaresUntilEdge[squareIndex][4] = std::min(numNorth, numWest);
			NumOfSquaresUntilEdge[squareIndex][5] = std::min(numSouth, numEast);
			NumOfSquaresUntilEdge[squareIndex][6] = std::min(numNorth, numEast);
			NumOfSquaresUntilEdge[squareIndex][7] = std::min(numSouth, numWest);

		}
	}
	GenerateMoves(isNextMoveForWhite);
}

void GenerateLegalMoves::GenerateMoves(bool isNextMoveForWhite)
{
	AttackedSquares.fill(false);
	PinnedSquaresWithTheKingBeingPinned.fill(false);
	WhichBoardSquaresAreAbsPinned.fill(0);
	int BoardSquarePos = 0;
	for (int i : m_BoardSquare)
	{ 
		if (i == 0)
		{
			BoardSquarePos++;
			continue;
		}
		else if (i == 10 or i == 12 or i == 18 or i == 20 or i == 21 or i == 13)
		{
			SliderMoveGen(BoardSquarePos, isNextMoveForWhite);
			BoardSquarePos++;
			continue;
		}
		else if (i == 17 or i == 9)
		{
			PawnMoveGen(BoardSquarePos, isNextMoveForWhite);
			BoardSquarePos++;
			continue;
		}
		else if (i == 19 or i == 11)
		{
			KnightMoveGen(BoardSquarePos, isNextMoveForWhite);
			BoardSquarePos++;
			continue;
		}
		else if (i == 22 or i == 14)
		{
			KingMoveGen(BoardSquarePos, isNextMoveForWhite);
			BoardSquarePos++;
		}
	}

}

void GenerateLegalMoves::SliderMoveGen(int BoardSquarePos, bool isNextMoveForWhite)
{
	unsigned int PieceType = m_BoardSquare[BoardSquarePos];
	std::vector<unsigned int>::iterator iterator;
	std::vector<unsigned int>::iterator absPin_iter;
	for (unsigned int direction = 0; direction < 8; direction++)
	{
		for (int i = 1; i <= NumOfSquaresUntilEdge[BoardSquarePos][direction]; i++)
		{
			int PieceTypeAtOffset = m_BoardSquare[BoardSquarePos + (OffsetForDirections[direction] * i)];
			//rook
			if (((PieceType == 20 and isNextMoveForWhite) or (PieceType == 12 and !isNextMoveForWhite)) and direction <= 3)
			{
				if (PieceTypeAtOffset == 0)
				{
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * i));
					AttackedSquares[BoardSquarePos] = true;
					continue;
				}else
				if (Board::IsPieceColorWhite(PieceType) != Board::IsPieceColorWhite(PieceTypeAtOffset))
				{
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * i));
					AttackedSquares[BoardSquarePos] = true;

					//calculate checking squares
					if ((PieceTypeAtOffset == 14 and isNextMoveForWhite) or (PieceTypeAtOffset == 22 and !isNextMoveForWhite))
					{
						iterator = moves[BoardSquarePos].TargetSquares.end();

						for (int j = 1; - i + j < 0; j++)
						{
							CheckTargetSquares[*(iterator - i + j)] = BoardSquarePos;
						}
					}
					
					//calculate pinnable squares
					for (unsigned int j = i + 1; j <= NumOfSquaresUntilEdge[BoardSquarePos][direction]; j++)
					{
						int PieceTypeAtOffsetBehind = m_BoardSquare[BoardSquarePos + (OffsetForDirections[direction] * j)];
						if (PieceTypeAtOffsetBehind == 0)
						{
							moves[BoardSquarePos].PinnedTargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * j));
							if ((PieceTypeAtOffset == 14 and isNextMoveForWhite) or (PieceTypeAtOffset == 22 and !isNextMoveForWhite))
								PinnedSquaresWithTheKingBeingPinned[BoardSquarePos + (OffsetForDirections[direction] * j)] = true;
							continue;
						}
						else if (Board::IsPieceColorWhite(PieceType) != Board::IsPieceColorWhite(PieceTypeAtOffsetBehind))
						{
							moves[BoardSquarePos].PinnedTargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * j));
							if ((PieceTypeAtOffset == 14 and isNextMoveForWhite) or (PieceTypeAtOffset == 22 and !isNextMoveForWhite))
								PinnedSquaresWithTheKingBeingPinned[BoardSquarePos + (OffsetForDirections[direction] * j)] = true;
							//calculate abs pins
							if ((PieceTypeAtOffsetBehind == 14 and isNextMoveForWhite) or (PieceTypeAtOffsetBehind == 22 and !isNextMoveForWhite))
							{
								absPin_iter = moves[BoardSquarePos].PinnedTargetSquares.end();

								for (unsigned int k = i + 1; k < j; k++)
								{
									WhichBoardSquaresAreAbsPinned[*(absPin_iter - (k - (i + 1)))] = BoardSquarePos;
								}
							}
							break;
						}
					}
					break;
				}
				else
				{
					break;
				}
			}

			//bishop
			if (((PieceType == 18 and isNextMoveForWhite) or (PieceType == 10 and !isNextMoveForWhite)) and direction > 3)
			{
				if (PieceTypeAtOffset == 0)
				{
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * i));
					AttackedSquares[BoardSquarePos] = true;
					continue;
				}
				if (Board::IsPieceColorWhite(PieceType) != Board::IsPieceColorWhite(PieceTypeAtOffset))
				{
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * i));
					AttackedSquares[BoardSquarePos] = true;

					//calculate checking squares
					if ((PieceTypeAtOffset == 14 and isNextMoveForWhite) or (PieceTypeAtOffset == 22 and !isNextMoveForWhite))
					{
						iterator = moves[BoardSquarePos].TargetSquares.end();

						for (int j = 1; -i + j < 0; j++)
						{
							CheckTargetSquares[*(iterator - i + j)] = BoardSquarePos;
						}
					}

					//calculate pinnable squares
					for (unsigned int j = i + 1; j <= NumOfSquaresUntilEdge[BoardSquarePos][direction]; j++)
					{
						int PieceTypeAtOffsetBehind = m_BoardSquare[BoardSquarePos + (OffsetForDirections[direction] * j)];
						if (PieceTypeAtOffsetBehind == 0)
						{
							moves[BoardSquarePos].PinnedTargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * j));

							if ((PieceTypeAtOffset == 14 and isNextMoveForWhite) or (PieceTypeAtOffset == 22 and !isNextMoveForWhite))
								PinnedSquaresWithTheKingBeingPinned[BoardSquarePos + (OffsetForDirections[direction] * j)] = true;
							continue;
						}
						else if (Board::IsPieceColorWhite(PieceType) != Board::IsPieceColorWhite(PieceTypeAtOffsetBehind))
						{
							moves[BoardSquarePos].PinnedTargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * j));
							if ((PieceTypeAtOffset == 14 and isNextMoveForWhite) or (PieceTypeAtOffset == 22 and !isNextMoveForWhite))
								PinnedSquaresWithTheKingBeingPinned[BoardSquarePos + (OffsetForDirections[direction] * j)] = true;
							//calculate abs pins
							if ((PieceTypeAtOffsetBehind == 14 and isNextMoveForWhite) or (PieceTypeAtOffsetBehind == 22 and !isNextMoveForWhite))
							{
								absPin_iter = moves[BoardSquarePos].PinnedTargetSquares.end();

								for (unsigned int k = i + 1; k < j; k++)
								{
									WhichBoardSquaresAreAbsPinned[*(absPin_iter - (k - (i + 1)))] = BoardSquarePos;
								}
							}
							break;
						}
					}
					break;
				}
				else
				{
					break;
				}
			}

			//queen
			if ((PieceType == 21 and isNextMoveForWhite) or (PieceType == 13 and !isNextMoveForWhite))
			{
				if (PieceTypeAtOffset == 0)
				{
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * i));
					AttackedSquares[BoardSquarePos] = true;
					continue;
				}
				if (Board::IsPieceColorWhite(PieceType) != Board::IsPieceColorWhite(PieceTypeAtOffset))
				{
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * i));
					AttackedSquares[BoardSquarePos] = true;

					//calculate checking squares
					if ((PieceTypeAtOffset == 14 and isNextMoveForWhite) or (PieceTypeAtOffset == 22 and !isNextMoveForWhite))
					{
						iterator = moves[BoardSquarePos].TargetSquares.end();

						for (int j = 1; -i + j < 0; j++)
						{
							CheckTargetSquares[*(iterator - i + j)] = BoardSquarePos;
						}
					}

					//calculate pinnable squares
					for (unsigned int j = i + 1; j <= NumOfSquaresUntilEdge[BoardSquarePos][direction]; j++)
					{
						int PieceTypeAtOffsetBehind = m_BoardSquare[BoardSquarePos + (OffsetForDirections[direction] * j)];
						if (PieceTypeAtOffsetBehind == 0)
						{
							moves[BoardSquarePos].PinnedTargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * j));
							if ((PieceTypeAtOffset == 14 and isNextMoveForWhite) or (PieceTypeAtOffset == 22 and !isNextMoveForWhite))
								PinnedSquaresWithTheKingBeingPinned[BoardSquarePos + (OffsetForDirections[direction] * j)] = true;
							continue;
						}
						else if (Board::IsPieceColorWhite(PieceType) != Board::IsPieceColorWhite(PieceTypeAtOffsetBehind))
						{
							moves[BoardSquarePos].PinnedTargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * j));
							if ((PieceTypeAtOffset == 14 and isNextMoveForWhite) or (PieceTypeAtOffset == 22 and !isNextMoveForWhite))
								PinnedSquaresWithTheKingBeingPinned[BoardSquarePos + (OffsetForDirections[direction] * j)] = true;
							//calculate abs pins
							if ((PieceTypeAtOffsetBehind == 14 and isNextMoveForWhite) or (PieceTypeAtOffsetBehind == 22 and !isNextMoveForWhite))
							{
								absPin_iter = moves[BoardSquarePos].PinnedTargetSquares.end();

								for (unsigned int k = i + 1; k < j; k++)
								{
									WhichBoardSquaresAreAbsPinned[*(absPin_iter - (k - (i + 1)))] = BoardSquarePos;
								}
							}
							break;
						}

					}
					break;
				}
				else
				{
					break;
				}
			}

		}
	}
}

//knight
void GenerateLegalMoves::KnightMoveGen(int BoardSquarePos, bool isNextMoveForWhite)
{
	if ((m_BoardSquare[BoardSquarePos] == 19 and isNextMoveForWhite) or (m_BoardSquare[BoardSquarePos] == 11 and !isNextMoveForWhite))
	{
		CreateOffesetsForKnight(BoardSquarePos);
		for (int i : OffsetsForKnight)
		{
			//i know that if only the first condition is met then c++ won't check the other one, if it does i'm blaming c++ and i WILL be calling it stupid
			if (((int)m_BoardSquare[i]) == 0 or (Board::IsPieceColorWhite(m_BoardSquare[BoardSquarePos]) != Board::IsPieceColorWhite(m_BoardSquare[i])))
			{
				moves[BoardSquarePos].PieceType = m_BoardSquare[BoardSquarePos];
				moves[BoardSquarePos].TargetSquares.push_back(i);
				AttackedSquares[BoardSquarePos] = true;
			}
			
		}
		OffsetsForKnight.resize(0);
	}
}

void GenerateLegalMoves::CreateOffesetsForKnight(int BoardSquarePos)
{
	for (int i = 0; i <= 4; i++)
	{
		if (NumOfSquaresUntilEdge[BoardSquarePos][i] >= 1)
		{
			if (i == 0 and BoardSquarePos <= 47)
			{
				if (NumOfSquaresUntilEdge[BoardSquarePos + 16][2] >= 1)
					OffsetsForKnight.push_back(BoardSquarePos + 15);
				if (NumOfSquaresUntilEdge[BoardSquarePos + 16][3] >= 1)
					OffsetsForKnight.push_back(BoardSquarePos + 17);
			}
			else if (i == 1 and BoardSquarePos >= 16)
			{
				if (NumOfSquaresUntilEdge[BoardSquarePos - 16][2] >= 1)
					OffsetsForKnight.push_back(BoardSquarePos - 15);
				if (NumOfSquaresUntilEdge[BoardSquarePos - 16][3] >= 1)
					OffsetsForKnight.push_back(BoardSquarePos - 17);
			}
			if (NumOfSquaresUntilEdge[BoardSquarePos][2] >= 2)
			{
				if (i == 2 and BoardSquarePos >= 2)
				{
					if (NumOfSquaresUntilEdge[BoardSquarePos - 2][0] >= 1)
						OffsetsForKnight.push_back(BoardSquarePos + 6);
					if (NumOfSquaresUntilEdge[BoardSquarePos - 2][1] >= 1)
						OffsetsForKnight.push_back(BoardSquarePos - 10);
				}
			}
			if (NumOfSquaresUntilEdge[BoardSquarePos][3] >= 2)
			{
				if (i == 3 and BoardSquarePos <= 61)
				{
					if (NumOfSquaresUntilEdge[BoardSquarePos + 2][0] >= 1)
						OffsetsForKnight.push_back(BoardSquarePos + 10);
					if (NumOfSquaresUntilEdge[BoardSquarePos + 2][1] >= 1)
						OffsetsForKnight.push_back(BoardSquarePos - 6);
				}
			}
		}
	}
}

//pawn
void GenerateLegalMoves::PawnMoveGen(int BoardSquarePos, bool isNextMoveForWhite)
{
	unsigned int PieceType = m_BoardSquare[BoardSquarePos];
	if (PieceType == 17 and isNextMoveForWhite)//white pawn
	{
		for (int Offset : OffsetForWhitePawn)
		{
			unsigned int BoardPosPlusOffset = BoardSquarePos + Offset;
			unsigned int PieceTypeAtOffset = m_BoardSquare[BoardPosPlusOffset];
			int directions[3] = { 4,0,6 };

			if (NumOfSquaresUntilEdge[BoardSquarePos][directions[Offset-7]] != 0)
			{
				if (PieceTypeAtOffset != 0 and !Board::IsPieceColorWhite(PieceTypeAtOffset) and Offset != 8)
				{
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].TargetSquares.push_back(BoardPosPlusOffset);
					AttackedSquares[BoardSquarePos] = true;
				}
				else if (Offset == 8 and PieceTypeAtOffset == 0)
				{
					if (m_BoardSquare[BoardSquarePos + 16] == 0 and BoardSquarePos <= 16 and BoardSquarePos >= 8)
					{
						moves[BoardSquarePos].PieceType = PieceType;
						moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + 16);
					}
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].TargetSquares.push_back(BoardPosPlusOffset);
				}
				else if (PieceTypeAtOffset == 0 and Offset != 8)
				{
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].AttackableSquares.push_back(BoardPosPlusOffset);
					AttackedSquares[BoardSquarePos] = true;
				}


				//en passant
				if (BoardSquarePos <= 40 and BoardSquarePos >= 33 and Offset != 8 and PieceTypeAtOffset == 0 and !m_previousBoardSquare.empty() and (m_previousBoardSquare[BoardSquarePos + (Offset * 2)]) == 9 and (m_BoardSquare[BoardSquarePos + Offset - 8]) == 9)
				{
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].TargetSquares.push_back(BoardPosPlusOffset);
				}
			}
		}
	}
	if (PieceType == 9 and !isNextMoveForWhite)//black pawn
	{
		for (int Offset : OffsetForBlackPawn)
		{
			unsigned int BoardPosPlusOffset = BoardSquarePos + Offset;
			unsigned int PieceTypeAtOffset = m_BoardSquare[BoardPosPlusOffset];
			int directions[3] = { 7,1,5 };

			if (NumOfSquaresUntilEdge[BoardSquarePos][directions[Offset+9]] != 0)
			{
				if (PieceTypeAtOffset != 0 and Board::IsPieceColorWhite(PieceTypeAtOffset) and Offset != -8)
				{
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].TargetSquares.push_back(BoardPosPlusOffset);
					AttackedSquares[BoardSquarePos] = true;
				}
				else if (Offset == -8 and PieceTypeAtOffset == 0)
				{
					if (m_BoardSquare[BoardSquarePos - 16] == 0 and BoardSquarePos <= 55 and BoardSquarePos >= 48)
					{
						moves[BoardSquarePos].PieceType = PieceType;
						moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos - 16);
					}
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].TargetSquares.push_back(BoardPosPlusOffset);
				}
				else if (PieceTypeAtOffset == 0 and Offset != -8)
				{
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].AttackableSquares.push_back(BoardPosPlusOffset);
					AttackedSquares[BoardSquarePos] = true;
				}
				//en passant
				if (BoardSquarePos <= 32 and BoardSquarePos >= 25 and Offset != -8 and PieceTypeAtOffset == 0 and !m_previousBoardSquare.empty() and (m_previousBoardSquare[BoardSquarePos + (Offset * 2)]) == 17 and (m_BoardSquare[BoardSquarePos + Offset + 8]) == 17)
				{
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].TargetSquares.push_back(BoardPosPlusOffset);
				}
			}
		}
	}
}

//king
void GenerateLegalMoves::KingMoveGen(int BoardSquarePos, bool isNextMoveForWhite)
{
	if ((m_BoardSquare[BoardSquarePos] == 22 and isNextMoveForWhite) or (m_BoardSquare[BoardSquarePos] == 14 and !isNextMoveForWhite))
	{
		unsigned int PieceType = m_BoardSquare[BoardSquarePos];
		for (unsigned int direction = 0; direction < 8; direction++)
		{
			if (NumOfSquaresUntilEdge[BoardSquarePos][direction] > 0)
			{
				if (m_BoardSquare[BoardSquarePos + OffsetForDirections[direction]] == 0)
				{
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + OffsetForDirections[direction]);
					AttackedSquares[BoardSquarePos] = true;
					continue;
				}
				if (Board::IsPieceColorWhite(PieceType) != Board::IsPieceColorWhite(m_BoardSquare[BoardSquarePos + OffsetForDirections[direction]]))
				{
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + OffsetForDirections[direction]);
					AttackedSquares[BoardSquarePos] = true;
				}
			}
		}
		if (PieceType == 22 and !CanCastle.HasWhiteKingMoved)
		{
			if (!CanCastle.HasWhiteLongRookMoved and m_BoardSquare[1, 2, 3] == 0)
			{
				moves[4].TargetSquares.push_back(2);
				moves[4].Castle = 1;
			}
			if (!CanCastle.HasWhiteLongRookMoved and m_BoardSquare[5, 6] == 0)
			{
				moves[4].TargetSquares.push_back(6);
				moves[4].Castle = 2;
			}
		}
		if (PieceType == 14 and !CanCastle.HasBlackKingMoved)
		{
			if (!CanCastle.HasBlackLongRookMoved and m_BoardSquare[57, 58, 59] == 0)
			{
				moves[60].TargetSquares.push_back(58);
				moves[60].Castle = 3;
			}
			if (!CanCastle.HasBlackLongRookMoved and m_BoardSquare[61, 62] == 0)
			{
				moves[60].TargetSquares.push_back(62);
				moves[60].Castle = 4;
			}
		}
	}
}

//sidenote: this is only for x-ray moves, i have to work out the knight and pawn
void GenerateLegalMoves::RemoveIllegalMoves_Checks_AbsolutePins(bool isNextMoveForWhite)
{	
	
	std::vector<unsigned int> TargetSquaresThatAreChecking;
	std::vector<unsigned int> PinnedTargetSquaresThatAreAbsolutePinning;
	std::vector<unsigned int> SquareWhichTargetSquaresThatAreChecking;
	std::vector<unsigned int> SquareWhichPinnedTargetSquaresThatAreAbsolutePinning;//peak of variable names
	unsigned int BoardSquareOfAttackedKing = 0;
	bool isKingUnderCheck = false;

	if (isNextMoveForWhite)
	{
		for (BoardSquareOfAttackedKing = 0; BoardSquareOfAttackedKing < 64; BoardSquareOfAttackedKing++)
		{
			if (m_BoardSquare[BoardSquareOfAttackedKing] == 22)
				break;
		}

	}
	else
	{
		for (BoardSquareOfAttackedKing = 0; BoardSquareOfAttackedKing < 64; BoardSquareOfAttackedKing++)
		{
			if (m_BoardSquare[BoardSquareOfAttackedKing] == 14)
				break;
		}
	}

	//ouch, could be optimized, but i'm not gonna use just one thread for the whole project, so yeah, fuck it
	GenerateLegalMoves OppositeMoves(m_BoardSquare, CanCastle, !isNextMoveForWhite, MoveNum, true);

	//fill the vectors
	uint8_t count = 0;
	bool Has1CheckBeenFound = 0;
	for (unsigned int Square : CheckTargetSquares)
	{
		if (Square != 0)
		{
			if (Has1CheckBeenFound)
			{
				SquareWhichTargetSquaresThatAreChecking.push_back(Square);
				break;
			}
			Has1CheckBeenFound = true;
			SquareWhichTargetSquaresThatAreChecking.push_back(Square);
		}

		count++;
	}

	/*
	if (SquareWhichTargetSquaresThatAreChecking.size() == 1)
	{
		for (unsigned int j = 0; j < 64; j++)
		{
			bool isPiecePinned = false;
			for (unsigned int l : SquareWhichPinnedTargetSquaresThatAreAbsolutePinning)
			{
				if (std::find(moves[l].TargetSquares.begin(), moves[l].TargetSquares.end(), j) != moves[l].TargetSquares.end())
					isPiecePinned = true;
			}
			
			if (!isPiecePinned)
			{
				for (unsigned int i : TargetSquaresThatAreChecking)
				{

					for (unsigned int k : moves[j].TargetSquares)
					{
						if ((k == i and k != BoardSquareOfAttackedKing) or k == SquareWhichTargetSquaresThatAreChecking[0])
						{
							LegalMoves[j].TargetSquares.push_back(k);
							isItCheckmate = false;
						}
					}
				}
				//if king and piece are next to each other
				if (TargetSquaresThatAreChecking.empty())
				{
					for (unsigned int k : moves[j].TargetSquares)
					{
						if (k == SquareWhichTargetSquaresThatAreChecking[0])
						{
							LegalMoves[j].TargetSquares.push_back(k);
							isItCheckmate = false;
						}
					}
				}
				//moves that make the king 'run away'
				//optimization sidenote: std::find appears really slow
				for (unsigned int KingMove : moves[BoardSquareOfAttackedKing].TargetSquares)
				{
					if (std::find(OppositeMoves.moves[SquareWhichTargetSquaresThatAreChecking[0]].TargetSquares.begin(), OppositeMoves.moves[SquareWhichTargetSquaresThatAreChecking[0]].TargetSquares.end(), KingMove) == OppositeMoves.moves[SquareWhichTargetSquaresThatAreChecking[0]].TargetSquares.end())
					{
						if (std::find(OppositeMoves.moves[SquareWhichTargetSquaresThatAreChecking[0]].PinnedTargetSquares.begin(), OppositeMoves.moves[SquareWhichTargetSquaresThatAreChecking[0]].PinnedTargetSquares.end(), KingMove) == OppositeMoves.moves[SquareWhichTargetSquaresThatAreChecking[0]].PinnedTargetSquares.end())
						{
							LegalMoves[BoardSquareOfAttackedKing].TargetSquares.push_back(KingMove);
							isItCheckmate = false;
						}
					}
				}
			}
		}
	}

	if (SquareWhichTargetSquaresThatAreChecking.size() >= 1)
	{
		for (unsigned int KingMove : moves[BoardSquareOfAttackedKing].TargetSquares)
		{
			for (unsigned int j : SquareWhichTargetSquaresThatAreChecking)
			{
				for (unsigned int PieceMove : OppositeMoves.moves[j].TargetSquares)
				{
					if (KingMove != PieceMove)
					{
						LegalMoves[BoardSquareOfAttackedKing].TargetSquares.push_back(j);
						isItCheckmate = false;
					}
				}
			}
		}
	}*/

	//todo: this new algorithm works, but i believe there might be problems in RenderChesspcs, especially when dropping a piece
	//possible overhaul of how Legal moves are calculated 
	for (unsigned int KingMove : moves[BoardSquareOfAttackedKing].TargetSquares)
	{
		//checks if possible move square is not under attack
		if (OppositeMoves.AttackedSquares[KingMove] == true)
		{
			//checks if possible move is not going to be under attack when it's made
			if (OppositeMoves.PinnedSquaresWithTheKingBeingPinned[KingMove] == false)
			{
				LegalMoves[BoardSquareOfAttackedKing].TargetSquares.push_back(KingMove);
				isItCheckmate = false;
			}
		}
	}
	if (SquareWhichTargetSquaresThatAreChecking.size() == 1)
	{
		count = 0;
		for (MOVE Piece : moves)
		{
			if (Piece.PieceType == 22 or Piece.PieceType == 14)
				continue;
			//checks if not under abs pin
			if (OppositeMoves.WhichBoardSquaresAreAbsPinned[count] == 0)
			{
				for (unsigned int Move : Piece.TargetSquares)
				{
					//checks if Move captures piece or blocks check
					if (Move == SquareWhichTargetSquaresThatAreChecking[0] or CheckTargetSquares[Move] == SquareWhichTargetSquaresThatAreChecking[0])
					{
						LegalMoves[count].TargetSquares.push_back(Move);
						isItCheckmate = false;
					}
				}
			}

			count++;
		}
	}
	
}

bool GenerateLegalMoves::isA64ArrayEmpty(std::array<MOVE, 64> Array)
{
	for (MOVE i : Array)
	{
		if (!i.TargetSquares.empty())
		{
			return false;
		}
	}
	return true;
}


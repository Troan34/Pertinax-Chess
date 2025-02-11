#include "LegalMoves.h"
static std::array<uint8_t, 64> m_previousBoardSquare;
static std::array<MOVE, 64> OppositeMoves;

GenerateLegalMoves::GenerateLegalMoves(const std::array<uint8_t, 64Ui64>& BoardSquare, const std::array<uint8_t, 64>* previousBoardSquare, canCastle CanCastle, bool isNextMoveForWhite, unsigned int MoveNum, bool isForOppositeMoves)
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
	if (!isForOppositeMoves)
	{
		RemoveIllegalMoves(isNextMoveForWhite);
		if (isItCheckmate)
		{
			//std::cout << "Checkmate" << '\n';
		}
	}
	if(previousBoardSquare != nullptr)
		m_previousBoardSquare = *previousBoardSquare;
}

GenerateLegalMoves::~GenerateLegalMoves()
{

}

void GenerateLegalMoves::GenerateMoves(bool isNextMoveForWhite)
{
	AttackedSquares.fill(false);
	PinnedSquaresWithTheKingBeingPinned.fill(false);
	WhichBoardSquaresAreAbsPinned.fill(0);
	uint8_t BoardSquarePos = 0;
	for (uint8_t i : m_BoardSquare)
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
 
void GenerateLegalMoves::SliderMoveGen(const uint8_t& BoardSquarePos, bool isNextMoveForWhite)
{
	uint8_t PieceType = m_BoardSquare[BoardSquarePos];
	std::vector<uint8_t>::iterator iterator;
	std::vector<uint8_t>::iterator absPin_iter;

	for (uint8_t direction = 0; direction < 8; direction++)
	{
		for (uint8_t i = 1; i <= NumOfSquaresUntilEdge[BoardSquarePos][direction]; i++)
		{
			uint8_t PieceTypeAtOffset = m_BoardSquare[BoardSquarePos + (OffsetForDirections[direction] * i)];
			//rook
			if (((PieceType == 20 and isNextMoveForWhite) or (PieceType == 12 and !isNextMoveForWhite)) and direction <= 3)
			{
				AttackedSquares[BoardSquarePos + (OffsetForDirections[direction] * i)] = true;

				if (PieceTypeAtOffset == 0)
				{
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * i));
					continue;
				}else
				if (Board::IsPieceColorWhite(PieceType) != Board::IsPieceColorWhite(PieceTypeAtOffset))
				{
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * i));

					//calculate checking squares
					if ((PieceTypeAtOffset == 14 and isNextMoveForWhite) or (PieceTypeAtOffset == 22 and !isNextMoveForWhite))
					{
						iterator = moves[BoardSquarePos].TargetSquares.end();

						for (int j = 0; - i + j < 0; j++)
						{
							CheckTargetSquares[*(iterator - i + j)] = BoardSquarePos;
						}
					}
					
					//calculate pinnable squares
					for (uint8_t j = i + 1; j <= NumOfSquaresUntilEdge[BoardSquarePos][direction]; j++)
					{
						uint8_t PieceTypeAtOffsetBehind = m_BoardSquare[BoardSquarePos + (OffsetForDirections[direction] * j)];
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
								absPin_iter = moves[BoardSquarePos].TargetSquares.end();

								bool DoNotRun = false;
								bool Flag = false;
								for (uint8_t k = 0; k < i; k++)
								{
									if (m_BoardSquare[*(absPin_iter - k - 1)] != 0)
									{
										if (!Flag)
											Flag = true;
										else
										{
											DoNotRun = true;
										}
									}
								}

								if (!DoNotRun)
								{
									WhichBoardSquaresAreAbsPinned[BoardSquarePos + (OffsetForDirections[direction] * i)] = BoardSquarePos;
									for (uint8_t k = 0; k < i; k++)
									{
										WhichBoardSquaresAreAbsPinned[*(absPin_iter - k - 1)] = BoardSquarePos;
									}
									DoNotRun = false;
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
				AttackedSquares[BoardSquarePos + (OffsetForDirections[direction] * i)] = true;
				if (PieceTypeAtOffset == 0)
				{
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * i));
					continue;
				}
				if (Board::IsPieceColorWhite(PieceType) != Board::IsPieceColorWhite(PieceTypeAtOffset))
				{
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * i));

					//calculate checking squares
					if ((PieceTypeAtOffset == 14 and isNextMoveForWhite) or (PieceTypeAtOffset == 22 and !isNextMoveForWhite))
					{
						iterator = moves[BoardSquarePos].TargetSquares.end();

						for (int j = 0; -i + j < 0; j++)
						{
							CheckTargetSquares[*(iterator - i + j)] = BoardSquarePos;
						}
					}

					//calculate pinnable squares
					for (uint8_t j = i + 1; j <= NumOfSquaresUntilEdge[BoardSquarePos][direction]; j++)
					{
						uint8_t PieceTypeAtOffsetBehind = m_BoardSquare[BoardSquarePos + (OffsetForDirections[direction] * j)];
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
								absPin_iter = moves[BoardSquarePos].TargetSquares.end();

								bool DoNotRun = false;
								bool Flag = false;
								for (uint8_t k = 0; k < i; k++)
								{
									if (m_BoardSquare[*(absPin_iter - k - 1)] != 0)
									{
										if (!Flag)
											Flag = true;
										else
										{
											DoNotRun = true;
										}
									}
								}

								if (!DoNotRun)
								{
									WhichBoardSquaresAreAbsPinned[BoardSquarePos + (OffsetForDirections[direction] * i)] = BoardSquarePos;
									for (uint8_t k = 0; k < i; k++)
									{
										WhichBoardSquaresAreAbsPinned[*(absPin_iter - k - 1)] = BoardSquarePos;
									}
									DoNotRun = false;
								}
							}
							break;
						}
					}
					break;
				}
				else
					break;
			}

			//queen
			if ((PieceType == 21 and isNextMoveForWhite) or (PieceType == 13 and !isNextMoveForWhite))
			{
				AttackedSquares[BoardSquarePos + (OffsetForDirections[direction] * i)] = true;
				if (PieceTypeAtOffset == 0)
				{
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * i));
					continue;
				}
				if (Board::IsPieceColorWhite(PieceType) != Board::IsPieceColorWhite(PieceTypeAtOffset))
				{
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * i));

					//calculate checking squares
					if ((PieceTypeAtOffset == 14 and isNextMoveForWhite) or (PieceTypeAtOffset == 22 and !isNextMoveForWhite))
					{
						iterator = moves[BoardSquarePos].TargetSquares.end();

						for (int index = 0; -i + index < 0; index++)
						{
							CheckTargetSquares[*(iterator - i + index)] = BoardSquarePos;
						}
					}

					//calculate pinnable squares
					for (uint8_t j = i + 1; j <= NumOfSquaresUntilEdge[BoardSquarePos][direction]; j++)
					{
						uint8_t PieceTypeAtOffsetBehind = m_BoardSquare[BoardSquarePos + (OffsetForDirections[direction] * j)];

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
								absPin_iter = moves[BoardSquarePos].TargetSquares.end();

								bool DoNotRun = false;
								bool Flag = false;
								for (uint8_t k = 0; k < i; k++)
								{
									if (m_BoardSquare[*(absPin_iter - k - 1)] != 0)
									{
										if (!Flag)
											Flag = true;
										else
										{
											DoNotRun = true;
										}
									}
								}

								if (!DoNotRun)
								{
									WhichBoardSquaresAreAbsPinned[BoardSquarePos + (OffsetForDirections[direction] * i)] = BoardSquarePos;
									for (uint8_t k = 0; k < i; k++)
									{
										WhichBoardSquaresAreAbsPinned[*(absPin_iter - k - 1)] = BoardSquarePos;
									}
									DoNotRun = false;
								}
							}
							break;
						}

					}
					break;
				}
				else
					break;
			}

		}
	}
}

//knight
void GenerateLegalMoves::KnightMoveGen(const uint8_t& BoardSquarePos, bool isNextMoveForWhite)
{
	if ((m_BoardSquare[BoardSquarePos] == 19 and isNextMoveForWhite) or (m_BoardSquare[BoardSquarePos] == 11 and !isNextMoveForWhite))
	{
		CreateOffesetsForKnight(BoardSquarePos);
		for (int8_t i : OffsetsForKnight)
		{
			AttackedSquares[i] = true;
			//i know that if only the first condition is met then c++ won't check the other one, if it does i'm blaming c++ and i WILL be calling it stupid
			if ((m_BoardSquare[i]) == 0 or (Board::IsPieceColorWhite(m_BoardSquare[BoardSquarePos]) != Board::IsPieceColorWhite(m_BoardSquare[i])))
			{
				moves[BoardSquarePos].PieceType = m_BoardSquare[BoardSquarePos];
				moves[BoardSquarePos].TargetSquares.push_back(i);
				if (m_BoardSquare[i] == 14 and isNextMoveForWhite or m_BoardSquare[i] == 22 and !isNextMoveForWhite)
				{
					CheckTargetSquares[BoardSquarePos] = BoardSquarePos;
				}
			}
			
		}
		OffsetsForKnight.resize(0);
	}
}

void GenerateLegalMoves::CreateOffesetsForKnight(const uint8_t& BoardSquarePos)
{
	for (uint8_t i = 0; i <= 4; i++)
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
					OffsetsForKnight.push_back(BoardSquarePos - 17);
				if (NumOfSquaresUntilEdge[BoardSquarePos - 16][3] >= 1)
					OffsetsForKnight.push_back(BoardSquarePos - 15);
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
void GenerateLegalMoves::PawnMoveGen(const uint8_t& BoardSquarePos, bool isNextMoveForWhite)
{
	uint8_t PieceType = m_BoardSquare[BoardSquarePos];
	if (PieceType == 17 and isNextMoveForWhite)//white pawn
	{
		for (int Offset : OffsetForWhitePawn)
		{
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
					}
					if (PieceTypeAtOffset == 14)
						CheckTargetSquares[BoardPosPlusOffset] = BoardSquarePos;

					//promotion
					if (BoardPosPlusOffset >= 56)
						moves[BoardSquarePos].Promotion[Offset - 7] = BoardPosPlusOffset;
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
				if (BoardSquarePos <= 39 and BoardSquarePos >= 32 and Offset != 8 and PieceTypeAtOffset == 0 and !m_previousBoardSquare.empty() and (m_previousBoardSquare[BoardSquarePos + (Offset + 8)]) == 9 and (m_BoardSquare[BoardSquarePos + Offset - 8]) == 9)
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
					}
					if (PieceTypeAtOffset == 22)
						CheckTargetSquares[BoardPosPlusOffset] = BoardSquarePos;

					//promotion
					if (BoardPosPlusOffset <= 7)
						moves[BoardSquarePos].Promotion[Offset + 9] = BoardPosPlusOffset;
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
				if (BoardSquarePos <= 31 and BoardSquarePos >= 24 and Offset != -8 and PieceTypeAtOffset == 0 and !m_previousBoardSquare.empty() and (m_previousBoardSquare[BoardSquarePos + (Offset - 8)]) == 17 and (m_BoardSquare[BoardSquarePos + Offset + 8]) == 17)
				{
					moves[BoardSquarePos].PieceType = PieceType;
					moves[BoardSquarePos].TargetSquares.push_back(BoardPosPlusOffset);
				}
			}
		}
	}
}

//king
void GenerateLegalMoves::KingMoveGen(const uint8_t& BoardSquarePos, bool isNextMoveForWhite)
{
	if ((m_BoardSquare[BoardSquarePos] == 22 and isNextMoveForWhite) or (m_BoardSquare[BoardSquarePos] == 14 and !isNextMoveForWhite))
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
		if (PieceType == 22 and !CanCastle.HasWhiteKingMoved)//castling
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
		if (PieceType == 14 and !CanCastle.HasBlackKingMoved)//castling
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

void GenerateLegalMoves::RemoveIllegalMoves(bool isNextMoveForWhite)
{	
	std::vector<uint8_t> SquareWhichTargetSquaresThatAreChecking;
	uint8_t BoardSquareOfKingToMove = 0;

	if (isNextMoveForWhite)
	{
		for (BoardSquareOfKingToMove = 0; BoardSquareOfKingToMove < 64; BoardSquareOfKingToMove++)
		{
			if (m_BoardSquare[BoardSquareOfKingToMove] == 22)
				break;
		}

	}
	else
	{
		for (BoardSquareOfKingToMove = 0; BoardSquareOfKingToMove < 64; BoardSquareOfKingToMove++)
		{
			if (m_BoardSquare[BoardSquareOfKingToMove] == 14)
				break;
		}
	}

	
	auto p_prevBoardSquare = &m_previousBoardSquare;
	if (MoveNum == 0)
		p_prevBoardSquare = nullptr;

	GenerateLegalMoves OppositeMoves(m_BoardSquare,p_prevBoardSquare, CanCastle, !isNextMoveForWhite, MoveNum, true);


	//fill SquareWhichTargetSquaresThatAreChecking
	uint8_t count = 0;
	uint8_t CheckingSquare = 0;
	for (uint16_t Square : OppositeMoves.CheckTargetSquares)
	{
		if (Square != 0)
		{
			if (CheckingSquare == Square)
			{
				continue;
			}
			else
			{
				SquareWhichTargetSquaresThatAreChecking.push_back(Square);
				CheckingSquare = Square;
			}
		}
	}

	//moves for King
	if (SquareWhichTargetSquaresThatAreChecking.size() >= 0)
	{
		//for (unsigned int KingMove : moves[BoardSquareOfKingToMove].TargetSquares)
		for(auto it = moves[BoardSquareOfKingToMove].TargetSquares.begin(); it != moves[BoardSquareOfKingToMove].TargetSquares.end();)
		{
			uint8_t KingMove = *it;
			//checks if possible move square is under attack
			if (OppositeMoves.AttackedSquares[KingMove] == true)
			{
				it = moves[BoardSquareOfKingToMove].TargetSquares.erase(it);
				continue;
			}
			//checks if possible move is going to be under attack when it's made
			else if (OppositeMoves.PinnedSquaresWithTheKingBeingPinned[KingMove] == true)
			{
				it = moves[BoardSquareOfKingToMove].TargetSquares.erase(it);
				continue;
			}
			//checks that it can castle
			else if (BoardSquareOfKingToMove - KingMove == 2 or BoardSquareOfKingToMove - KingMove == -2)//checked for weird behaviours, rvalue(i think)
			{
				CanKingCastle_LMoves(OppositeMoves, isItCheckmate, it, BoardSquareOfKingToMove, KingMove);
			}
			else
			{
				isItCheckmate = false;
				++it;
			}
		}
	}
	//Moves for every piece while check
	if (SquareWhichTargetSquaresThatAreChecking.size() == 1)
	{
		count = 0;
		for (MOVE Piece : moves)
		{//this Piece.PieceType == 0 might be risky, but only if piece has moves and Piecetype = 0 is either no piece or no moves for the piece
			if (Piece.PieceType == 22 or Piece.PieceType == 14 or Piece.PieceType == 0)
			{
				count++;
				continue;
			}

			//checks if under abs pin
			if (OppositeMoves.WhichBoardSquaresAreAbsPinned[count] != 0)
			{
				moves[count].TargetSquares.clear();
			}
			else
			{
				int16_t IterCount = 0;
				for (unsigned int Move : Piece.TargetSquares)
				{
					//checks if Move neither captures piece and doesn't block check
					if (Move != SquareWhichTargetSquaresThatAreChecking[0] and OppositeMoves.CheckTargetSquares[Move] != SquareWhichTargetSquaresThatAreChecking[0])
					{
						moves[count].TargetSquares.erase(moves[count].TargetSquares.begin() + IterCount);
						continue;
					}
					else
					{
						isItCheckmate = false;
					}
					IterCount++;
				}
			}

			count++;
		}
	}

	//moves for every piece while no checks
	if (SquareWhichTargetSquaresThatAreChecking.size() == 0)
	{
		count = 0;
		for (MOVE Piece : moves)
		{//this Piece.PieceType == 0 might be risky, but only if piece has moves and Piecetype = 0 is either no piece or no moves for the piece
			if (Piece.PieceType == 22 or Piece.PieceType == 14 or Piece.PieceType == 0)
			{
				count++;
				continue;
			}

			if (OppositeMoves.WhichBoardSquaresAreAbsPinned[count] != 0)
			{
				int16_t IterCount = 0;
				for (uint8_t Move : Piece.TargetSquares)
				{
					
					if (OppositeMoves.WhichBoardSquaresAreAbsPinned[count] != OppositeMoves.WhichBoardSquaresAreAbsPinned[Move] and OppositeMoves.WhichBoardSquaresAreAbsPinned[count] != Move)
					{
						moves[count].TargetSquares.erase(moves[count].TargetSquares.begin() + IterCount);
						continue;
					}
					else
					{
						isItCheckmate = false;
					}
					IterCount++;
				}
			}
			else
			{
				isItCheckmate = false;
			}
			count++;
		}
	}

	SquareWhichTargetSquaresThatAreChecking.clear();
	
}

//fun for LegalMoves, checks if castling is permitted
void GenerateLegalMoves::CanKingCastle_LMoves(const GenerateLegalMoves& OppositeMoves, bool& isItCheckmate, std::vector<uint8_t>::iterator& it, const uint8_t& BoardSquareOfKingToMove, const uint8_t& KingMove)
{
	if (Board::IsPieceColorWhite(m_BoardSquare[BoardSquareOfKingToMove]))
	{
		if (BoardSquareOfKingToMove - KingMove == 2)
		{
			if (OppositeMoves.AttackedSquares[4] != false or OppositeMoves.AttackedSquares[3] != false or OppositeMoves.AttackedSquares[2] != false)//checked for weird behaviours
			{
				it = moves[BoardSquareOfKingToMove].TargetSquares.erase(it);
			}
			else
			{
				isItCheckmate = false;
				++it;
			}
		}
		else if (BoardSquareOfKingToMove - KingMove == -2)
		{
			if (OppositeMoves.AttackedSquares[4] != false or OppositeMoves.AttackedSquares[5] != false or OppositeMoves.AttackedSquares[6] != false)
			{
				it = moves[BoardSquareOfKingToMove].TargetSquares.erase(it);
			}
			else
			{
				isItCheckmate = false;
				++it;
			}
		}
	}
	else
	{
		if (BoardSquareOfKingToMove - KingMove == 2)
		{
			if (OppositeMoves.AttackedSquares[58] != false or OppositeMoves.AttackedSquares[59] != false or OppositeMoves.AttackedSquares[60] != false)//checked for weird behaviours
			{
				it = moves[BoardSquareOfKingToMove].TargetSquares.erase(it);
			}
			else
			{
				isItCheckmate = false;
				++it;
			}
		}
		else if (BoardSquareOfKingToMove - KingMove == -2)
		{
			if (OppositeMoves.AttackedSquares[60] != false or OppositeMoves.AttackedSquares[61] != false or OppositeMoves.AttackedSquares[62] != false)
			{
				it = moves[BoardSquareOfKingToMove].TargetSquares.erase(it);
			}
			else
			{
				isItCheckmate = false;
				++it;
			}
		}
	}
}

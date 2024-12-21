#include "LegalMoves.h"
static std::array<unsigned int, 64> m_previousBoardSquare;
GenerateLegalMoves::GenerateLegalMoves(std::array<unsigned int, 64Ui64> BoardSquare, std::array<unsigned int, 64> previousBoardSquare, canCastle CanCastle, bool isNextMoveForWhite)
	:moves(), m_BoardSquare(BoardSquare), CanCastle(CanCastle)
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
	m_previousBoardSquare = previousBoardSquare;
}

GenerateLegalMoves::GenerateLegalMoves(std::array<unsigned int, 64Ui64> BoardSquare, canCastle CanCastle, bool isNextMoveForWhite)
	:moves(), m_BoardSquare(BoardSquare), CanCastle(CanCastle)
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
					//moves[BoardSquarePos].PieceType = PieceType + (OffsetForDirections[direction] * i);
					moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * i));
					continue;
				}else
				if (Board::IsPieceColorWhite(PieceType) != Board::IsPieceColorWhite(PieceTypeAtOffset))
				{
					//moves[BoardSquarePos].PieceType = PieceType + (OffsetForDirections[direction] * i);
					moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * i));
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
					//moves[BoardSquarePos].PieceType = PieceType + (OffsetForDirections[direction] * i);
					moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * i));
					continue;
				}
				if (Board::IsPieceColorWhite(PieceType) != Board::IsPieceColorWhite(PieceTypeAtOffset))
				{
					//moves[BoardSquarePos].PieceType = PieceType + (OffsetForDirections[direction] * i);
					moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * i));
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
					//moves[BoardSquarePos].PieceType = PieceType + (OffsetForDirections[direction] * i);
					moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * i));
					continue;
				}
				if (Board::IsPieceColorWhite(PieceType) != Board::IsPieceColorWhite(PieceTypeAtOffset))
				{
					//moves[BoardSquarePos].PieceType = PieceType + (OffsetForDirections[direction] * i);
					moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + (OffsetForDirections[direction] * i));
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
				//moves[BoardSquarePos].PieceType = m_BoardSquare[BoardSquarePos] + i;
				moves[BoardSquarePos].TargetSquares.push_back(i);
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
					//moves[BoardSquarePos].PieceType = PieceTypeAtOffset;
					moves[BoardSquarePos].TargetSquares.push_back(BoardPosPlusOffset);
				}
				if (Offset == 8 and PieceTypeAtOffset == 0)
				{
					if (m_BoardSquare[BoardSquarePos + 16] == 0 and BoardSquarePos <= 16 and BoardSquarePos >= 8)
					{
						//moves[BoardSquarePos].PieceType = m_BoardSquare[BoardSquarePos + 16];
						moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + 16);
					}
					//moves[BoardSquarePos].PieceType = PieceTypeAtOffset;
					moves[BoardSquarePos].TargetSquares.push_back(BoardPosPlusOffset);
				}
				//en passant
				if (BoardSquarePos <= 40 and BoardSquarePos >= 33 and Offset != 8 and PieceTypeAtOffset == 0 and !m_previousBoardSquare.empty() and (m_previousBoardSquare[BoardSquarePos + (Offset * 2)]) == 9 and (m_BoardSquare[BoardSquarePos + Offset - 8]) == 9)
				{
					//moves[BoardSquarePos].PieceType = PieceTypeAtOffset;
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
					//moves[BoardSquarePos].PieceType = PieceTypeAtOffset;
					moves[BoardSquarePos].TargetSquares.push_back(BoardPosPlusOffset);
				}
				if (Offset == -8 and PieceTypeAtOffset == 0)
				{
					if (m_BoardSquare[BoardSquarePos - 16] == 0 and BoardSquarePos <= 55 and BoardSquarePos >= 48)
					{
						//moves[BoardSquarePos].PieceType = m_BoardSquare[BoardSquarePos - 16];
						moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos - 16);
					}
					//moves[BoardSquarePos].PieceType = PieceTypeAtOffset;
					moves[BoardSquarePos].TargetSquares.push_back(BoardPosPlusOffset);
				}
				//en passant
				if (BoardSquarePos <= 32 and BoardSquarePos >= 25 and Offset != -8 and PieceTypeAtOffset == 0 and !m_previousBoardSquare.empty() and (m_previousBoardSquare[BoardSquarePos + (Offset * 2)]) == 17 and (m_BoardSquare[BoardSquarePos + Offset + 8]) == 17)
				{
					//moves[BoardSquarePos].PieceType = PieceTypeAtOffset;
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
					//moves[BoardSquarePos].PieceType = PieceType + OffsetForDirections[direction];
					moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + OffsetForDirections[direction]);
					continue;
				}
				if (Board::IsPieceColorWhite(PieceType) != Board::IsPieceColorWhite(m_BoardSquare[BoardSquarePos + OffsetForDirections[direction]]))
				{
					//moves[BoardSquarePos].PieceType = PieceType + OffsetForDirections[direction];
					moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + OffsetForDirections[direction]);
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

#include "LegalMoves.h"
static std::array<unsigned int, 64> m_previousBoardSquare;
GenerateLegalMoves::GenerateLegalMoves(std::array<unsigned int, 64> BoardSquare, std::array<unsigned int, 64> previousBoardSquare)
	:moves(), m_BoardSquare(BoardSquare)
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
	m_previousBoardSquare = previousBoardSquare;
}

GenerateLegalMoves::GenerateLegalMoves(std::array<unsigned int, 64> BoardSquare)
	:moves(), m_BoardSquare(BoardSquare)
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
			NumOfSquaresUntilEdge[squareIndex][7] = std::min(numNorth, numWest);

		}
	}
}

void GenerateLegalMoves::GenerateMoves(bool isNextMoveForWhite)
{

}

//slider means Rook, Bishop, Queen
void GenerateLegalMoves::SliderMoveGen(int BoardSquarePos, bool isNextMoveForWhite)
{
	for (unsigned int direction = 0; direction < 8; direction++)
	{
		for (int i = 0; i < NumOfSquaresUntilEdge[BoardSquarePos][direction]; i++)
		{
			unsigned int PieceType = m_BoardSquare[BoardSquarePos];
			//rook
			if ((PieceType == 20 or PieceType == 12) and direction <= 3)
			{
				if (m_BoardSquare[PieceType + (OffsetForDirections[direction] * i)] == 0)
				{
					moves[BoardSquarePos].PieceType = PieceType + (OffsetForDirections[direction] * i);
					moves[BoardSquarePos].TargetSquares.push_back(PieceType + (OffsetForDirections[direction] * i));
					continue;
				}
				if (Board::IsPieceColorWhite(PieceType) != Board::IsPieceColorWhite(PieceType + (OffsetForDirections[direction] * i)))
				{
					moves[BoardSquarePos].PieceType = PieceType + (OffsetForDirections[direction] * i);
					moves[BoardSquarePos].TargetSquares.push_back(PieceType + (OffsetForDirections[direction] * i));
					moves[BoardSquarePos].PieceType = PieceType + (OffsetForDirections[direction] * (i + 1));
					moves[BoardSquarePos].TargetSquares.push_back(PieceType + (OffsetForDirections[direction] * (i + 1)));
					break;
				}
				else
				{
					break;
				}
			}

			//bishop
			if ((PieceType == 18 or PieceType == 10) and direction > 3)
			{
				if (m_BoardSquare[PieceType + (OffsetForDirections[direction] * i)] == 0)
				{
					moves[BoardSquarePos].PieceType = PieceType + (OffsetForDirections[direction] * i);
					moves[BoardSquarePos].TargetSquares.push_back(PieceType + (OffsetForDirections[direction] * i));
					continue;
				}
				if (Board::IsPieceColorWhite(PieceType) != Board::IsPieceColorWhite(PieceType + (OffsetForDirections[direction] * i)))
				{
					moves[BoardSquarePos].PieceType = PieceType + (OffsetForDirections[direction] * i);
					moves[BoardSquarePos].TargetSquares.push_back(PieceType + (OffsetForDirections[direction] * i));
					moves[BoardSquarePos].PieceType = PieceType + (OffsetForDirections[direction] * (i + 1));
					moves[BoardSquarePos].TargetSquares.push_back(PieceType + (OffsetForDirections[direction] * (i + 1)));
					break;
				}
				else
				{
					break;
				}
			}

			//queen
			if ((PieceType == 21 or PieceType == 13))
			{
				if (m_BoardSquare[PieceType + (OffsetForDirections[direction] * i)] == 0)
				{
					moves[BoardSquarePos].PieceType = PieceType + (OffsetForDirections[direction] * i);
					moves[BoardSquarePos].TargetSquares.push_back(PieceType + (OffsetForDirections[direction] * i));
					continue;
				}
				if (Board::IsPieceColorWhite(PieceType) != Board::IsPieceColorWhite(PieceType + (OffsetForDirections[direction] * i)))
				{
					moves[BoardSquarePos].PieceType = PieceType + (OffsetForDirections[direction] * i);
					moves[BoardSquarePos].TargetSquares.push_back(PieceType + (OffsetForDirections[direction] * i));
					moves[BoardSquarePos].PieceType = PieceType + (OffsetForDirections[direction] * (i + 1));
					moves[BoardSquarePos].TargetSquares.push_back(PieceType + (OffsetForDirections[direction] * (i + 1)));
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
	for(int i : OffsetForKnight)
	{
		if ((int)m_BoardSquare[BoardSquarePos] + i >= 0 and (int)m_BoardSquare[BoardSquarePos] + i <= 64)
		{
			//i know that if only the first condition is met then c++ won't check the other one, if it does i'm blaming c++ and i WILL be calling it stupid
			if (((int)m_BoardSquare[BoardSquarePos] + i) == 0 or (Board::IsPieceColorWhite(m_BoardSquare[BoardSquarePos]) != Board::IsPieceColorWhite(m_BoardSquare[BoardSquarePos] + i)))
			{
				moves[BoardSquarePos].PieceType = m_BoardSquare[BoardSquarePos] + i;
				moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + i);
			}
		}
	}
}

//pawn
void GenerateLegalMoves::PawnMoveGen(int BoardSquarePos, bool isNextMoveForWhite)
{
	unsigned int PieceType = m_BoardSquare[BoardSquarePos];
	if (PieceType == 17)//white pawn
	{
		for (int Offset : OffsetForWhitePawn)
		{
			unsigned int BoardPosPlusOffset = BoardSquarePos + Offset;
			unsigned int PieceTypeAtOffset = m_BoardSquare[BoardPosPlusOffset];
			
			if (NumOfSquaresUntilEdge[BoardSquarePos][Offset] != 0)
			{
				if (PieceTypeAtOffset != 0 and !Board::IsPieceColorWhite(PieceTypeAtOffset))
				{
					moves[BoardSquarePos].PieceType = PieceTypeAtOffset;
					moves[BoardSquarePos].TargetSquares.push_back(BoardPosPlusOffset);
				}
				if (Offset == 8 and PieceTypeAtOffset == 0)
				{
					if (m_BoardSquare[BoardSquarePos + 16] == 0 and BoardSquarePos <= 16 and BoardSquarePos >= 9)
					{
						moves[BoardSquarePos].PieceType = m_BoardSquare[BoardSquarePos + 16];
						moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos + 16);
					}
					moves[BoardSquarePos].PieceType = PieceTypeAtOffset;
					moves[BoardSquarePos].TargetSquares.push_back(BoardPosPlusOffset);
				}
				//en passant
				if (BoardSquarePos <= 40 and BoardSquarePos >= 33 and Offset != 8 and PieceTypeAtOffset == 0 and (m_previousBoardSquare[BoardSquarePos + (Offset*2)]) == 9 and (m_BoardSquare[BoardSquarePos + Offset - 8]) == 9)
				{
					moves[BoardSquarePos].PieceType = PieceTypeAtOffset;
					moves[BoardSquarePos].TargetSquares.push_back(BoardPosPlusOffset);
				}
			}
		}
	}
	if (PieceType == 9)//black pawn
	{
		for (int Offset : OffsetForBlackPawn)
		{
			unsigned int BoardPosPlusOffset = BoardSquarePos + Offset;
			unsigned int PieceTypeAtOffset = m_BoardSquare[BoardPosPlusOffset];

			if (NumOfSquaresUntilEdge[BoardSquarePos][Offset] != 0)
			{
				if (PieceTypeAtOffset != 0 and !Board::IsPieceColorWhite(PieceTypeAtOffset))
				{
					moves[BoardSquarePos].PieceType = PieceTypeAtOffset;
					moves[BoardSquarePos].TargetSquares.push_back(BoardPosPlusOffset);
				}
				if (Offset == -8 and PieceTypeAtOffset == 0)
				{
					if (m_BoardSquare[BoardSquarePos - 16] == 0 and BoardSquarePos <= 56 and BoardSquarePos >= 49)
					{
						moves[BoardSquarePos].PieceType = m_BoardSquare[BoardSquarePos - 16];
						moves[BoardSquarePos].TargetSquares.push_back(BoardSquarePos - 16);
					}
					moves[BoardSquarePos].PieceType = PieceTypeAtOffset;
					moves[BoardSquarePos].TargetSquares.push_back(BoardPosPlusOffset);
				}
				//en passant
				if (BoardSquarePos <= 32 and BoardSquarePos >= 25 and Offset != -8 and PieceTypeAtOffset == 0 and (m_previousBoardSquare[BoardSquarePos + (Offset * 2)]) == 9 and (m_BoardSquare[BoardSquarePos + Offset + 8]) == 17)
				{
					moves[BoardSquarePos].PieceType = PieceTypeAtOffset;
					moves[BoardSquarePos].TargetSquares.push_back(BoardPosPlusOffset);
				}
			}
		}
	}
}


#include "LegalMoves.h"
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
				moves[BoardSquarePos].TargetSquares.push_back(m_BoardSquare[BoardSquarePos] + i);
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
		if (m_BoardSquare[BoardSquarePos + 9] != 0)
		{
			if (Board::IsPieceColorWhite(m_BoardSquare[BoardSquarePos + 9]) != Board::IsPieceColorWhite(PieceType))
			{
				moves[BoardSquarePos].PieceType = m_BoardSquare[BoardSquarePos + 9];
				moves[BoardSquarePos].TargetSquares.push_back(m_BoardSquare[BoardSquarePos + 9]);
			}
		}
		if (m_BoardSquare[BoardSquarePos + 7] != 0)
		{
			if (Board::IsPieceColorWhite(m_BoardSquare[BoardSquarePos + 7]) != Board::IsPieceColorWhite(PieceType))
			{
				moves[BoardSquarePos].PieceType = m_BoardSquare[BoardSquarePos + 7];
				moves[BoardSquarePos].TargetSquares.push_back(m_BoardSquare[BoardSquarePos + 7]);
			}
		}
		if (m_BoardSquare[BoardSquarePos + 8] == 0)
		{
			moves[BoardSquarePos].PieceType = m_BoardSquare[BoardSquarePos + 8];
			moves[BoardSquarePos].TargetSquares.push_back(m_BoardSquare[BoardSquarePos + 8]);
		}
		if ((BoardSquarePos >= 9 and BoardSquarePos <= 16) and m_BoardSquare[BoardSquarePos + 16] == 0)
		{
			moves[BoardSquarePos].PieceType = m_BoardSquare[BoardSquarePos + 16];
			moves[BoardSquarePos].TargetSquares.push_back(m_BoardSquare[BoardSquarePos + 16]);
		}
		//TODO: en passant 3.7.3.1 on the FIDE rulebook
		//TODO: en passant 3.7.3.1 on the FIDE rulebook
		//TODO: en passant 3.7.3.1 on the FIDE rulebook
		//TODO: en passant 3.7.3.1 on the FIDE rulebook
		//TODO: en passant 3.7.3.1 on the FIDE rulebook
		//TODO: en passant 3.7.3.1 on the FIDE rulebook
		//TODO: en passant 3.7.3.1 on the FIDE rulebook
		//TODO: en passant 3.7.3.1 on the FIDE rulebook
		//TODO: en passant 3.7.3.1 on the FIDE rulebook
	}
	if (PieceType == 9)//black pawn
	{
		if (m_BoardSquare[BoardSquarePos - 9] != 0)
		{
			if (Board::IsPieceColorWhite(m_BoardSquare[BoardSquarePos - 9]) != Board::IsPieceColorWhite(PieceType))
			{
				moves[BoardSquarePos].PieceType = m_BoardSquare[BoardSquarePos + 9];
				moves[BoardSquarePos].TargetSquares.push_back(m_BoardSquare[BoardSquarePos + 9]);
			}
		}
		if (m_BoardSquare[BoardSquarePos - 7] != 0)
		{
			if (Board::IsPieceColorWhite(m_BoardSquare[BoardSquarePos - 7]) != Board::IsPieceColorWhite(PieceType))
			{
				moves[BoardSquarePos].PieceType = m_BoardSquare[BoardSquarePos - 7];
				moves[BoardSquarePos].TargetSquares.push_back(m_BoardSquare[BoardSquarePos - 7]);
			}
		}
		if (m_BoardSquare[BoardSquarePos - 8] == 0)
		{
			moves[BoardSquarePos].PieceType = m_BoardSquare[BoardSquarePos - 8];
			moves[BoardSquarePos].TargetSquares.push_back(m_BoardSquare[BoardSquarePos - 8]);
		}
		if ((BoardSquarePos >= 49 and BoardSquarePos <= 56) and m_BoardSquare[BoardSquarePos - 16] == 0)
		{
			moves[BoardSquarePos].PieceType = m_BoardSquare[BoardSquarePos - 16];
			moves[BoardSquarePos].TargetSquares.push_back(m_BoardSquare[BoardSquarePos - 16]);
		}
		//TODO: en passant 3.7.3.1 on the FIDE rulebook
		//TODO: en passant 3.7.3.1 on the FIDE rulebook
		//TODO: en passant 3.7.3.1 on the FIDE rulebook
		//TODO: en passant 3.7.3.1 on the FIDE rulebook
		//TODO: en passant 3.7.3.1 on the FIDE rulebook
		//TODO: en passant 3.7.3.1 on the FIDE rulebook
		//TODO: en passant 3.7.3.1 on the FIDE rulebook
		//TODO: en passant 3.7.3.1 on the FIDE rulebook
	}
}


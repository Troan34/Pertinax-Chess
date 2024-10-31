#include "LegalMoves.h"
GenerateLegalMoves::GenerateLegalMoves(std::array<unsigned int, 64> BoardSquare)
	:moves(), m_BoardSquare(BoardSquare)
{
	for (int file = 0; file < 8; file++)
	{
		for (int rank = 0; rank < 8; rank++)
		{
			unsigned int numNorth = 7 - rank;
			unsigned int numSouth = rank;
			unsigned int numWest = file;
			unsigned int numEast = 7 - file;

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
void GenerateLegalMoves::SliderMoveGen(unsigned int PieceType, int BoardSquarePos, bool isNextMoveForWhite)
{
	for (unsigned int direction = 0; direction < 8; direction++)
	{
		for (int i = 0; i < NumOfSquaresUntilEdge[BoardSquarePos][direction]; i++)
		{
			unsigned int PieceType = m_BoardSquare[BoardSquarePos];
			//rook
			if ((PieceType == 20 or PieceType == 12) and direction <= 3)
			{
				if(Board::IsPieceColorWhite(PieceType == Board::IsPieceColorWhite(PieceType + (OffsetForDirections[direction]* i))))
					break;
				if (Board::IsPieceColorWhite(PieceType != Board::IsPieceColorWhite(PieceType + (OffsetForDirections[direction] * i))))
				{
					moves[BoardSquarePos + (OffsetForDirections[direction] * i)].PieceType = PieceType;
					moves[BoardSquarePos + (OffsetForDirections[direction] * i)].TargetSquares.push_back(PieceType + (OffsetForDirections[direction] * i));
					break;
				}
				moves[BoardSquarePos + (OffsetForDirections[direction] * i)].PieceType = PieceType;
				moves[BoardSquarePos + (OffsetForDirections[direction] * i)].TargetSquares.push_back(PieceType + (OffsetForDirections[direction] * i));
			}

			//bishop
			if ((PieceType == 18 or PieceType == 10) and direction > 3)
			{
				if (Board::IsPieceColorWhite(PieceType == Board::IsPieceColorWhite(PieceType + (OffsetForDirections[direction] * i))))
					break;
				if (Board::IsPieceColorWhite(PieceType != Board::IsPieceColorWhite(PieceType + (OffsetForDirections[direction] * i))))
				{
					moves[BoardSquarePos + (OffsetForDirections[direction] * i)].PieceType = PieceType;
					moves[BoardSquarePos + (OffsetForDirections[direction] * i)].TargetSquares.push_back(PieceType + (OffsetForDirections[direction] * i));
					break;
				}
				moves[BoardSquarePos + (OffsetForDirections[direction] * i)].PieceType = PieceType;
				moves[BoardSquarePos + (OffsetForDirections[direction] * i)].TargetSquares.push_back(PieceType + (OffsetForDirections[direction] * i));
			}
			//queen
			if ((PieceType == 21 or PieceType == 13))
			{
				if (Board::IsPieceColorWhite(PieceType == Board::IsPieceColorWhite(PieceType + (OffsetForDirections[direction] * i))))
					break;
				if (Board::IsPieceColorWhite(PieceType != Board::IsPieceColorWhite(PieceType + (OffsetForDirections[direction] * i))))
				{
					moves[BoardSquarePos + (OffsetForDirections[direction] * i)].PieceType = PieceType;
					moves[BoardSquarePos + (OffsetForDirections[direction] * i)].TargetSquares.push_back(PieceType + (OffsetForDirections[direction] * i));
					break;
				}
				moves[BoardSquarePos + (OffsetForDirections[direction] * i)].PieceType = PieceType;
				moves[BoardSquarePos + (OffsetForDirections[direction] * i)].TargetSquares.push_back(PieceType + (OffsetForDirections[direction] * i));
			}
			
		}
	}
}


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
void GenerateLegalMoves::SliderMoveGen(unsigned int PieceType, unsigned int BoardSquarePos, bool isNextMoveForWhite)
{
	for (unsigned int direction = 0; direction < 8; direction++)
	{
		for (unsigned int i = 0; i < NumOfSquaresUntilEdge[BoardSquarePos][direction]; i++)
		{
			//rook
			if (m_BoardSquare[BoardSquarePos] == 10 or m_BoardSquare[BoardSquarePos] == 4)
			{
				if (direction > 3)
					continue;

			}

		}
	}
}


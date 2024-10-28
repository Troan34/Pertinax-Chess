#include "LegalMoves.h"
//doesn't work
void GenerateLegalMoves::SetNumOfSquaresUntilEdge()
{
	for (int file = 0; file < 8; file++)
	{
		for (int rank = 0; rank < 8; rank++)
		{
			int numNorth = 7 - rank;
			int numSouth = rank;
			int numWest = file;
			int numEast = 7 - file;

			int squareIndex = rank * 8 + file;

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

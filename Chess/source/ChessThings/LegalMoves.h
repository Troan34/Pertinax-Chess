#pragma once
#include <array>
#include <cmath>
class GenerateLegalMoves
{
private:
	const int OffsetForDirections[8] = {8, -8, -1, 1, 7, -7, 9, -9};
	

public:
	static std::array<std::array<int, 8>, 64>NumOfSquaresUntilEdge;
	static void SetNumOfSquaresUntilEdge();
};
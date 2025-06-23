#pragma once
#include "ChessThings/Board.h"
#include "ChessThings/Engine/Search.h"

class IterativeDeepening
{
private:

	Timer Time;
	std::array<uint8_t, 64> m_BoardSquare;
	std::array<uint8_t, 64> m_PreviousBoardSquare;
	canCastle m_CanCastle;
	uint16_t m_MoveNum;
	std::vector<Move> m_SearchMoves;
	size_t HashSize;
	int16_t m_MaxDepth;
	uint8_t Depth = 0;

public:

	IterativeDeepening(const std::array<uint8_t, 64>& BoardSquare, const std::array<uint8_t, 64>& PreviousBoardSquare, const canCastle& CanCastle, const uint16_t& MoveNum,
		std::vector<Move>& SearchMoves, const size_t& HashSize, Timer& Time, int16_t MaxDepth);
	Move GetBestMove();

};
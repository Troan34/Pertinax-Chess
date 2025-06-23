#include "ChessThings/Engine/IterativeD.h"


IterativeDeepening::IterativeDeepening(const std::array<uint8_t, 64>& BoardSquare, const std::array<uint8_t, 64>& PreviousBoardSquare, const canCastle& CanCastle, const uint16_t& MoveNum, std::vector<Move>& SearchMoves, const size_t& HashSize, Timer& Time, int16_t MaxDepth)
	:Time(Time),m_BoardSquare(BoardSquare), m_PreviousBoardSquare(PreviousBoardSquare), m_CanCastle(CanCastle), m_MoveNum(MoveNum), m_SearchMoves(SearchMoves), HashSize(HashSize), m_MaxDepth(MaxDepth)
{
}

Move IterativeDeepening::GetBestMove()
{
	std::vector<Move> CurrentPV;
	int32_t BestEval = -INT32_MAX;
	Move BestMove;

	while(Depth < m_MaxDepth)
	{
		Depth++;
		Search search(m_BoardSquare, m_PreviousBoardSquare, m_CanCastle, Depth, m_MoveNum, m_SearchMoves, HashSize, CurrentPV);
		auto bestMove = search.GetBestMoveWithEval();

		BestEval = bestMove.second;
		BestMove = bestMove.first;
	}

	return BestMove;
}

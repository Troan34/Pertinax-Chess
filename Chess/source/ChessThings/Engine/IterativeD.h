#pragma once
#include "ChessThings/Board.h"
#include "ChessThings/Engine/Search.h"
#include "ChessThings/LegalMoves.h"


class IterativeDeepening
{
private:

	Timer Time;
	Position ChessPosition;
	std::vector<Move> m_SearchMoves;
	size_t HashSize;
	int16_t m_MaxDepth;
	uint8_t Depth = 0;
	bool RanASearch = false;//exception handling
	uint64_t NumOfNodesSearched;


	bool* stop;
	bool WhiteTurn;



	std::chrono::milliseconds TimeForMove();
	std::chrono::milliseconds TimeForDepth(std::chrono::milliseconds DepthTime);
	void UpdateTimeControl(std::chrono::milliseconds TimeUsed);
	void UpdateEBF(double TimeUsedForDepth, double TimeUsedForLastDepth);
	void PrintInfo(UCIInfoes Info);

public:

	IterativeDeepening(const Position& ChessPosition, std::vector<Move>& SearchMoves, const size_t& HashSize, Timer& Time, int16_t MaxDepth, bool WhiteTurn, bool* Stop);

	Move GetBestMove(bool RanWithGo);
	std::vector<Move> GetPV();

};
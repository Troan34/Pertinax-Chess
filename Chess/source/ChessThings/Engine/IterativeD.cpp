#include "ChessThings/Engine/IterativeD.h"


std::chrono::milliseconds IterativeDeepening::TimeForMove()
{
	std::chrono::milliseconds* TimeLeft = WhiteTurn ? &Time.WTime : &Time.BTime;

	return (*TimeLeft / 20) + ((WhiteTurn ? Time.WIncrement : Time.BIncrement)/2);//to algorithm-fy
}

void IterativeDeepening::UpdateTimeControl(std::chrono::milliseconds TimeUsed)
{
	if (WhiteTurn)
	{
		Time.WTime -= TimeUsed + Time.WIncrement;
	}
	else
	{
		Time.BTime -= TimeUsed + Time.BIncrement;
	}
}

void IterativeDeepening::PrintInfo(UCIInfoes Info)
{
	//waiting for this to work (i put the compiler to c++23)
	//std::println("info depth {} nodes {} nps {} hashfull {} pv {}", *Info.Depth, *Info.NumOfNodes, *Info.NpS, *Info.HashFull, Board::GetPrintableFromVecOfMoves(*Info.PV));
	std::cout << "info " << "depth " << *Info.Depth + '0' - 48 <<
		" score " << [&]()->std::string 
		{
			std::string str;
			if (*Info.Score == INT32_MAX)
				str = "mate " + std::to_string(Info.PV->size());
			else if (*Info.Score == -INT32_MAX)
				str = "mate -" + std::to_string(Info.PV->size());
			else
			{
				str = "cp " + std::to_string(*Info.Score);
			}
			return str;
		}() <<
		" nodes " << *Info.NumOfNodes << " nps " << *Info.NpS << " hashfull " << *Info.HashFull
		<< " pv " << Board::GetPrintableFromVecOfMoves(*Info.PV) << std::endl;
}

IterativeDeepening::IterativeDeepening(const std::array<uint8_t, 64>& BoardSquare, const std::array<uint8_t, 64>& PreviousBoardSquare, const canCastle& CanCastle, const uint16_t& MoveNum, std::vector<Move>& SearchMoves, const size_t& HashSize, Timer& Time, int16_t MaxDepth, bool WhiteTurn, bool* stop)
	:Time(Time),m_BoardSquare(BoardSquare), m_PreviousBoardSquare(PreviousBoardSquare), m_CanCastle(CanCastle), m_MoveNum(MoveNum), m_SearchMoves(SearchMoves), HashSize(HashSize), m_MaxDepth(MaxDepth), WhiteTurn(WhiteTurn), stop(stop)
{
}

Move IterativeDeepening::GetBestMove(bool RanWithGo)
{

	auto start = std::chrono::high_resolution_clock::now();

	std::vector<Move> CurrentPV;
	int32_t BestEval = -INT32_MAX;
	Move BestMove;

	while(Depth < m_MaxDepth)
	{
		if(*stop)
		{
			if(Depth > 0)
				break;
		}

		auto LocalStart = std::chrono::high_resolution_clock::now();

		Depth++;
		Search search(m_BoardSquare, m_PreviousBoardSquare, m_CanCastle, Depth, m_MoveNum, m_SearchMoves, HashSize);
		auto bestMove = search.GetBestMoveWithEval(CurrentPV);

		BestEval = bestMove.second;
		BestMove = CurrentPV.front();

		auto Stop = std::chrono::high_resolution_clock::now();
		if (RanWithGo)
		{
			UCIInfoes Info;
			Info.Depth = &Depth;
			auto Hashfullness = search.GetTTFullness();
			Info.HashFull = &Hashfullness;

			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(Stop - start);
			auto Nps = uint32_t((float)search.GetNodesVisited() / (float)(duration.count() / 1000));
			Info.NpS = &Nps;

			auto NumOfNds = search.GetNodesVisited();
			Info.NumOfNodes = &NumOfNds;
			std::vector<Move> PV = CurrentPV;
			Info.PV = &PV;
			Info.Score = &BestEval;

			PrintInfo(Info);
		}

#ifndef _DEBUG
		if (std::chrono::duration_cast<std::chrono::milliseconds>(Stop - start) >= TimeForMove())
			break;
#endif

	}
	auto Stop = std::chrono::high_resolution_clock::now();
	UpdateTimeControl(std::chrono::duration_cast<std::chrono::milliseconds>(Stop - start));

	RanASearch = true;
	*stop = false;
	return BestMove;
}

std::vector<Move> IterativeDeepening::GetPV()
{
	if (RanASearch)
	{
		return m_SearchMoves;
	}
}


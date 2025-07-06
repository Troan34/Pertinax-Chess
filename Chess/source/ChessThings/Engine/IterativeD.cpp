#include "ChessThings/Engine/IterativeD.h"


std::chrono::milliseconds IterativeDeepening::TimeLeft(std::chrono::milliseconds TimeUsed)
{
	std::chrono::milliseconds* TimeLeft = WhiteTurn ? &Time.WTime : &Time.BTime;
	*TimeLeft += WhiteTurn ? Time.WIncrement : Time.BIncrement;
	*TimeLeft -= TimeUsed;
	return (*TimeLeft + TimeUsed) / (90 - m_MoveNum);//to algorithm-fy
}

void IterativeDeepening::PrintInfo(UCIInfoes Info)
{
	//waiting for this to work (i put the compiler to c++23)
	//std::println("info depth {} nodes {} nps {} hashfull {} pv {}", *Info.Depth, *Info.NumOfNodes, *Info.NpS, *Info.HashFull, Board::GetPrintableFromVecOfMoves(*Info.PV));
	std::cout << "info " << "depth " << *Info.Depth + '0' - 48 << " nodes " << *Info.NumOfNodes << " nps " << *Info.NpS << " hashfull " << *Info.HashFull
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

	while(Depth < m_MaxDepth and !(*stop))
	{
		auto LocalStart = std::chrono::high_resolution_clock::now();

		Depth++;
		Search search(m_BoardSquare, m_PreviousBoardSquare, m_CanCastle, Depth, m_MoveNum, m_SearchMoves, HashSize);
		auto bestMove = search.GetBestMoveWithEval(CurrentPV);

		BestEval = bestMove.second;
		BestMove = bestMove.first;

		auto stop = std::chrono::high_resolution_clock::now();
		if (RanWithGo)
		{
			UCIInfoes Info;
			Info.Depth = &Depth;
			auto Hashfullness = search.GetTTFullness();
			Info.HashFull = &Hashfullness;

			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
			auto Nps = uint32_t((float)search.GetNodesVisited() / (float)(duration.count() / 1000));
			Info.NpS = &Nps;

			auto NumOfNds = search.GetNodesVisited();
			Info.NumOfNodes = &NumOfNds;
			std::vector<Move> PV = CurrentPV;
			Info.PV = &PV;

			PrintInfo(Info);
		}
		
		if (std::chrono::duration_cast<std::chrono::milliseconds>(stop - start) >= TimeLeft(std::chrono::duration_cast<std::chrono::milliseconds>(stop - LocalStart)))
			break;
	}

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


#include "ChessThings/Engine/IterativeD.h"
using namespace std::chrono_literals;
//time management
/*
	EBF does stand for effective branching factor, but for our use it better means time-to-search factor, although they do correlate
*/
static double PersistentEBF = 2.5f;
double EBF = 2.5f;


std::chrono::milliseconds IterativeDeepening::TimeForMove()
{
	std::chrono::milliseconds* TimeLeft = WhiteTurn ? &Time.WTime : &Time.BTime;
	auto TimeForMove = (*TimeLeft / 20) + ((WhiteTurn ? Time.WIncrement : Time.BIncrement) / 2);//to algorithm-fy
	return TimeForMove;
}

std::chrono::milliseconds IterativeDeepening::TimeForDepth(std::chrono::milliseconds DepthTime)
{
	double BlendedEBF = (0.8 * EBF) + (0.2 * PersistentEBF);
	auto TimeForDepth = DepthTime.count() * BlendedEBF;
	return std::chrono::milliseconds((int64_t)TimeForDepth);
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

void IterativeDeepening::UpdateEBF(double TimeUsedForDepth, double TimeUsedForLastDepth)
{
	constexpr double alpha = 0.7f; //smoothing factor, tunable
	EBF = (alpha * (TimeUsedForDepth / TimeUsedForDepth)) + ((1 - alpha) * EBF);
	EBF = std::clamp(EBF, (double)1.2f, (double)6.0f);

	constexpr double beta = 0.1f; //smoothing factor, tunable
	PersistentEBF = (beta * (TimeUsedForDepth / TimeUsedForDepth)) + ((1 - beta) * PersistentEBF);
	PersistentEBF = std::clamp(PersistentEBF, (double)1.2f, (double)6.0f);
}

void IterativeDeepening::PrintInfo(UCIInfoes Info)
{
	//waiting for this to work (i put the compiler to c++23)
	//std::println("info depth {} nodes {} nps {} hashfull {} pv {}", *Info.Depth, *Info.NumOfNodes, *Info.NpS, *Info.HashFull, Board::GetPrintableFromVecOfMoves(*Info.PV));
	std::cout << "info " << "depth " << *Info.Depth + '0' - 48 <<
		" score " << 
		[&]()->std::string 
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
	auto TimeForNextMove = TimeForMove();
	std::chrono::milliseconds TimeForLastDepth(5); //just a number

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
		BestEval = search.GetBestMoveWithEval(CurrentPV);

		BestMove = CurrentPV.front();

		auto Stop = std::chrono::high_resolution_clock::now();
		TimeForNextMove -= std::chrono::duration_cast<std::chrono::milliseconds>(Stop - LocalStart);

		//output things
		if (RanWithGo)
		{
			UCIInfoes Info;
			Info.Depth = &Depth;
			auto Hashfullness = search.GetTTFullness();
			Info.HashFull = &Hashfullness;

			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(Stop - LocalStart);
			uint64_t Nps = (search.GetNodesVisited()*1000) / duration.count();
			Info.NpS = &Nps;

			auto NumOfNds = search.GetNodesVisited();
			Info.NumOfNodes = &NumOfNds;
			std::vector<Move> PV = CurrentPV;
			Info.PV = &PV;
			Info.Score = &BestEval;
			
			PrintInfo(Info);
		}

		UpdateEBF(std::chrono::duration_cast<std::chrono::milliseconds>(Stop - LocalStart).count(), TimeForLastDepth.count());

		
#ifndef _DEBUG
		if (TimeForDepth(std::chrono::duration_cast<std::chrono::milliseconds>(Stop - LocalStart)) > TimeForNextMove)
			break;
		if (Depth == 5 and TimeForNextMove < 40000ms)
			break;//my nodes visited shoot up harder than a factorial, TO BE REMOVED WHEN ENGINE IS BETTER
#endif
		TimeForLastDepth = std::chrono::duration_cast<std::chrono::milliseconds>(Stop - LocalStart);

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


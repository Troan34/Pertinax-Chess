#pragma once
#include "ChessThings/Board.h"
#include "ChessThings/Engine/Evaluation.h"
#include "ChessThings/Engine/Transposition/ZobristHashing.h"
#include "ChessThings/Engine/Transposition/TT.h"
#include <iostream>
#include <algorithm>
#include <chrono>


class Search {
private:
	const bit::Position m_ChessPosition;
	const uint8_t m_depth;
	const std::vector<Move> m_SearchMoves; //Moves selected by go
	size_t HashSize;
	pv_line* m_PreviousPV;

	uint64_t NodesVisited = 0;
	uint64_t Cutoffs = 0;
	uint64_t TThits = 0;

	int32_t NegaMax(ZobristHashing& m_Hash, bit::Position ChessPosition, uint8_t depth, int32_t alpha, int32_t beta, pv_line* PVLine);

	//the double definitions are for the quiet versions
	void MakeMove(const GenerateLegalMoves& LegalMoves, ZobristHashing& Hash, const Move Move_, bit::Position& ChessPosition);
	void QMakeMove(const GenerateLegalMoves& LegalMoves, const Move Move_, Position& ChessPosition);
	//218 is the maximum amount of legal moves for a position
	void OrderMoves(const GenerateLegalMoves& LegalMoves, const bit::BitPosition& fun_BoardSquare, GuessStruct* GuessedOrder, uint8_t depth);
	void QOrderMoves(const GenerateLegalMoves& LegalMoves, const std::array<uint8_t, 64>& fun_BoardSquare, GuessStruct* GuessedOrder, uint32_t NumOfTacticalMoves);

	int32_t QuietSearch(Position ChessPosition, int32_t alpha, int32_t beta);

	int32_t SEE(const Position& ChessPosition, Move Move);
	int32_t SEE(const bit::Position& ChessPosition, Move Move);

public:

	Search(const Position& ChessPosition, const uint8_t depth, std::vector<Move>& SearchMoves, const size_t& HashSize);
	int32_t GetBestMoveWithEval(pv_line& PV);
	uint64_t GetNodesVisited() const;
	uint16_t GetTTFullness() const;
	static void ClearTT();
};
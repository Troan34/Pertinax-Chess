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
	const std::array<uint8_t, 64> m_BoardSquare;
	const std::array<uint8_t, 64> m_PreviousBoardSquare;
	const canCastle m_CanCastle;
	const uint8_t m_depth;
	const uint16_t m_MoveNum;
	const std::vector<Move> m_SearchMoves;
	size_t HashSize;
	std::vector<Move> m_PV;


	uint8_t m_BestMove;
	uint8_t m_BestBoardPos;
	uint8_t m_BestPromotion = 65;
	int32_t m_BestEvaluation = -INT32_MAX;
	uint64_t NodesVisited = 0;

	SearchResult NegaMax(ZobristHashing& m_Hash, std::array<uint8_t, 64Ui64> BoardSquare, std::array<uint8_t, 64> previousBoardSquare, canCastle CanCastle,uint8_t MoveNum, uint8_t depth, int32_t alpha, int32_t beta, std::vector<Move>& PreviousPV);
	void MakeMove(const GenerateLegalMoves& LegalMoves, ZobristHashing& Hash, Move Move_, std::array<uint8_t, 64>& fun_BoardSquare, std::array<uint8_t, 64>& fun_previousBoardSquare, canCastle& Castle);
	std::vector<GuessStruct> OrderMoves(const GenerateLegalMoves& LegalMoves, const std::array<uint8_t, 64>& fun_BoardSquare);

public:
	std::vector<Move> m_PreviousPV;

	Search(const std::array<uint8_t, 64>& BoardSquare, const std::array<uint8_t, 64>& PreviousBoardSquare, const canCastle& CanCastle, const uint8_t& depth, const uint16_t& MoveNum,
		std::vector<Move>& SearchMoves, const size_t& HashSize);
	~Search();
	std::pair<Move, int32_t> GetBestMoveWithEval(std::vector<Move>& PV);
	uint64_t GetNodesVisited() const;
	uint16_t GetTTFullness() const;

};
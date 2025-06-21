#pragma once
#include "ChessThings/Board.h"
#include "ChessThings/Engine/Evaluation.h"
#include "ChessThings/Engine/Transposition/ZobristHashing.h"
#include "ChessThings/Engine/Transposition/TT.h"
#include <iostream>
#include <algorithm>
#include <chrono>

struct GuessStruct
{
	uint8_t BoardSquare;
	uint8_t Move;
	uint8_t PromotionType = 65;
	int16_t GuessedEval;

	GuessStruct(uint8_t BoardSquare, uint8_t Move, uint8_t PromotionType, int32_t GuessedEval)
		: BoardSquare(BoardSquare),
		Move(Move),
		PromotionType(PromotionType),
		GuessedEval(GuessedEval) {
	}
};


class Search {
private:
	const std::array<uint8_t, 64> m_BoardSquare;
	const std::array<uint8_t, 64> m_PreviousBoardSquare;
	const canCastle m_CanCastle;
	const uint8_t m_depth;
	const uint16_t m_MoveNum;
	const std::vector<Move> m_SearchMoves;
	size_t HashSize;


	uint8_t m_BestMove;
	uint8_t m_BestBoardPos;
	uint8_t m_BestPromotion = 65;
	int32_t m_BestEvaluation = -INT32_MAX;

	int NegaMax(ZobristHashing& m_Hash, std::array<uint8_t, 64Ui64> BoardSquare, std::array<uint8_t, 64> previousBoardSquare, canCastle CanCastle,uint8_t MoveNum, uint8_t depth, int32_t alpha, int32_t beta );
	void MakeMove(ZobristHashing& m_Hash, const GenerateLegalMoves& LegalMoves, ZobristHashing& Hash, Move Move_, std::array<uint8_t, 64>& fun_BoardSquare, std::array<uint8_t, 64>& fun_previousBoardSquare, canCastle& Castle);
	std::vector<GuessStruct> OrderMoves(const GenerateLegalMoves& LegalMoves, const std::array<uint8_t, 64>& fun_BoardSquare);

public:

	Search(const std::array<uint8_t, 64>& BoardSquare, const std::array<uint8_t, 64>& PreviousBoardSquare, const canCastle& CanCastle, const uint8_t& depth, const uint16_t& MoveNum, std::vector<Move>& SearchMoves, const size_t& HashSize);
	~Search();
	Move GetBestMove();
	void ComputeUCIInfo(UCIInfoes UciInfo);

};
#pragma once
#include "ChessThings/Board.h"
#include "ChessThings/Engine/Evaluation.h"
#include "ChessThings/Engine/Transposition/ZobristHashing.h"
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

struct TranspositionTable
{
	uint64_t Hash;
	Move BestMove;
	char BoundType; // e for exact, l for lower, u for upper
	int32_t Evaluation;
	uint8_t Depth;
};

class Search {
private:
	std::unique_ptr<ZobristHashing> m_Hash;
	const std::array<uint8_t, 64> m_BoardSquare;
	const std::array<uint8_t, 64> m_PreviousBoardSquare;
	const canCastle m_CanCastle;
	const uint8_t m_depth;
	const uint16_t m_MoveNum;
	uint8_t m_BestMove;
	uint8_t m_BestBoardPos;
	uint8_t m_BestPromotion = 65;
	int32_t m_BestEvaluation = -INT32_MAX;

	int NegaMax(std::array<uint8_t, 64Ui64> BoardSquare, std::array<uint8_t, 64> previousBoardSquare, canCastle CanCastle,uint8_t MoveNum, uint8_t depth, int32_t alpha, int32_t beta );
	void MakeMove(const GenerateLegalMoves& LegalMoves, std::unique_ptr<ZobristHashing>& Hash, const uint8_t& BoardSquare, const uint8_t& move, std::array<uint8_t, 64>& fun_BoardSquare, std::array<uint8_t, 64>& fun_previousBoardSquare, canCastle& Castle, const uint8_t& PieceTypeToPromoteTo);
	std::vector<GuessStruct> OrderMoves(const GenerateLegalMoves& LegalMoves, const std::array<uint8_t, 64>& fun_BoardSquare);

public:

	Search(std::array<uint8_t, 64> BoardSquare, std::array<uint8_t, 64> PreviousBoardSquare, canCastle CanCastle, uint8_t depth, uint16_t MoveNum);
	~Search();
	std::pair<std::pair<uint8_t, uint8_t>, uint8_t> GetBestMove();

};
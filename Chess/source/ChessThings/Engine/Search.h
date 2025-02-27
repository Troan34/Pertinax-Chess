#pragma once
#include "ChessThings/Board.h"
#include "ChessThings/Engine/Evaluation.h"

class Search {
private:
	const std::array<uint8_t, 64> m_BoardSquare;
	const std::array<uint8_t, 64> m_PreviousBoardSquare;
	const canCastle m_CanCastle;
	const uint8_t m_depth;
	const uint16_t m_MoveNum;


	EvalMove LoopThroughTheTree(std::array<uint8_t, 64Ui64> BoardSquare, std::array<uint8_t, 64> previousBoardSquare, canCastle CanCastle,uint8_t MoveNum, uint8_t depth );
	void MakeMove(const GenerateLegalMoves& LegalMoves, const uint8_t& BoardSquare, const uint8_t& move, std::array<uint8_t, 64>& fun_BoardSquare, std::array<uint8_t, 64>& fun_previousBoardSquare, canCastle& Castle, const uint8_t& PieceTypeToPromoteTo);

public:

	Search(std::array<uint8_t, 64> BoardSquare, std::array<uint8_t, 64> PreviousBoardSquare, canCastle CanCastle, uint8_t depth, uint16_t MoveNum);
	~Search();
	EvalMove GetBestMove();

};
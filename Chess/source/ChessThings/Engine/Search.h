#pragma once
#include "ChessThings/Board.h"
#include "ChessThings/Engine/Evaluation.h"

class Search {
private:
	std::array<uint8_t, 64> m_BoardSquare;
	std::array<uint8_t, 64> m_PreviousBoardSquare;
	canCastle m_CanCastle;
	uint8_t m_depth;
	uint16_t m_MoveNum;

	int LoopThroughTheTree();
	void MakeMove(const GenerateLegalMoves& LegalMoves, const uint8_t& BoardSquare, const uint8_t& move, std::array<uint8_t, 64>& fun_BoardSquare, std::array<uint8_t, 64>& fun_previousBoardSquare, canCastle& Castle, const uint8_t& PieceTypeToPromoteTo);

public:

	Search(std::array<uint8_t, 64> BoardSquare, std::array<uint8_t, 64> PreviousBoardSquare, canCastle CanCastle, uint8_t depth, uint16_t MoveNum);
	~Search();

};
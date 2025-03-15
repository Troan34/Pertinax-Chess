#include "Evaluation.h"

Evaluator::Evaluator(const GenerateLegalMoves& LegalMoves)
	:m_LegalMoves(LegalMoves)
{

}

void Evaluator::SetParameters(const std::array<uint8_t, 64>& BoardSquare, const std::array<uint8_t, 64>& PreviousBoardSquare, const canCastle& CanCastle, const uint8_t& MoveNum)
{
	m_BoardSquare = BoardSquare;
	m_PreviousBoardSquare = PreviousBoardSquare;
	m_CanCastle = CanCastle;
	m_SideToMove = (MoveNum % 2 == 0) ? 1 : -1;
	m_Evaluation = 0;
}


int Evaluator::Evaluate()
{
	BoardMatValue();
	//m_Evaluation += m_LegalMoves.m_NumOfLegalMoves * 1 * m_SideToMove;

	return m_Evaluation * m_SideToMove;
}

void Evaluator::BoardMatValue()
{
	int32_t eval = 0;
	for (uint8_t i = 0; i != 64; ++i)
	{
		if (m_BoardSquare[i] == 0)
			continue;
		if (Board::IsPieceColorWhite(m_BoardSquare[i]))
		{
			eval += ConvertPieceTypeToMatValue(m_BoardSquare[i]);
		}
		else
		{
			eval -= ConvertPieceTypeToMatValue(m_BoardSquare[i]);
		}
	}
	
	m_Evaluation += std::abs(eval);
}


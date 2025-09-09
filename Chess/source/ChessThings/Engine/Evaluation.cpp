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
	m_MoveNum = MoveNum;
	m_Evaluation = 0;
}


int Evaluator::Evaluate()
{
	m_Evaluation = BoardMatValue() + MobilityEval();
	//m_Evaluation += m_LegalMoves.m_NumOfLegalMoves * 1 * m_SideToMove;

	return m_Evaluation * ((m_MoveNum % 2 == 0) ? 1 : -1);
}

int32_t Evaluator::BoardMatValue()
{
	int32_t MatEval = 0;
	for (uint8_t i = 0; i != 64; ++i)
	{
		if (m_BoardSquare[i] == 0)
			continue;
		if (Board::IsPieceColorWhite(m_BoardSquare[i]))
		{
			MatEval += ConvertPieceTypeToMatValue(m_BoardSquare[i]);
		}
		else
		{
			MatEval -= ConvertPieceTypeToMatValue(m_BoardSquare[i]);
		}
	}
	
	return MatEval;
}

int32_t Evaluator::MobilityEval()
{
	int32_t MobilityEval = 0;
	for (uint8_t i = 0; i != 64; ++i)
	{
		if (m_BoardSquare[i] == 0)
			continue;
		if (Board::IsPieceColorWhite(m_BoardSquare[i]))
		{
			MobilityEval += (int)(m_LegalMoves.m_NumOfLegalMoves * 0.1);
		}
		else
		{
			MobilityEval -= (int)(m_LegalMoves.m_NumOfLegalMoves * 0.1);
		}
	}
	return MobilityEval;
}
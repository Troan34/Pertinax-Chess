#include "Evaluation.h"

Evaluator::Evaluator(const GenerateLegalMoves& LegalMoves, const std::array<uint8_t, 64>& BoardSquare, const std::array<uint8_t, 64>& PreviousBoardSquare, const canCastle& CanCastle)
	:m_BoardSquare(BoardSquare), m_PreviousBoardSquare(PreviousBoardSquare), m_CanCastle(CanCastle), m_LegalMoves(LegalMoves)
{

}


int Evaluator::Evaluate(const uint8_t& BoardSquarePos, const uint8_t& Move)
{
	int Eval = 0;

	if (m_BoardSquare[Move] != 0)
	{
		Eval += CapturingEval(m_BoardSquare[BoardSquarePos], m_BoardSquare[Move]);
	}

	return Eval;
}

int Evaluator::CapturingEval(const uint8_t& CapturingPieceType, const uint8_t& CapturedPieceType)
{
	return (ConvertPieceTypeToMatValue(CapturedPieceType) - (ConvertPieceTypeToMatValue(CapturingPieceType) / 10) * (Board::IsPieceColorWhite(CapturingPieceType) ? 1 : -1));
}



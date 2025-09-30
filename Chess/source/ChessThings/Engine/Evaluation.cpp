#include "Evaluation.h"
//Michniewski's tables (let's not reinvent the wheel)
static constexpr std::array<int8_t, 64> WHITE_PAWN_PST = {
	0,  0,  0,  0,  0,  0,  0,  0,
	50, 50, 50, 50, 50, 50, 50, 50,
	10, 10, 20, 30, 30, 20, 10, 10,
	5,  5, 10, 25, 25, 10,  5,  5,
	0,  0,  0, 20, 20,  0,  0,  0,
	5, -5,-10,  0,  0,-10, -5,  5,
	5, 10, 10,-20,-20, 10, 10,  5,
	0,  0,  0,  0,  0,  0,  0,  0
};
static constexpr std::array<int8_t, 64> BLACK_PAWN_PST = {
	0,  0,  0,  0,  0,  0,  0,  0,
	5, 10, 10,-20,-20, 10, 10,  5,
	5, -5,-10,  0,  0,-10, -5,  5,
	0,  0,  0, 20, 20,  0,  0,  0,
	5,  5, 10, 25, 25, 10,  5,  5,
	10, 10, 20, 30, 30, 20, 10, 10,
	50, 50, 50, 50, 50, 50, 50, 50,
	0,  0,  0,  0,  0,  0,  0,  0
};
static constexpr std::array<int8_t, 64> WHITE_KNIGHT_PST = {
	-50,-40,-30,-30,-30,-30,-40,-50,
	-40,-20,  0,  0,  0,  0,-20,-40,
	-30,  0, 10, 15, 15, 10,  0,-30,
	-30,  5, 15, 20, 20, 15,  5,-30,
	30,  0, 15, 20, 20, 15,  0,-30,
	-30,  5, 10, 15, 15, 10,  5,-30,
	-40,-20,  0,  5,  5,  0,-20,-40,
	-50,-40,-30,-30,-30,-30,-40,-50
};
static constexpr std::array<int8_t, 64> BLACK_KNIGHT_PST = {
	-50,-40,-30,-30,-30,-30,-40,-50,
	-40,-20,  0,  5,  5,  0,-20,-40,
	-30,  5, 10, 15, 15, 10,  5,-30,
	30,  0, 15, 20, 20, 15,  0,-30,
	-30,  5, 15, 20, 20, 15,  5,-30,
	-30,  0, 10, 15, 15, 10,  0,-30,
	-40,-20,  0,  0,  0,  0,-20,-40,
	-50,-40,-30,-30,-30,-30,-40,-50
};
static constexpr std::array<int8_t, 64> WHITE_BISHOP_PST = {
	-20,-10,-10,-10,-10,-10,-10,-20,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-10,  0,  5, 10, 10,  5,  0,-10,
	-10,  5,  5, 10, 10,  5,  5,-10,
	-10,  0, 10, 10, 10, 10,  0,-10,
	-10, 10, 10, 10, 10, 10, 10,-10,
	-10,  5,  0,  0,  0,  0,  5,-10,
	-20,-10,-10,-10,-10,-10,-10,-20
};
static constexpr std::array<int8_t, 64> BLACK_BISHOP_PST = {
	-20,-10,-10,-10,-10,-10,-10,-20,
	-10,  5,  0,  0,  0,  0,  5,-10,
	-10, 10, 10, 10, 10, 10, 10,-10,
	-10,  0, 10, 10, 10, 10,  0,-10,
	-10,  5,  5, 10, 10,  5,  5,-10,
	-10,  0,  5, 10, 10,  5,  0,-10,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-20,-10,-10,-10,-10,-10,-10,-20
};
static constexpr std::array<int8_t, 64> WHITE_ROOK_PST = {
	0,  0,  0,  0,  0,  0,  0,  0,
	5, 10, 10, 10, 10, 10, 10,  5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	0,  0,  0,  5,  5,  0,  0,  0
};
static constexpr std::array<int8_t, 64> BLACK_ROOK_PST = {
	0,  0,  0,  5,  5,  0,  0,  0
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	5, 10, 10, 10, 10, 10, 10,  5,
	0,  0,  0,  0,  0,  0,  0,  0,
};
static constexpr std::array<int8_t, 64> WHITE_QUEEN_PST = {
	-20,-10,-10, -5, -5,-10,-10,-20,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-10,  0,  5,  5,  5,  5,  0,-10,
	 -5,  0,  5,  5,  5,  5,  0, -5,
	  0,  0,  5,  5,  5,  5,  0, -5,
	-10,  5,  5,  5,  5,  5,  0,-10,
	-10,  0,  5,  0,  0,  0,  0,-10,
	-20,-10,-10, -5, -5,-10,-10,-20
};
static constexpr std::array<int8_t, 64> BLACK_QUEEN_PST = {
	-20,-10,-10, -5, -5,-10,-10,-20,
	-10,  0,  5,  0,  0,  0,  0,-10,
	-10,  5,  5,  5,  5,  5,  0,-10,
	  0,  0,  5,  5,  5,  5,  0, -5,
	 -5,  0,  5,  5,  5,  5,  0, -5,
	-10,  0,  5,  5,  5,  5,  0,-10,
	-10,  0,  0,  0,  0,  0,  0,-10
	-20,-10,-10, -5, -5,-10,-10,-20
};
static constexpr std::array<int8_t, 64> WHITE_KING_LATE_PST = {
	-50,-40,-30,-20,-20,-30,-40,-50,
	-30,-20,-10,  0,  0,-10,-20,-30,
	-30,-10, 20, 30, 30, 20,-10,-30,
	-30,-10, 30, 40, 40, 30,-10,-30,
	-30,-10, 30, 40, 40, 30,-10,-30,
	-30,-10, 20, 30, 30, 20,-10,-30,
	-30,-30,  0,  0,  0,  0,-30,-30,
	-50,-30,-30,-30,-30,-30,-30,-50
};
static constexpr std::array<int8_t, 64> BLACK_KING_LATE_PST = {
	-50,-30,-30,-30,-30,-30,-30,-50,
	-30,-30,  0,  0,  0,  0,-30,-30,
	-30,-10, 20, 30, 30, 20,-10,-30,
	-30,-10, 30, 40, 40, 30,-10,-30,
	-30,-10, 30, 40, 40, 30,-10,-30,
	-30,-10, 20, 30, 30, 20,-10,-30,
	-30,-20,-10,  0,  0,-10,-20,-30,
	-50,-40,-30,-20,-20,-30,-40,-50
};
static constexpr std::array<int8_t, 64> WHITE_KING_MIDDLE_PST = {
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	 20,-30,-30,-40,-40,-30,-30,-20,
	-10,-20,-20,-20,-20,-20,-20,-10,
	 20, 20,  0,  0,  0,  0, 20, 20,
	 20, 30, 10,  0,  0, 10, 30, 20
};
static constexpr std::array<int8_t, 64> BLACK_KING_MIDDLE_PST = {
	20, 30, 10,  0,  0, 10, 30, 20,
	20, 20,  0,  0,  0,  0, 20, 20,
	-10,-20,-20,-20,-20,-20,-20,-10,
	20,-30,-30,-40,-40,-30,-30,-20,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	 -30,-40,-40,-50,-50,-40,-40,-30,
	 -30,-40,-40,-50,-50,-40,-40,-30
};

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
	
	for (uint8_t i = 0; i < MAX_SQUARE; i++)
	{
		NumOfPieces += (bool)m_BoardSquare[i];
	}
}


int Evaluator::Evaluate()
{
	m_Evaluation = BoardMatValue() + MobilityEval() + PieceSquareEval();

	//m_Evaluation += m_LegalMoves.m_NumOfLegalMoves * 1 * m_SideToMove;
	return m_Evaluation * ((m_MoveNum % 2 == 0) ? 1 : -1);
}

int32_t Evaluator::BoardMatValue()
{
	int32_t MatEval = 0;
	
	for (const auto& Piece : m_BoardSquare)
	{
		if (Piece == 0)
			continue;
		if (Board::IsPieceColorWhite(Piece))
		{
			MatEval += ConvertPieceTypeToMatValue(Piece);
		}
		else
		{
			MatEval -= ConvertPieceTypeToMatValue(Piece);
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
			MobilityEval += m_LegalMoves.m_NumOfLegalMoves * 0.1;
		}
		else
		{
			MobilityEval -= m_LegalMoves.m_NumOfLegalMoves * 0.1;
		}
	}
	return MobilityEval;
}

int32_t Evaluator::PieceSquareEval() const
{
	int32_t Evaluation = 0;
	float LateWeight;
	for (uint8_t Square = 0; Square <= MAX_SQUARE; Square++)
	{
		switch (m_BoardSquare[Square])
		{
		case NONE:
			break;
		case WHITE_PAWN:
			Evaluation += WHITE_PAWN_PST[Square];
			break;
		case BLACK_PAWN:
			Evaluation -= BLACK_PAWN_PST[Square];
			break;
		case WHITE_BISHOP:
			Evaluation += WHITE_BISHOP_PST[Square];
			break;
		case BLACK_BISHOP:
			Evaluation -= BLACK_BISHOP_PST[Square];
			break;
		case WHITE_ROOK:
			Evaluation += WHITE_ROOK_PST[Square];
			break;
		case BLACK_ROOK:
			Evaluation -= BLACK_ROOK_PST[Square];
			break;
		case WHITE_KNIGHT:
			Evaluation += WHITE_KNIGHT_PST[Square];
			break;
		case BLACK_KNIGHT:
			Evaluation -= BLACK_KNIGHT_PST[Square];
			break;
		case WHITE_QUEEN:
			Evaluation += WHITE_QUEEN_PST[Square];
			break;
		case BLACK_QUEEN:
			Evaluation -= BLACK_QUEEN_PST[Square];
			break;
		case WHITE_KING:
		{
			LateWeight = 1.f - (NumOfPieces/36.f);
			Evaluation += WHITE_KING_MIDDLE_PST[Square] * (1.f - LateWeight);
			Evaluation += WHITE_KING_LATE_PST[Square] * LateWeight;
			break;
		}
		case BLACK_KING:
			LateWeight = 1.f - (NumOfPieces / 36.f);
			Evaluation -= BLACK_KING_MIDDLE_PST[Square] * (1.f - LateWeight);
			Evaluation -= BLACK_KING_LATE_PST[Square] * LateWeight;
			break;
		default:
			__debugbreak();//something is fucked up
			break;
		}
	}

	return Evaluation;
}


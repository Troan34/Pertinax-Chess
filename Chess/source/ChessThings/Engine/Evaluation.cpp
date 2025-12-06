#include "Evaluation.h"
//Michniewski's tables
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

void Evaluator::SetParameters(const bit::Position& f_ChessPosition)
{
	ChessPosition = f_ChessPosition;
	
	NumOfPieces += ChessPosition.BoardSquare.popcnt();
}


int Evaluator::Evaluate()
{
	m_Evaluation = BoardMatValue() + MobilityEval() + PieceSquareEval();

	//m_Evaluation += m_LegalMoves.m_NumOfLegalMoves * 1 * m_SideToMove;
	return m_Evaluation * ((WHITE_TURN(ChessPosition.MoveNum)) ? 1 : -1);
}

int32_t Evaluator::BoardMatValue()
{
	int32_t MatEval = 0;

	for (uint8_t PieceType = PAWN; PieceType <= KING; PieceType++)
	{
		MatEval += (ConvertPieceTypeToMatValue(PieceType) *
			(ChessPosition.BoardSquare.ColorPositions[0] | ChessPosition.BoardSquare.PiecePositions[PieceType - 1]).popcnt());

		MatEval -= (ConvertPieceTypeToMatValue(PieceType) *
			(ChessPosition.BoardSquare.ColorPositions[1] | ChessPosition.BoardSquare.PiecePositions[PieceType - 1]).popcnt());
	}

	return MatEval;
}

int32_t Evaluator::MobilityEval()
{
	int32_t MobilityEval = 0;

	MobilityEval += (m_LegalMoves.AttackedSquares & (~ChessPosition.BoardSquare.ColorPositions[0])).popcnt();
	MobilityEval += (m_LegalMoves.OppositeAttackedSquares & (~ChessPosition.BoardSquare.ColorPositions[1])).popcnt();

	return MobilityEval * 0.1f;
}

int32_t Evaluator::PieceSquareEval() const
{
    int32_t Evaluation = 0;

    float LateWeight = 1.f - (NumOfPieces / 36.f);

    bit::BitBoard64 bits = 0;
    uint8_t Index = 0;


    // Pawns
    bits = (uint64_t)ChessPosition.BoardSquare.PiecePositions[PAWN - 1] & (uint64_t)ChessPosition.BoardSquare.ColorPositions[0];
    while (bit::pop_lsb(bits, Index)) { Evaluation += WHITE_PAWN_PST[Index]; }
    bits = (uint64_t)ChessPosition.BoardSquare.PiecePositions[PAWN - 1] & (uint64_t)ChessPosition.BoardSquare.ColorPositions[1];
    while (bit::pop_lsb(bits, Index)) { Evaluation -= BLACK_PAWN_PST[Index]; }

    // Bishops
    bits = (uint64_t)ChessPosition.BoardSquare.PiecePositions[BISHOP - 1] & (uint64_t)ChessPosition.BoardSquare.ColorPositions[0];
    while (bit::pop_lsb(bits, Index)) { Evaluation += WHITE_BISHOP_PST[Index]; }
    bits = (uint64_t)ChessPosition.BoardSquare.PiecePositions[BISHOP - 1] & (uint64_t)ChessPosition.BoardSquare.ColorPositions[1];
    while (bit::pop_lsb(bits, Index)) { Evaluation -= BLACK_BISHOP_PST[Index]; }

    // Rooks
    bits = (uint64_t)ChessPosition.BoardSquare.PiecePositions[ROOK - 1] & (uint64_t)ChessPosition.BoardSquare.ColorPositions[0];
    while (bit::pop_lsb(bits, Index)) { Evaluation += WHITE_ROOK_PST[Index]; }
    bits = (uint64_t)ChessPosition.BoardSquare.PiecePositions[ROOK - 1] & (uint64_t)ChessPosition.BoardSquare.ColorPositions[1];
    while (bit::pop_lsb(bits, Index)) { Evaluation -= BLACK_ROOK_PST[Index]; }

    // Knights
    bits = (uint64_t)ChessPosition.BoardSquare.PiecePositions[KNIGHT - 1] & (uint64_t)ChessPosition.BoardSquare.ColorPositions[0];
    while (bit::pop_lsb(bits, Index)) { Evaluation += WHITE_KNIGHT_PST[Index]; }
    bits = (uint64_t)ChessPosition.BoardSquare.PiecePositions[KNIGHT - 1] & (uint64_t)ChessPosition.BoardSquare.ColorPositions[1];
    while (bit::pop_lsb(bits, Index)) { Evaluation -= BLACK_KNIGHT_PST[Index]; }

    // Queens
    bits = (uint64_t)ChessPosition.BoardSquare.PiecePositions[QUEEN - 1] & (uint64_t)ChessPosition.BoardSquare.ColorPositions[0];
    while (bit::pop_lsb(bits, Index)) { Evaluation += WHITE_QUEEN_PST[Index]; }
    bits = (uint64_t)ChessPosition.BoardSquare.PiecePositions[QUEEN - 1] & (uint64_t)ChessPosition.BoardSquare.ColorPositions[1];
    while (bit::pop_lsb(bits, Index)) { Evaluation -= BLACK_QUEEN_PST[Index]; }

    // Kings
    bits = (uint64_t)ChessPosition.BoardSquare.PiecePositions[KING - 1] & (uint64_t)ChessPosition.BoardSquare.ColorPositions[0];
    while (bit::pop_lsb(bits, Index)) {
        Evaluation += static_cast<int>(WHITE_KING_MIDDLE_PST[Index] * (1.f - LateWeight) + WHITE_KING_LATE_PST[Index] * LateWeight);
    }
    bits = (uint64_t)ChessPosition.BoardSquare.PiecePositions[KING - 1] & (uint64_t)ChessPosition.BoardSquare.ColorPositions[1];
    while (bit::pop_lsb(bits, Index)) {
        Evaluation -= static_cast<int>(BLACK_KING_MIDDLE_PST[Index] * (1.f - LateWeight) + BLACK_KING_LATE_PST[Index] * LateWeight);
    }

    return Evaluation;
}


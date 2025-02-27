#pragma once
#include <cstdint>
#include <ChessThings/Board.h>
#include "ChessThings/LegalMoves.h"
#include <cmath>

struct EvalMove
{
	uint8_t BoardSquarePos;
	uint8_t MovePos;
	int32_t Eval = -INT32_MAX;
	uint8_t PieceToPromoteTo;
};

class Evaluator
{
private:
	static constexpr int16_t M_PAWN_MATERIAL_VALUE = 100;
	static constexpr int16_t M_KNIGHT_MATERIAL_VALUE = 300;
	static constexpr int16_t M_BISHOP_MATERIAL_VALUE = 310;
	static constexpr int16_t M_ROOK_MATERIAL_VALUE = 400;
	static constexpr int16_t M_QUEEN_MATERIAL_VALUE = 900;

	static constexpr int16_t ConvertPieceTypeToMatValue(const uint8_t& PieceType)
	{
		switch (PieceType)
		{
		case BLACK_PAWN:
			return M_PAWN_MATERIAL_VALUE;
		case BLACK_BISHOP:
			return M_BISHOP_MATERIAL_VALUE;
		case BLACK_KNIGHT:
			return M_KNIGHT_MATERIAL_VALUE;
		case BLACK_ROOK:
			return M_ROOK_MATERIAL_VALUE;
		case BLACK_QUEEN:
			return M_QUEEN_MATERIAL_VALUE;
		case WHITE_PAWN:
			return M_PAWN_MATERIAL_VALUE;
		case WHITE_BISHOP:
			return M_BISHOP_MATERIAL_VALUE;
		case WHITE_KNIGHT:
			return M_KNIGHT_MATERIAL_VALUE;
		case WHITE_ROOK:
			return M_ROOK_MATERIAL_VALUE;
		case WHITE_QUEEN:
			return M_QUEEN_MATERIAL_VALUE;
		}
	}

	const std::array<uint8_t, 64>& m_BoardSquare;
	const std::array<uint8_t, 64>& m_PreviousBoardSquare;
	const canCastle& m_CanCastle;

	const GenerateLegalMoves m_LegalMoves;


	int CapturingEval(const uint8_t& CapturingPieceType, const uint8_t& CapturedPieceType);

public:
	
	Evaluator(const GenerateLegalMoves& LegalMoves, const std::array<uint8_t, 64>& BoardSquare, const std::array<uint8_t, 64>& PreviousBoardSquare, const canCastle& CanCastle);
	int Evaluate(const uint8_t& BoardSquarePos, const uint8_t& Move);


};
#pragma once
#include <cstdint>
#include <ChessThings/Board.h>
#include "ChessThings/LegalMoves.h"
#include <cmath>


class Evaluator
{
private:
	static constexpr int16_t M_PAWN_MATERIAL_VALUE = 100;
	static constexpr int16_t M_KNIGHT_MATERIAL_VALUE = 300;
	static constexpr int16_t M_BISHOP_MATERIAL_VALUE = 310;
	static constexpr int16_t M_ROOK_MATERIAL_VALUE = 400;
	static constexpr int16_t M_QUEEN_MATERIAL_VALUE = 900;
	static constexpr int16_t M_KING_MATERIAL_VALUE = 15000;

	const GenerateLegalMoves& m_LegalMoves;
	std::array<uint8_t, 64> m_BoardSquare; // Change reference to value
	std::array<uint8_t, 64> m_PreviousBoardSquare; // Change reference to value
	canCastle m_CanCastle; // Change reference to value
	int32_t m_Evaluation = 0;
	int32_t m_MoveNum;
	uint8_t NumOfPieces = 0;
	
	//self describing
	int32_t BoardMatValue();

	int32_t MobilityEval();

	//This does the PST additions
	int32_t PieceSquareEval() const;
public:
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
		case BLACK_KING:
			return M_KING_MATERIAL_VALUE;
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
		case WHITE_KING:
			return M_KING_MATERIAL_VALUE;
		}
	}

	Evaluator(const GenerateLegalMoves& LegalMoves);
	void SetParameters(const std::array<uint8_t, 64>& BoardSquare, const std::array<uint8_t, 64>& PreviousBoardSquare, const canCastle& CanCastle, const uint8_t& MoveNum);
	int Evaluate();
};
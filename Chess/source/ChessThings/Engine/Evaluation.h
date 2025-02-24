#pragma once
#include <cstdint>
#include <ChessThings/Board.h>

class Evaluator
{
private:
	static constexpr uint8_t m_PawnMaterialValue = 100;
	static constexpr uint8_t m_KnightMaterialValue = 300;
	static constexpr uint8_t m_BishopMaterialValue = 310;
	static constexpr uint8_t m_RookMaterialValue = 400;
	static constexpr uint8_t m_QueenMaterialValue = 900;

public:
	
	int Evaluate(const uint8_t& BoardSquarePos, const uint8_t& Move, const std::array<uint8_t, 64>& BoardSquare, const std::array<uint8_t, 64>& PreviousBoardSquare, const canCastle& CanCastle);
};
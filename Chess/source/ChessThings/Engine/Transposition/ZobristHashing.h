#pragma once  
#include <cstdint>  
#include <array>  
#include <random>  
#include "ChessThings/LegalMoves.h"

class ZobristHashing  
{  
private:
   const GenerateLegalMoves* m_LegalMoves;
   const std::array<uint8_t, 64>* m_BoardSquare;
   const std::array<uint8_t, 64>* m_PreviousBoardSquare;
   const CastlingAbility m_CastleAbility;
   bool m_SideToMove = false; // true for white, false for black

   void InitializeKeys();
   void CreateInitialHash();

public:
    uint64_t Hash = 0; // current hash

    ZobristHashing(const GenerateLegalMoves& LegalMoves, const std::array<uint8_t, 64>& BoardSquare, const std::array<uint8_t, 64>& PreviousBoardSquare, const canCastle& CanCastle, const uint32_t& MoveNum);
    void UpdateHash(Move Move_, const uint8_t& PieceType);

};  
#pragma once  
#include <cstdint>  
#include <array>  
#include <random>  
#include "ChessThings/LegalMoves.h"

class ZobristHashing  
{  
private:
   GenerateLegalMoves* m_LegalMoves;
   std::array<uint8_t, 64>* m_BoardSquare;
   std::array<uint8_t, 64>* m_PreviousBoardSquare;
   CastlingAbility m_CastleAbility;
   bool m_SideToMove = false; // true for white, false for black

   void InitializeKeys();
   void CreateInitialHash();

public:
    uint64_t m_Hash = 0; // current hash

    ZobristHashing(GenerateLegalMoves* LegalMoves, std::array<uint8_t, 64>* BoardSquare, std::array<uint8_t, 64>* PreviousBoardSquare, const canCastle& CanCastle, const uint32_t& MoveNum);
    void UpdateHash(const uint8_t& BoardSquare, const uint8_t& move, const uint8_t& PieceType, const uint8_t& PieceTypeToPromoteTo);

};  
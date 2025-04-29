#pragma once  
#include <cstdint>  
#include <array>  
#include <random>  
#include "ChessThings/Board.h"
#include "ChessThings/LegalMoves.h"

class ZobristHashing  
{  
private:  


   

   uint64_t m_Hash = 0; // current hash
   GenerateLegalMoves* m_LegalMoves;
   std::array<uint8_t, 64>* m_BoardSquare;
   std::array<uint8_t, 64>* m_PreviousBoardSquare;
   CastlingAbility m_CastleAbility;
   bool m_SideToMove = false; // true for white, false for black

   void InitializeKeys();
   void CreateInitialHash();
   void UpdateHash(const uint8_t& BoardSquare, const uint8_t& move, const uint8_t& PieceType, const uint8_t& PieceTypeToPromoteTo);

public:  
    ZobristHashing(std::array<uint8_t, 64>* BoardSquare, std::array<uint8_t, 64>* PreviousBoardSquare, const canCastle& CanCastle, GenerateLegalMoves* LegalMoves, const uint32_t& MoveNum);


};  
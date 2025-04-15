#pragma once  
#include <cstdint>  
#include <array>  
#include <random>  
#include "ChessThings/Board.h"
#include "ChessThings/LegalMoves.h"

class ZobristHashing  
{  
private:  

   static std::default_random_engine rng;  
   static std::uniform_int_distribution<uint64_t> dist;  

   static uint64_t Random64Bit()
   {
       return dist(rng);
   }

   static std::array<std::array<uint64_t, 64>, 12> ZobristPieces; // 2d array: 64 squares for the 12 piece types  
   static std::array<uint64_t, 4> ZobristCastlingRights; // castling rights, order: KQkq 
   static std::array<uint64_t, 8> ZobristEnPassant; // en passant squares  
   static uint64_t ZobristSideToMove; // hash for black to move
   

   uint64_t m_Hash = 0; // current hash
   GenerateLegalMoves* m_LegalMoves;
   std::array<uint8_t, 64> m_BoardSquare;
   CastlingAbility m_CastleAbility;
   bool m_SideToMove = false; // true for white, false for black

   void InitializeKeys();
   void CreateInitialHash();
   void UpdateHash(const uint8_t& BoardSquare, const uint8_t& move, const uint8_t& PieceType, const uint8_t& PieceTypeToPromoteTo);

public:  
    ZobristHashing(const std::array<uint8_t, 64>& BoardSquare, const canCastle& CanCastle, GenerateLegalMoves* LegalMoves, const uint32_t& MoveNum);


};  
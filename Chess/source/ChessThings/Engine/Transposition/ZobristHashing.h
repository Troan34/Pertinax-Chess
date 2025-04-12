#pragma once  
#include <cstdint>  
#include <array>  
#include <random>  
#include "ChessThings/Board.h"

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
   static std::array<uint64_t, 4> ZobristCastlingRights; // castling rights  
   static std::array<uint64_t, 8> ZobristEnPassant; // en passant squares  
   static uint64_t ZobristSideToMove; // hash for black to move


   void InitializeKeys()  
   {  
       for (auto& piece : ZobristPieces)  
       {  
           for (auto& square : piece)  
           {  
               square = Random64Bit();  
           }  
       }  

       for (auto& castling : ZobristCastlingRights)  
       {  
           castling = Random64Bit();  
       }  

       for (auto& enPassant : ZobristEnPassant)  
       {  
           enPassant = Random64Bit();  
       }

	   ZobristSideToMove = Random64Bit();
   }  

   std::array<uint8_t, 64> BoardSquare;
   canCastle CanCastle;

   ////////////////TODO : PUT EVERYTHING IN THE .cpp
   void CreateInitialHash()
   {
       uint64_t hash = 0;

	   for (uint8_t i = 0; i < 64; ++i)
	   {
		   uint8_t piece = BoardSquare[i];
		   if (piece != 0)
		   {
			   hash ^= ZobristPieces[piece][i]; // XOR the hash with the piece's hash
		   }
	   }

	   for (uint8_t i = 0; i < 4; i++)
	   {
		   if (CanCastle.)//fix the canCastle struct
		   {
			   hash ^= ZobristCastlingRights[]; // XOR the hash with the castling rights
		   }
	   }
   }

public:  
    ZobristHashing(const std::array<uint8_t, 64>& BoardSquare, const canCastle& CanCastle)
		:BoardSquare(BoardSquare),
		 CanCastle(CanCastle)
	{
		InitializeKeys();
	}


};  
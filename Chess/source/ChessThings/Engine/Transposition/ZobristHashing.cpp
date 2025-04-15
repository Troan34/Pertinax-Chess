#include "ZobristHashing.h"

ZobristHashing::ZobristHashing(const std::array<uint8_t, 64>& BoardSquare, const canCastle& CanCastle, GenerateLegalMoves* LegalMoves, const uint32_t& MoveNum)
	:m_BoardSquare(BoardSquare),
	m_LegalMoves(LegalMoves)
{
	m_CastleAbility = Board::canCastle2CastlingAbility(CanCastle);
	m_SideToMove = MoveNum % 2 == 0; // true for white, false for black
	InitializeKeys();

}

void ZobristHashing::InitializeKeys()
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

void ZobristHashing::CreateInitialHash()
{

    for (uint8_t i = 0; i < 64; ++i)
    {
        uint8_t piece = m_BoardSquare[i];
        if (piece != 0)
        {
            m_Hash ^= ZobristPieces[piece][i]; // XOR the hash with the piece's hash
        }
    }

    for (uint8_t i = 0; i < 4; i++)
    {
        if (m_CastleAbility[i])//fix the canCastle struct
        {
            m_Hash ^= ZobristCastlingRights[i]; // XOR the hash with the castling rights
        }
    }
    
	for (uint8_t i = 0; i < 8; ++i)
	{
		if (m_LegalMoves->EnPassantFiles[i] == true)
		{
			m_Hash ^= ZobristEnPassant[i]; // XOR the hash with the en passant square
		}
	}

	if (!m_SideToMove)
	{
		m_Hash ^= ZobristSideToMove; // XOR the hash with the side to move
	}
}

void ZobristHashing::UpdateHash(const uint8_t& StartingSquare, const uint8_t& move, const uint8_t& PieceType, const uint8_t& PieceTypeToPromoteTo)
{
    if (PieceTypeToPromoteTo != 65)
    {
		m_Hash ^= ZobristPieces[PieceType][StartingSquare]; // XOR piece removal
		m_Hash ^= ZobristPieces[PieceTypeToPromoteTo][move]; // XOR placing and promoting
	}
    else
    {
        m_Hash ^= ZobristPieces[PieceType][StartingSquare]; // XOR piece removal
		m_Hash ^= ZobristPieces[PieceType][move]; // XOR placing
    }

	//castling
	if (m_BoardSquare[StartingSquare] == WHITE_KING or m_BoardSquare[StartingSquare] == BLACK_KING)
	{
		if (StartingSquare - move == -2)
		{
			if (StartingSquare == 4)
			{
				m_Hash ^= ZobristPieces[ROOK][7];
				m_Hash ^= ZobristPieces[ROOK][5];
			}
			else
			{
				m_Hash ^= ZobristPieces[ROOK][63];
				m_Hash ^= ZobristPieces[ROOK][61];
			}
		}
		if (StartingSquare - move == 2)
		{
			if (StartingSquare == 4)
			{
				m_Hash ^= ZobristPieces[ROOK][0];
				m_Hash ^= ZobristPieces[ROOK][3];
			}
			else
			{
				m_Hash ^= ZobristPieces[ROOK][56];
				m_Hash ^= ZobristPieces[ROOK][59];
			}
		}

	}

	//TODO: Figure out how to get previousBoardSquare efficiently
	if (m_BoardSquare[BoardSquare] == WHITE_PAWN)
	{
		if (fun_previousBoardSquare[move] == 0)
		{
			if (BoardSquare - move == -7 or BoardSquare - move == -9)
			{
				fun_BoardSquare[move - 8] = 0;
			}
		}
	}
	//Black en passant
	if (m_BoardSquare[BoardSquare] == BLACK_PAWN)
	{
		if (fun_previousBoardSquare[move] == 0)
		{
			if (BoardSquare - move == 7 or BoardSquare - move == 9)
			{
				fun_BoardSquare[move + 8] = 0;
			}
		}
	}
}

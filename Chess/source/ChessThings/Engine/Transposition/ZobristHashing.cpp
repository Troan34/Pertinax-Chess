#include "ZobristHashing.h"

ZobristHashing::ZobristHashing(std::array<uint8_t, 64>* BoardSquare, std::array<uint8_t, 64>* PreviousBoardSquare, const canCastle& CanCastle, GenerateLegalMoves* LegalMoves, const uint32_t& MoveNum)
	:m_BoardSquare(BoardSquare),
	m_LegalMoves(LegalMoves),
	m_PreviousBoardSquare(PreviousBoardSquare)
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
        uint8_t piece = m_BoardSquare->at(i);
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
	uint8_t f_PieceType = Board::GetPieceType2Uncolored(PieceType);
	if (Board::WillCanCastleChange(PieceType, StartingSquare, move))
	{
		if (PieceType == WHITE_KING)
		{
			m_Hash ^= ZobristCastlingRights[0];
			m_Hash ^= ZobristCastlingRights[1];
		}
		else if (PieceType == WHITE_ROOK)
		{
			if (StartingSquare == 0)
			{
				m_Hash ^= ZobristCastlingRights[1];
			}
			else
			{
				m_Hash ^= ZobristCastlingRights[0];
			}
		}
		else if (PieceType == BLACK_KING)
		{
			m_Hash ^= ZobristCastlingRights[2];
			m_Hash ^= ZobristCastlingRights[3];
		}
		else if (PieceType == BLACK_ROOK)
		{
			if (StartingSquare == 56)
			{
				m_Hash ^= ZobristCastlingRights[3];
			}
			else
			{
				m_Hash ^= ZobristCastlingRights[2];
			}
		}
	}

    if (PieceTypeToPromoteTo != 65)
    {
		m_Hash ^= ZobristPieces[f_PieceType][StartingSquare]; // XOR piece removal
		m_Hash ^= ZobristPieces[Board::GetPieceType2Uncolored(PieceTypeToPromoteTo)][move]; // XOR placing and promoting
	}
    else
    {
        m_Hash ^= ZobristPieces[f_PieceType][StartingSquare]; // XOR piece removal
		m_Hash ^= ZobristPieces[f_PieceType][move]; // XOR placing
    }

	//castling
	if (f_PieceType == KING)
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

	//white en passant
	if (PieceType == WHITE_PAWN)
	{
		if (m_PreviousBoardSquare->at(move) == 0)
		{
			if (StartingSquare - move == -7 or StartingSquare - move == -9)
			{
				m_Hash ^= ZobristPieces[PAWN][move - 8];
			}
		}
	}
	//Black en passant
	if (PieceType == BLACK_PAWN)
	{
		if (m_PreviousBoardSquare->at(move) == 0)
		{
			if (StartingSquare - move == 7 or StartingSquare - move == 9)
			{
				m_Hash ^= ZobristPieces[PAWN][move + 8];
			}
		}
	}

	//TODO: add en passant hashing and side to move
}

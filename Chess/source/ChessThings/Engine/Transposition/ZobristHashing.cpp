#include "ZobristHashing.h"

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
static bool Initialized = false;
static uint8_t WhichFileHadEnPassant = 8; //8 for none


ZobristHashing::ZobristHashing(GenerateLegalMoves* LegalMoves, std::array<uint8_t, 64>* BoardSquare, std::array<uint8_t, 64>* PreviousBoardSquare, const canCastle& CanCastle, const uint32_t& MoveNum)
	:m_BoardSquare(BoardSquare),
	m_LegalMoves(LegalMoves),
	m_PreviousBoardSquare(PreviousBoardSquare)
{
	m_CastleAbility = Board::canCastle2CastlingAbility(CanCastle);
	m_SideToMove = MoveNum % 2 == 0; // true for white, false for black
	if (!Initialized)
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
	Initialized = true;
}

void ZobristHashing::CreateInitialHash()
{

    for (uint8_t i = 0; i < 64; ++i)
    {
        uint8_t piece = m_BoardSquare->at(i);
        if (piece != 0)
        {
            Hash ^= ZobristPieces[piece][i]; // XOR the hash with the piece's hash
        }
    }

    for (uint8_t i = 0; i < 4; i++)
    {
        if (m_CastleAbility[i])//fix the canCastle struct
        {
            Hash ^= ZobristCastlingRights[i]; // XOR the hash with the castling rights
        }
    }
    
	for (uint8_t i = 0; i < 8; ++i)
	{
		if (m_LegalMoves->EnPassantFiles[i] == true)
		{
			Hash ^= ZobristEnPassant[i]; // XOR the hash with the en passant square
		}
	}

	if (!m_SideToMove)
	{
		Hash ^= ZobristSideToMove; // XOR the hash with the side to move
	}
}

void ZobristHashing::UpdateHash(const uint8_t& StartingSquare, const uint8_t& move, const uint8_t& PieceType, const uint8_t& PieceTypeToPromoteTo)
{
	uint8_t f_PieceType = Board::GetPieceType2Uncolored(PieceType);
	if (Board::WillCanCastleChange(PieceType, StartingSquare, move))
	{
		if (PieceType == WHITE_KING)
		{
			Hash ^= ZobristCastlingRights[0];
			Hash ^= ZobristCastlingRights[1];
		}
		else if (PieceType == WHITE_ROOK)
		{
			if (StartingSquare == 0)
			{
				Hash ^= ZobristCastlingRights[1];
			}
			else
			{
				Hash ^= ZobristCastlingRights[0];
			}
		}
		else if (PieceType == BLACK_KING)
		{
			Hash ^= ZobristCastlingRights[2];
			Hash ^= ZobristCastlingRights[3];
		}
		else if (PieceType == BLACK_ROOK)
		{
			if (StartingSquare == 56)
			{
				Hash ^= ZobristCastlingRights[3];
			}
			else
			{
				Hash ^= ZobristCastlingRights[2];
			}
		}
	}

    if (PieceTypeToPromoteTo != 65)
    {
		Hash ^= ZobristPieces[f_PieceType][StartingSquare]; // XOR piece removal
		Hash ^= ZobristPieces[Board::GetPieceType2Uncolored(PieceTypeToPromoteTo)][move]; // XOR placing and promoting
	}
    else
    {
        Hash ^= ZobristPieces[f_PieceType][StartingSquare]; // XOR piece removal
		Hash ^= ZobristPieces[f_PieceType][move]; // XOR placing
    }

	//castling
	if (f_PieceType == KING)
	{
		if (StartingSquare - move == -2)
		{
			if (StartingSquare == 4)
			{
				Hash ^= ZobristPieces[ROOK][7];
				Hash ^= ZobristPieces[ROOK][5];
			}
			else
			{
				Hash ^= ZobristPieces[ROOK][63];
				Hash ^= ZobristPieces[ROOK][61];
			}
		}
		if (StartingSquare - move == 2)
		{
			if (StartingSquare == 4)
			{
				Hash ^= ZobristPieces[ROOK][0];
				Hash ^= ZobristPieces[ROOK][3];
			}
			else
			{
				Hash ^= ZobristPieces[ROOK][56];
				Hash ^= ZobristPieces[ROOK][59];
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
				Hash ^= ZobristPieces[PAWN][move - 8];
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
				Hash ^= ZobristPieces[PAWN][move + 8];
			}
		}
	}

	if (WhichFileHadEnPassant != 8)
	{
		Hash ^= ZobristEnPassant[WhichFileHadEnPassant]; // XOR the hash with the en passant square
		WhichFileHadEnPassant = 8; // reset to none
	}

	for (uint8_t index = 0; index < 8; index++)
	{
		if (m_LegalMoves->EnPassantFiles[index] == true)
		{
			Hash ^= ZobristEnPassant[index]; // XOR the hash with the en passant square
			WhichFileHadEnPassant = index;
		}
	}
}

#include "Search.h"

Search::Search(std::array<uint8_t, 64> BoardSquare, std::array<uint8_t, 64> PreviousBoardSquare, canCastle CanCastle, uint8_t depth, uint16_t MoveNum)
	:m_BoardSquare(BoardSquare), m_PreviousBoardSquare(PreviousBoardSquare), m_CanCastle(CanCastle), m_depth(depth), m_MoveNum(MoveNum)
{

}

Search::~Search()
{
}

int Search::LoopThroughTheTree()//TODO: make parameters, finish the basic function
{
	int Evaluation = 0;
	int BestEvaluation = -INT64_MAX;

	GenerateLegalMoves LegalMoves(m_BoardSquare, &m_PreviousBoardSquare, m_CanCastle, (m_MoveNum % 2 != 0) ? false : true, m_MoveNum, false);
	Evaluator evaluator(LegalMoves, m_BoardSquare, m_PreviousBoardSquare, m_CanCastle);

	uint8_t count = 0;

	for (MOVE& piece : LegalMoves.moves)
	{
		for (const uint8_t& move : piece.TargetSquares)
		{

			//perft the promotions, this if is basically a blunt .find()
			if (piece.Promotion[0] != 65 and piece.Promotion[0] == move or piece.Promotion[1] != 65 and piece.Promotion[1] == move or piece.Promotion[2] != 65 and piece.Promotion[2] == move)
			{
				bool IsWhite = Board::IsPieceColorWhite(m_BoardSquare[count]);

				for (uint8_t i = 0; i != 4; ++i)
				{
					if (m_depth == 1)
					{
						int Evaluation = evaluator.Evaluate(count, move);

						if(Evaluation > BestEvaluation)
							BestEvaluation = Evaluation;

						continue;
					}
					else
					{

						if (IsWhite)
							MakeMove(LegalMoves, count, move, m_BoardSquare, m_PreviousBoardSquare, m_CanCastle, i + 18);
						else
							MakeMove(LegalMoves, count, move, m_BoardSquare, m_PreviousBoardSquare, m_CanCastle, i + 10);

						LoopThroughTheTree();

						m_BoardSquare = m_PreviousBoardSquare;
					}
				}
				/*
				if (m_depth != 1)
				{
					BoardSquare = ConstBoardSquare;
					perftPreviousBoardSquare = ConstPreviousBoardSquare;
					CanCastle = ConstCanCastle;
				}*/

				if (IsWhite)
					piece.Promotion[move - count - 7] = 65;
				else
					piece.Promotion[move - count + 9] = 65;

				continue;
			}

			if (m_depth == 1)
			{

			}
			else
			{

				MakeMove(LegalMoves, count, move, m_BoardSquare, m_PreviousBoardSquare, m_CanCastle, 65);

				LoopThroughTheTree();

			}
		}
		count++;
	}

}

void Search::MakeMove(const GenerateLegalMoves& LegalMoves, const uint8_t& BoardSquare, const uint8_t& move, std::array<uint8_t, 64>& fun_BoardSquare, std::array<uint8_t, 64>& fun_previousBoardSquare, canCastle& Castle, const uint8_t& PieceTypeToPromoteTo)
{
	fun_previousBoardSquare = fun_BoardSquare;
	Board::WillCanCastleChange(fun_BoardSquare[BoardSquare], BoardSquare, move, Castle);
	fun_BoardSquare[move] = fun_BoardSquare[BoardSquare];

	//castling
	if (fun_BoardSquare[BoardSquare] == WHITE_KING or fun_BoardSquare[BoardSquare] == BLACK_KING)
	{
		if (BoardSquare - move == -2)
		{
			if (BoardSquare == 4)
			{
				fun_BoardSquare[5] = WHITE_ROOK;
				fun_BoardSquare[7] = 0;
			}
			else
			{
				fun_BoardSquare[61] = BLACK_ROOK;
				fun_BoardSquare[63] = 0;
			}
		}
		if (BoardSquare - move == 2)
		{
			if (BoardSquare == 4)
			{
				fun_BoardSquare[3] = WHITE_ROOK;
				fun_BoardSquare[0] = 0;
			}
			else
			{
				fun_BoardSquare[59] = BLACK_ROOK;
				fun_BoardSquare[56] = 0;
			}
		}

	}

	//promoting and en passant
	if (PieceTypeToPromoteTo != 65)
	{
		fun_BoardSquare[move] = PieceTypeToPromoteTo;
	}
	else//optimization
	{
		//White en passant
		if (fun_BoardSquare[BoardSquare] == WHITE_PAWN)
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
		if (fun_BoardSquare[BoardSquare] == BLACK_PAWN)
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

	fun_BoardSquare[BoardSquare] = 0;

	GenerateLegalMoves::SetDoNotEnPassant(false);
	if (abs(BoardSquare - move) == 16 and LegalMoves.WhichBoardSquaresAreAbsPinned[move] != 65)
	{
		GenerateLegalMoves::SetDoNotEnPassant(true);
	}
}
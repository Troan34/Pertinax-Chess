#include "Search.h"
#include <chrono>

Search::Search(std::array<uint8_t, 64> BoardSquare, std::array<uint8_t, 64> PreviousBoardSquare, canCastle CanCastle, uint8_t depth, uint16_t MoveNum)
	:m_BoardSquare(BoardSquare), m_PreviousBoardSquare(PreviousBoardSquare), m_CanCastle(CanCastle), m_depth(depth), m_MoveNum(MoveNum)
{

}

Search::~Search()
{
}

std::pair<std::pair<uint8_t, uint8_t>, uint8_t> Search::GetBestMove()
{
	auto start = std::chrono::high_resolution_clock::now();
	NegaMax(m_BoardSquare, m_PreviousBoardSquare, m_CanCastle, m_MoveNum, m_depth, -INT32_MAX, INT32_MAX);
	std::pair<uint8_t, uint8_t> pair(m_BestBoardPos, m_BestMove);
	std::pair<std::pair<uint8_t, uint8_t>, uint8_t> ppair(pair, m_BestPromotion);
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	std::cout << "In " << duration.count() << " ms" << '\n' << std::endl;
	return ppair;
}

int Search::NegaMax(std::array<uint8_t, 64Ui64> BoardSquare, std::array<uint8_t, 64> previousBoardSquare, canCastle CanCastle, uint8_t MoveNum, uint8_t depth, int32_t alpha, int32_t beta)
{
	int Evaluation;
	int BestEval = -INT32_MAX;

	GenerateLegalMoves LegalMoves(BoardSquare, &previousBoardSquare, CanCastle, (MoveNum % 2 != 0) ? false : true, MoveNum, false);
	Evaluator evaluator(LegalMoves, BoardSquare, previousBoardSquare, CanCastle);

	uint8_t count = 0;

	auto const cBoardSquare = BoardSquare;
	auto const cPreviousBoardSquare = previousBoardSquare;
	auto const cCanCastle = CanCastle;
	auto const cMoveNum = MoveNum;
	bool BreakFlag = false;


	for (MOVE& piece : LegalMoves.moves)
	{
		if (BreakFlag)
		{
			BreakFlag = false;
			break;
		}
		
		for (const uint8_t& move : piece.TargetSquares)
		{

			if (piece.Promotion[0] != 65 and piece.Promotion[0] == move or piece.Promotion[1] != 65 and piece.Promotion[1] == move or piece.Promotion[2] != 65 and piece.Promotion[2] == move)
			{
				bool IsWhite = Board::IsPieceColorWhite(BoardSquare[count]);

				for (uint8_t i = 0; i != 4; ++i)
				{

					if (depth == 0)
					{
						Evaluation = evaluator.Evaluate(count, move);

						if (Evaluation > BestEval)
						{
							BestEval = Evaluation;
						}
					}
					else
					{

						if (IsWhite)
							MakeMove(LegalMoves, count, move, BoardSquare, previousBoardSquare, CanCastle, i + 18);
						else
							MakeMove(LegalMoves, count, move, BoardSquare, previousBoardSquare, CanCastle, i + 10);

						Evaluation = std::max(Evaluation, -NegaMax(BoardSquare, previousBoardSquare, CanCastle, MoveNum + 1, depth - 1, -beta, -alpha));
						alpha = std::max(alpha, Evaluation);

						if (alpha >= beta)
						{
							std::cout << "Pruning" << std::endl;
							Evaluation = true;
							BreakFlag = true;
							break;
						}

						if (Evaluation > BestEval)
						{
							if (depth == m_depth)
							{
								m_BestBoardPos = count;
								m_BestMove = move;
								m_BestPromotion = 65;
								if (IsWhite)
									m_BestPromotion = i + 18;
								else
									m_BestPromotion = i + 10;
							}
							BestEval = Evaluation;
						}

						BoardSquare = cPreviousBoardSquare;
					}
				}

				if (depth != 0)
				{
					BoardSquare = cBoardSquare;
					previousBoardSquare = cPreviousBoardSquare;
					CanCastle = cCanCastle;
					MoveNum = cMoveNum;
				}

				if (IsWhite)
					piece.Promotion[move - count - 7] = 65;
				else
					piece.Promotion[move - count + 9] = 65;

				continue;
			}

			if (depth == 0)
			{
				Evaluation = evaluator.Evaluate(count, move);//to change with a quiescent fun

				if (Evaluation > BestEval)
				{
					BestEval = Evaluation;
				}

				continue;
				
			}
			else
			{

				MakeMove(LegalMoves, count, move, BoardSquare, previousBoardSquare, CanCastle, 65);

				Evaluation = std::max(Evaluation, -NegaMax(BoardSquare, previousBoardSquare, CanCastle, MoveNum + 1, depth - 1, -beta, -alpha));
				alpha = std::max(alpha, Evaluation);

				if (alpha >= beta)
				{
					std::cout << "Pruning" << std::endl;
					Evaluation = true;
					BreakFlag = true;
					break;
				}

				if (Evaluation > BestEval)
				{
					if (depth == m_depth)
					{
						m_BestBoardPos = count;
						m_BestMove = move;
						m_BestPromotion = 65;
					}
					BestEval = Evaluation;
				}


				BoardSquare = cBoardSquare;
				previousBoardSquare = cPreviousBoardSquare;
				CanCastle = cCanCastle;
				MoveNum = cMoveNum;
			}
		}
		count++;
	}

	//if (WorstEvaluation == INT32_MAX)
		//std::cout << "Checkmate found while searching" << std::endl;

	return BestEval;
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
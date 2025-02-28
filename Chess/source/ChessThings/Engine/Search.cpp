#include "Search.h"

Search::Search(std::array<uint8_t, 64> BoardSquare, std::array<uint8_t, 64> PreviousBoardSquare, canCastle CanCastle, uint8_t depth, uint16_t MoveNum)
	:m_BoardSquare(BoardSquare), m_PreviousBoardSquare(PreviousBoardSquare), m_CanCastle(CanCastle), m_depth(depth), m_MoveNum(MoveNum)
{

}

Search::~Search()
{
}

EvalMove Search::GetBestMove()
{
	return LoopThroughTheTree(m_BoardSquare, m_PreviousBoardSquare, m_CanCastle, m_MoveNum, m_depth);
}

EvalMove Search::LoopThroughTheTree(std::array<uint8_t, 64Ui64> BoardSquare, std::array<uint8_t, 64> previousBoardSquare, canCastle CanCastle, uint8_t MoveNum, uint8_t depth)
{
	int Evaluation = 0;
	int WorstEvaluation = -INT32_MAX;
	EvalMove WorstEvalMove;
	EvalMove tempEvalMove;

	GenerateLegalMoves LegalMoves(BoardSquare, &previousBoardSquare, CanCastle, (MoveNum % 2 != 0) ? false : true, MoveNum, false);
	Evaluator evaluator(LegalMoves, BoardSquare, previousBoardSquare, CanCastle);

	uint8_t count = 0;

	auto tempBoardSquare = BoardSquare;
	auto tempPreviousBoardSquare = previousBoardSquare;
	auto tempCanCastle = CanCastle;
	auto tempMoveNum = MoveNum;


	for (MOVE& piece : LegalMoves.moves)
	{

		for (const uint8_t& move : piece.TargetSquares)
		{
			if (WorstEvalMove.Eval == INT32_MAX)
			{
				WorstEvalMove.BoardSquarePos = count;
				WorstEvalMove.MovePos = move;
				WorstEvalMove.Eval = INT32_MAX - 1;
			}


			//perft the promotions, this if is basically a blunt .find()
			if (piece.Promotion[0] != 65 and piece.Promotion[0] == move or piece.Promotion[1] != 65 and piece.Promotion[1] == move or piece.Promotion[2] != 65 and piece.Promotion[2] == move)
			{
				bool IsWhite = Board::IsPieceColorWhite(BoardSquare[count]);

				for (uint8_t i = 0; i != 4; ++i)
				{
					if (depth == 1)
					{
						Evaluation = evaluator.Evaluate(count, move);

						if (Evaluation < WorstEvaluation)
						{
							WorstEvaluation = Evaluation;
							WorstEvalMove.BoardSquarePos = count;
							WorstEvalMove.MovePos = move;
							WorstEvalMove.Eval = WorstEvaluation;
							WorstEvalMove.PieceToPromoteTo = IsWhite ? i + 18 : i + 10;
						}
					}
					else
					{

						if (IsWhite)
							MakeMove(LegalMoves, count, move, tempBoardSquare, tempPreviousBoardSquare, tempCanCastle, i + 18);
						else
							MakeMove(LegalMoves, count, move, tempBoardSquare, tempPreviousBoardSquare, tempCanCastle, i + 10);

						tempEvalMove = LoopThroughTheTree(tempBoardSquare, tempPreviousBoardSquare, tempCanCastle, tempMoveNum + 1, depth - 1);

						if (tempEvalMove.Eval < WorstEvalMove.Eval)
							WorstEvalMove = tempEvalMove;

						tempBoardSquare = previousBoardSquare;
					}
				}

				if (depth != 1)
				{
					tempBoardSquare = BoardSquare;
					tempPreviousBoardSquare = previousBoardSquare;
					tempCanCastle = CanCastle;
					tempMoveNum = MoveNum;
				}

				if (IsWhite)
					piece.Promotion[move - count - 7] = 65;
				else
					piece.Promotion[move - count + 9] = 65;

				continue;
			}

			if (depth == 1)
			{
				Evaluation = evaluator.Evaluate(count, move);

				if (Evaluation < WorstEvaluation)
				{
					WorstEvaluation = Evaluation;
					WorstEvalMove.BoardSquarePos = count;
					WorstEvalMove.MovePos = move;
					WorstEvalMove.Eval = WorstEvaluation;
					WorstEvalMove.PieceToPromoteTo = 65;
				}

				continue;
				
			}
			else
			{

				MakeMove(LegalMoves, count, move, tempBoardSquare, tempPreviousBoardSquare, tempCanCastle, 65);

				tempEvalMove = LoopThroughTheTree(tempBoardSquare, tempPreviousBoardSquare, tempCanCastle, tempMoveNum + 1, depth - 1);

				if (tempEvalMove.Eval < WorstEvalMove.Eval)
					WorstEvalMove = tempEvalMove;

				tempBoardSquare = BoardSquare;
				tempPreviousBoardSquare = previousBoardSquare;
				tempCanCastle = CanCastle;
				tempMoveNum = MoveNum;
			}
		}
		count++;
	}

	if (WorstEvalMove.Eval == INT32_MAX)
		std::cout << "Checkmate" << std::endl;

	return WorstEvalMove;
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
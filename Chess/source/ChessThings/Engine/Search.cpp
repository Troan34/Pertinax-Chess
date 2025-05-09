#include "Search.h"

Search::Search(std::array<uint8_t, 64> BoardSquare, std::array<uint8_t, 64> PreviousBoardSquare, canCastle CanCastle, uint8_t depth, uint16_t MoveNum)
	:m_BoardSquare(BoardSquare), m_PreviousBoardSquare(PreviousBoardSquare), m_CanCastle(CanCastle), m_depth(depth), m_MoveNum(MoveNum)
{
	GenerateLegalMoves LegalMoves(m_BoardSquare, &m_PreviousBoardSquare, m_CanCastle, (MoveNum % 2 != 0) ? false : true, MoveNum, false);
	m_Hash = std::make_unique<ZobristHashing>(&LegalMoves, &BoardSquare, &PreviousBoardSquare, CanCastle, MoveNum);
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
	std::cout << "Found move in " << duration.count() << " ms" << '\n' << std::endl;
	return ppair;
}

int Search::NegaMax(std::array<uint8_t, 64Ui64> BoardSquare, std::array<uint8_t, 64> previousBoardSquare, canCastle CanCastle, uint8_t MoveNum, uint8_t depth, int32_t alpha, int32_t beta)
{
	int Evaluation;

	GenerateLegalMoves LegalMoves(BoardSquare, &previousBoardSquare, CanCastle, (MoveNum % 2 != 0) ? false : true, MoveNum, false);
	Evaluator evaluator(LegalMoves);

	if (depth == 0)
	{
		evaluator.SetParameters(BoardSquare, previousBoardSquare, CanCastle, MoveNum);
		Evaluation = std::max(Evaluation, evaluator.Evaluate());//to change with a quiescent fun
		alpha = std::max(alpha, Evaluation);
		return Evaluation;
	}

	auto const cBoardSquare = BoardSquare;
	auto const cPreviousBoardSquare = previousBoardSquare;
	auto const cCanCastle = CanCastle;
	auto const cMoveNum = MoveNum;
	auto const cHash = m_Hash->m_Hash;


	std::vector<GuessStruct> GuessedOrder = OrderMoves(LegalMoves, BoardSquare);

	for (const GuessStruct& Guess : GuessedOrder)
	{

		bool IsWhite = Board::IsPieceColorWhite(BoardSquare[Guess.BoardSquare]);

		MakeMove(LegalMoves, m_Hash, Guess.BoardSquare, Guess.Move, BoardSquare, previousBoardSquare, CanCastle, Guess.PromotionType);

		Evaluation = std::max(Evaluation, -NegaMax(BoardSquare, previousBoardSquare, CanCastle, MoveNum + 1, depth - 1, -beta, -alpha));
		alpha = std::max(alpha, Evaluation);

		if (alpha >= beta)
		{ 
			break;//prune
		}


		if (depth == m_depth and Evaluation > m_BestEvaluation)
		{
			m_BestBoardPos = Guess.BoardSquare;
			m_BestMove = Guess.Move;
			m_BestPromotion = Guess.PromotionType;
			m_BestEvaluation = Evaluation;
		}

		BoardSquare = cBoardSquare;
		previousBoardSquare = cPreviousBoardSquare;
		CanCastle = cCanCastle;
		MoveNum = cMoveNum;
		m_Hash->m_Hash = cHash;

	}
	return alpha;
}

void Search::MakeMove(const GenerateLegalMoves& LegalMoves, std::unique_ptr<ZobristHashing>& Hash, const uint8_t& BoardSquare, const uint8_t& move, std::array<uint8_t, 64>& fun_BoardSquare, std::array<uint8_t, 64>& fun_previousBoardSquare, canCastle& Castle, const uint8_t& PieceTypeToPromoteTo)
{
	Hash->UpdateHash(BoardSquare, move, fun_BoardSquare[BoardSquare], PieceTypeToPromoteTo);

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

//sorted best to worst
std::vector<GuessStruct> Search::OrderMoves(const GenerateLegalMoves& LegalMoves, const std::array<uint8_t, 64>& fun_BoardSquare)
{
	std::vector<GuessStruct> f_OrderedMoves;
	uint8_t count = 0;

	for (MOVE piece : LegalMoves.moves)
	{
		for (const uint8_t& move : piece.TargetSquares)
		{
			for (uint8_t i = 0; i != 3; ++i)
			{
				int16_t GuessedEval = 0;
				bool IsWhite = Board::IsPieceColorWhite(fun_BoardSquare[count]);
				if (fun_BoardSquare[move] != NONE)
				{
					GuessedEval += (10 * Evaluator::ConvertPieceTypeToMatValue(fun_BoardSquare[move])) - Evaluator::ConvertPieceTypeToMatValue(fun_BoardSquare[count]);
				}

				if (LegalMoves.AttackedSquares[move])
				{
					GuessedEval -= Evaluator::ConvertPieceTypeToMatValue(fun_BoardSquare[count]);
				}

				if ((piece.Promotion[0] != 65 and piece.Promotion[0] == move) or (piece.Promotion[1] != 65 and piece.Promotion[1] == move) or (piece.Promotion[2] != 65 and piece.Promotion[2] == move))
				{
					if (IsWhite)
					{
						piece.Promotion[move - count - 7] = 65;
						GuessedEval += Evaluator::ConvertPieceTypeToMatValue(i + 18);
					}
					else
					{
						piece.Promotion[move - count + 9] = 65;
						GuessedEval += Evaluator::ConvertPieceTypeToMatValue(i + 10);
					}
					f_OrderedMoves.emplace_back(count, move, (IsWhite ? i + 18 : i + 10), GuessedEval);
					continue;
				}

                f_OrderedMoves.emplace_back(count, move, 65, GuessedEval);
			}
		}
		count++;
	}

	std::sort(f_OrderedMoves.begin(), f_OrderedMoves.end(),
		[](const auto& a, const auto& b) {
			return a.GuessedEval > b.GuessedEval;
		});

	return f_OrderedMoves;
}

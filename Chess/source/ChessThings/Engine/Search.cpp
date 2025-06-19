#include "Search.h"
static TranspositionTable TT;

Search::Search(const std::array<uint8_t, 64>& BoardSquare, const std::array<uint8_t, 64>& PreviousBoardSquare, const canCastle& CanCastle, const uint8_t& depth, const uint16_t& MoveNum, std::vector<Move>& SearchMoves, const size_t& HashSize)
	:m_BoardSquare(BoardSquare), m_PreviousBoardSquare(PreviousBoardSquare), m_CanCastle(CanCastle), m_depth(depth), m_MoveNum(MoveNum), m_SearchMoves(SearchMoves), HashSize(HashSize)
{
	TT = TranspositionTable(HashSize);
}

Search::~Search()
{
}

Move Search::GetBestMove()
{
	//auto start = std::chrono::high_resolution_clock::now();
	GenerateLegalMoves LegalMoves(m_BoardSquare, &m_PreviousBoardSquare, m_CanCastle, (m_MoveNum % 2 != 0) ? false : true, m_MoveNum, false);
	ZobristHashing m_Hash(LegalMoves, m_BoardSquare, m_PreviousBoardSquare, m_CanCastle, m_MoveNum);

	NegaMax(m_Hash, m_BoardSquare, m_PreviousBoardSquare, m_CanCastle, m_MoveNum, m_depth, -INT32_MAX, INT32_MAX);

	Move BestMove(m_BestBoardPos, m_BestMove, m_BestPromotion);

	//auto stop = std::chrono::high_resolution_clock::now();
	//auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

	//std::cout << "Found move in " << duration.count() << " ms" << '\n' << std::endl;

	return BestMove;
}

int Search::NegaMax(ZobristHashing& m_Hash, std::array<uint8_t, 64Ui64> BoardSquare, std::array<uint8_t, 64> previousBoardSquare, canCastle CanCastle, uint8_t MoveNum, uint8_t depth, int32_t alpha, int32_t beta)
{
	//Probe TT
	int32_t Eval_Temp;
	TT.TTprobe(alpha, beta, Eval_Temp, m_Hash.Hash, depth);
	if (alpha >= beta)
	{
		return alpha;//prune
	}
	if (Eval_Temp != NOT_FOUND_EXACT_BOUND_FLAG)
	{
		return Eval_Temp;
	}

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
	auto const cHash = m_Hash.Hash;


	std::vector<GuessStruct> GuessedOrder = OrderMoves(LegalMoves, BoardSquare);

	for (const GuessStruct& Guess : GuessedOrder)
	{

		bool IsWhite = Board::IsPieceColorWhite(BoardSquare[Guess.BoardSquare]);

		Move Move_(Guess.BoardSquare, Guess.Move, Guess.PromotionType);
		MakeMove(m_Hash, LegalMoves, m_Hash,Move_, BoardSquare, previousBoardSquare, CanCastle);

		Evaluation = std::max(Evaluation, -NegaMax(m_Hash, BoardSquare, previousBoardSquare, CanCastle, MoveNum + 1, depth - 1, -beta, -alpha));
		alpha = std::max(alpha, Evaluation);

		//Make a TT entry
		TT.AddEntry(Move(Guess.BoardSquare, Guess.Move, Guess.PromotionType),
			Evaluation, depth,
			(Evaluation <= alpha) ? UPPER_BOUND : ((Evaluation >= beta) ? LOWER_BOUND : EXACT),
			m_Hash.Hash);

		if (alpha >= beta)
		{ 
			break;//prune
		}

		//undo move
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
		m_Hash.Hash = cHash;


	}

	return alpha;
}

void Search::MakeMove(ZobristHashing& m_Hash, const GenerateLegalMoves& LegalMoves, ZobristHashing& Hash, Move Move_, std::array<uint8_t, 64>& fun_BoardSquare, std::array<uint8_t, 64>& fun_previousBoardSquare, canCastle& Castle)
{
	Hash.UpdateHash(Move_, fun_BoardSquare[Move_.s_BoardSquare]);

	fun_previousBoardSquare = fun_BoardSquare;
	Board::WillCanCastleChange(fun_BoardSquare[Move_.s_BoardSquare], Move_.s_BoardSquare, Move_.s_Move, Castle);
	fun_BoardSquare[Move_.s_Move] = fun_BoardSquare[Move_.s_BoardSquare];

	//castling
	if (fun_BoardSquare[Move_.s_BoardSquare] == WHITE_KING or fun_BoardSquare[Move_.s_BoardSquare] == BLACK_KING)
	{
		if (Move_.s_BoardSquare - Move_.s_Move == -2)
		{
			if (Move_.s_BoardSquare == 4)
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
		if (Move_.s_BoardSquare - Move_.s_Move == 2)
		{
			if (Move_.s_BoardSquare == 4)
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

	uint8_t PieceTypeToPromoteTo = 65;
	if (Move_.s_PromotionType < 9)//put this so ALG2MOVE doesn't break anything
	{
		if (Board::IsPieceColorWhite(fun_BoardSquare[Move_.s_BoardSquare]) and Move_.s_PromotionType != 65)
			PieceTypeToPromoteTo = Move_.s_PromotionType + WHITE;
		else
			PieceTypeToPromoteTo = Move_.s_PromotionType + BLACK;
	}

	//promoting and en passant
	if (PieceTypeToPromoteTo != 65)
	{
		fun_BoardSquare[Move_.s_Move] = PieceTypeToPromoteTo;
	}
	else//optimization
	{
		//White en passant
		if (fun_BoardSquare[Move_.s_BoardSquare] == WHITE_PAWN)
		{
			if (fun_previousBoardSquare[Move_.s_Move] == 0)
			{
				if (Move_.s_BoardSquare - Move_.s_Move == -7 or Move_.s_BoardSquare - Move_.s_Move == -9)
				{
					fun_BoardSquare[Move_.s_Move - 8] = 0;
				}
			}
		}
		//Black en passant
		if (fun_BoardSquare[Move_.s_BoardSquare] == BLACK_PAWN)
		{
			if (fun_previousBoardSquare[Move_.s_Move] == 0)
			{
				if (Move_.s_BoardSquare - Move_.s_Move == 7 or Move_.s_BoardSquare - Move_.s_Move == 9)
				{
					fun_BoardSquare[Move_.s_Move + 8] = 0;
				}
			}
		}
	}

	fun_BoardSquare[Move_.s_BoardSquare] = 0;

	GenerateLegalMoves::SetDoNotEnPassant(false);
	if (abs(Move_.s_BoardSquare - Move_.s_Move) == 16 and LegalMoves.WhichBoardSquaresAreAbsPinned[Move_.s_Move] != 65)
	{
		GenerateLegalMoves::SetDoNotEnPassant(true);
	}

}

//sorted best to worst
std::vector<GuessStruct> Search::OrderMoves(const GenerateLegalMoves& LegalMoves, const std::array<uint8_t, 64>& fun_BoardSquare)
{
	std::vector<GuessStruct> f_OrderedMoves;
	uint8_t count = 0;
	bool flag = false;

	for (MOVE piece : LegalMoves.moves)
	{

		if (!m_SearchMoves.empty())
		{
			for (Move move_ : m_SearchMoves)
			{
				if (move_.s_BoardSquare == count)
				{
					count++;
					flag = true;
					break;
				}
			}
		}
		else {
			flag = true;
		}

		if (flag)
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
		}
		count++;
	}

	std::sort(f_OrderedMoves.begin(), f_OrderedMoves.end(),
		[](const auto& a, const auto& b) {
			return a.GuessedEval > b.GuessedEval;
		});

	return f_OrderedMoves;
}


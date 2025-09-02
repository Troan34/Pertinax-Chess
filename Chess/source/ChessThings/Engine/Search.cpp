#include "Search.h"
static TranspositionTable TT;

Search::Search(const std::array<uint8_t, 64>& BoardSquare, const std::array<uint8_t, 64>& PreviousBoardSquare, const canCastle& CanCastle, const uint8_t& depth, const uint16_t& MoveNum, std::vector<Move>& SearchMoves, const size_t& HashSize)
	:m_BoardSquare(BoardSquare), m_PreviousBoardSquare(PreviousBoardSquare), m_CanCastle(CanCastle), m_depth(depth), m_MoveNum(MoveNum), m_SearchMoves(SearchMoves), HashSize(HashSize)
{
	TT.ChangeHashSize(HashSize);
}

Search::~Search()
{
}

int32_t Search::GetBestMoveWithEval(std::vector<Move>& PV)
{
	//auto start = std::chrono::high_resolution_clock::now();

	GenerateLegalMoves LegalMoves(m_BoardSquare, &m_PreviousBoardSquare, m_CanCastle, (m_MoveNum % 2 != 0) ? false : true, m_MoveNum, false);
	ZobristHashing m_Hash(LegalMoves, m_BoardSquare, m_PreviousBoardSquare, m_CanCastle, m_MoveNum);

	int32_t Eval = (NegaMax(m_Hash, m_BoardSquare, m_PreviousBoardSquare, m_CanCastle, m_MoveNum, m_depth, -INT32_MAX, INT32_MAX, PV)).Eval;
	std::println("TT Hits / Cutoffs= {} / {}", TThits, Cutoffs);
	//auto stop = std::chrono::high_resolution_clock::now();
	//auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	TT.AgeIncrementOnNewSearch();

	return Eval;
}

uint64_t Search::GetNodesVisited() const
{
	return NodesVisited;
}

uint16_t Search::GetTTFullness() const
{
	return TT.GetTTFullness();
}


void Search::ClearTT()
{
	TT.ClearTT();
}

SearchResult Search::NegaMax(ZobristHashing& m_Hash, std::array<uint8_t, 64Ui64> BoardSquare, std::array<uint8_t, 64> previousBoardSquare, canCastle CanCastle, uint8_t MoveNum, uint8_t depth, int32_t alpha, int32_t beta, std::vector<Move>& PreviousPV)
{
	NodesVisited++;
	Move BestMove;
	Move CutOffMove;
	int BestEvaluation = -INT32_MAX;
	const std::vector<Move> ConstPV = PreviousPV;//if we get mated we come back to these moves
	std::vector<Move> LocalPV;
	std::vector<Move> EmptyPV;

	GenerateLegalMoves LegalMoves(BoardSquare, &previousBoardSquare, CanCastle, (MoveNum % 2 != 0) ? false : true, MoveNum, false);

	if (depth == 0)
	{
		Evaluator evaluator(LegalMoves);
		evaluator.SetParameters(BoardSquare, previousBoardSquare, CanCastle, MoveNum);
		BestEvaluation = max(BestEvaluation, evaluator.Evaluate());//to change with a quiescent fun
		alpha = max(alpha, BestEvaluation);
		return BestEvaluation;
	}

	auto const cBoardSquare = BoardSquare;
	auto const cPreviousBoardSquare = previousBoardSquare;
	auto const cCanCastle = CanCastle;
	auto const cMoveNum = MoveNum;
	auto const cHash = m_Hash.Hash;

	//Probe TT and manage its bound
	auto FoundTT = TT.TTprobe(m_Hash.Hash);
	bool InsertTTMove = false;
	if (FoundTT.first)
	{
		TThits++;
		if (FoundTT.second.Depth >= depth)
		{

			if (LegalMoves.IsMoveLegal(FoundTT.second.BestMove))
			{
				auto Bound = TranspositionTable::GetBound(FoundTT.second.AgeBound);
				if (Bound == EXACT)
				{
					Cutoffs++;
					return { FoundTT.second.Evaluation, {FoundTT.second.BestMove} };
				}
				else if (Bound == LOWER_BOUND)
				{
					if (FoundTT.second.Evaluation >= beta)
					{
						Cutoffs++;
						return FoundTT.second.Evaluation;
					}
					alpha = max(alpha, FoundTT.second.Evaluation);
				}
				else if (Bound == UPPER_BOUND)
				{
					if (FoundTT.second.Evaluation <= alpha)
					{
						Cutoffs++;
						return FoundTT.second.Evaluation;
					}
					beta = min(beta, FoundTT.second.Evaluation);
				}
				InsertTTMove = true;
			}
		}
	}

	if (!PreviousPV.empty())
	{
		Move PVMove = PreviousPV.front();
		MakeMove(LegalMoves, m_Hash, PVMove, BoardSquare, previousBoardSquare, CanCastle);

		PreviousPV = GetVecTail(PreviousPV);

		auto Result = NegaMax(m_Hash, BoardSquare, previousBoardSquare, CanCastle, MoveNum + 1, depth - 1, -beta, -alpha, PreviousPV);
		auto Evaluation = -Result.Eval;

		if (Evaluation > BestEvaluation)
		{
			BestEvaluation = Evaluation;
			BestMove = PVMove;

			if (BestEvaluation > alpha)
			{
				alpha = Evaluation;
				LocalPV = { PVMove };
				PushBackVec(LocalPV, Result.PV);
			}
		}

		BoardSquare = cBoardSquare;
		previousBoardSquare = cPreviousBoardSquare;
		CanCastle = cCanCastle;
		MoveNum = cMoveNum;
		m_Hash.Hash = cHash;

		if (alpha >= beta)
		{
			CutOffMove = PVMove;
		}
	}

	std::vector<GuessStruct> GuessedOrder = OrderMoves(LegalMoves, BoardSquare);

	if (InsertTTMove)
	{
		//big line as compiler won't let me define a function
		GuessedOrder.insert(GuessedOrder.begin(), GuessStruct(FoundTT.second.BestMove.s_BoardSquare, FoundTT.second.BestMove.s_Move, FoundTT.second.BestMove.s_PromotionType, FoundTT.second.Evaluation));
	}

	for (const GuessStruct& Guess : GuessedOrder)
	{
		if (!PreviousPV.empty() and Move(Guess.BoardSquare, Guess.Move, Guess.PromotionType) == PreviousPV[0])
			continue;

		Move Move_(Guess.BoardSquare, Guess.Move, Guess.PromotionType);
		MakeMove(LegalMoves, m_Hash, Move_, BoardSquare, previousBoardSquare, CanCastle);


		auto Result = NegaMax(m_Hash, BoardSquare, previousBoardSquare, CanCastle, MoveNum + 1, depth - 1, -beta, -alpha, EmptyPV);
		auto Evaluation = -Result.Eval;

		if (Evaluation > BestEvaluation)
		{
			BestEvaluation = Evaluation;
			BestMove = Move_;

			if (BestEvaluation > alpha)
			{
				alpha = Evaluation;
				LocalPV = { Move_ };
				PushBackVec(LocalPV, Result.PV);
			}
		}

		if (alpha >= beta)
		{ 
			CutOffMove = Move_;
			break;//prune
		}


		//undo move
		BoardSquare = cBoardSquare;
		previousBoardSquare = cPreviousBoardSquare;
		CanCastle = cCanCastle;
		MoveNum = cMoveNum;
		m_Hash.Hash = cHash;
	}

	//Make a TT entry
	Move StoreMove{};

	if (BestEvaluation >= beta) {
		StoreMove = CutOffMove;
	}
	else {
		StoreMove = BestMove;
	}

	if (StoreMove.s_BoardSquare != NULL_OPTION)
	{
		TT.AddEntry(StoreMove, BestEvaluation, depth, m_Hash.Hash, alpha, beta);
	}


	if(depth == m_depth)
		PreviousPV = LocalPV;
	if (BestEvaluation == -INT32_MAX)
	{
		PreviousPV = ConstPV;
	}
	return { BestEvaluation, LocalPV };
}

void Search::MakeMove(const GenerateLegalMoves& LegalMoves, ZobristHashing& Hash, Move Move_, std::array<uint8_t, 64>& fun_BoardSquare, std::array<uint8_t, 64>& fun_previousBoardSquare, canCastle& Castle)
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
	f_OrderedMoves.reserve(LegalMoves.m_NumOfLegalMoves);
	uint8_t count = 0;
	bool flag = true;

	for (MOVE_BIT piece : LegalMoves.moves)
	{
		/*
		if (!m_SearchMoves.empty())
		{
			for (Move move_ : m_SearchMoves)
			{
				if (move_.s_BoardSquare == count)
				{
					count++;
					flag = false;
					break;
				}
			}
		}
		*/
		if (flag)
		{
			for(uint8_t move = 0; move <= MAX_SQUARE; move++)
			{
				if (piece.TargetSquares[move] == false) { continue; }
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
					else
					{
						f_OrderedMoves.emplace_back(count, move, 65, GuessedEval);
						break;
					}
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


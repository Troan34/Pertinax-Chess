#include "Search.h"
static TranspositionTable TT;
static bool FollowPV = true;

Search::Search(const std::array<uint8_t, 64>& BoardSquare, const std::array<uint8_t, 64>& PreviousBoardSquare, const canCastle& CanCastle, const uint8_t& depth, const uint16_t& MoveNum, std::vector<Move>& SearchMoves, const size_t& HashSize)
	:m_BoardSquare(BoardSquare), m_PreviousBoardSquare(PreviousBoardSquare), m_CanCastle(CanCastle), m_depth(depth), m_MoveNum(MoveNum), m_SearchMoves(SearchMoves), HashSize(HashSize)
{
	TT.ChangeHashSize(HashSize);
}

Search::~Search()
{
}

int32_t Search::GetBestMoveWithEval(pv_line& PV)
{
	//auto start = std::chrono::high_resolution_clock::now();

	GenerateLegalMoves LegalMoves(m_BoardSquare, &m_PreviousBoardSquare, m_CanCastle, (m_MoveNum % 2 != 0) ? false : true, m_MoveNum, false);
	ZobristHashing m_Hash(LegalMoves, m_BoardSquare, m_PreviousBoardSquare, m_CanCastle, m_MoveNum);

	m_PreviousPV = &PV;

	FollowPV = true;
	int32_t Eval = NegaMax(m_Hash, m_BoardSquare, m_PreviousBoardSquare, m_CanCastle, m_MoveNum, m_depth, -INT32_MAX, INT32_MAX, m_PreviousPV);
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

int32_t Search::NegaMax(ZobristHashing& m_Hash, std::array<uint8_t, 64Ui64> BoardSquare, std::array<uint8_t, 64> previousBoardSquare, canCastle CanCastle, uint8_t MoveNum, uint8_t depth, int32_t alpha, int32_t beta, pv_line* previousPVLine)
{
	TT.ResizeTT();
	TT.AgeIncrementOnNewSearch();
	NodesVisited++;
	Move BestMove;
	Move CutOffMove;
	int BestEvaluation = -INT32_MAX;
	pv_line PVLine;

	GenerateLegalMoves LegalMoves(BoardSquare, &previousBoardSquare, CanCastle, (MoveNum % 2 != 0) ? false : true, MoveNum, false);
	if (LegalMoves.m_NumOfLegalMoves == 0) [[unlikely]]
	{
		return -INT32_MAX;
	}

	if (depth == 0)
	{
		Evaluator evaluator(LegalMoves);
		evaluator.SetParameters(BoardSquare, previousBoardSquare, CanCastle, MoveNum);
		BestEvaluation = max(BestEvaluation, evaluator.Evaluate());//to change with a quiescent fun
		alpha = max(alpha, BestEvaluation);
		previousPVLine->NumOfMoves = 0;
		return BestEvaluation;
	}

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
				switch (Bound)
				{
					case (EXACT):
					{
						return {FoundTT.second.Evaluation};
						break;
					}
					case (LOWER_BOUND):
					{
						alpha = max(alpha, FoundTT.second.Evaluation);
						break;
					}
					case (UPPER_BOUND):
					{
						beta = min(beta, FoundTT.second.Evaluation);
						break;
					}
				}
				InsertTTMove = true;
			}
		}
	}

	//make a variable size array, for SEGFAULT or STACK OVERFLOW check here first
	GuessStruct* GuessedOrder = static_cast<GuessStruct*>(alloca((LegalMoves.m_NumOfLegalMoves + (InsertTTMove ? 1 : 0)) * sizeof(GuessStruct)));
	OrderMoves(LegalMoves, BoardSquare, GuessedOrder, InsertTTMove ? FoundTT.second : TTEntry(), depth);

	for (uint8_t MoveIndex = 0; MoveIndex < (LegalMoves.m_NumOfLegalMoves + ((InsertTTMove) ? 1 : 0)); MoveIndex++)
	{
		GuessStruct Guess = GuessedOrder[MoveIndex];

		Move Move_(Guess.BoardSquare, Guess.Move, Guess.PromotionType);
		if (Move_.IsNull()) { continue; }//this usually happens because of the first element being
		MakeMove(LegalMoves, m_Hash, Move_, BoardSquare, previousBoardSquare, CanCastle);

		auto Evaluation = NegaMax(m_Hash, BoardSquare, previousBoardSquare, CanCastle, MoveNum + 1, depth - 1, -beta, -alpha, &PVLine);

		if (Evaluation > BestEvaluation)
		{
			BestEvaluation = Evaluation;
			BestMove = Move_;

			if (BestEvaluation > alpha)//pv changes
			{
				alpha = Evaluation;
				previousPVLine->moves[0] = Move_;
				std::copy(PVLine.moves.begin(), PVLine.moves.begin() + PVLine.NumOfMoves, previousPVLine->moves.begin() + 1);
				previousPVLine->NumOfMoves = PVLine.NumOfMoves + 1;
			}
		}
		if (Evaluation > beta)
		{
			if(Move_ == m_PreviousPV->moves[m_PreviousPV->NumOfMoves - depth + 1]){ FollowPV = false; }
			Cutoffs++;
			m_Hash.Hash = cHash;//make sure we give the tt the right hash
			goto after_search;
		}


		//undo move
		BoardSquare = previousBoardSquare;
		previousBoardSquare = cPreviousBoardSquare;
		CanCastle = cCanCastle;
		MoveNum = cMoveNum;
		m_Hash.Hash = cHash;
	}

	after_search:
	//Make a TT entry
	TT.AddEntry(BestMove, BestEvaluation, depth, m_Hash.Hash, alpha, beta);

	return BestEvaluation;
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


	//promoting and en passant
	if (Move_.s_PromotionType != NULL_OPTION)
	{
		fun_BoardSquare[Move_.s_Move] = Move_.s_PromotionType;
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
void Search::OrderMoves(const GenerateLegalMoves& LegalMoves, const std::array<uint8_t, 64>& fun_BoardSquare, GuessStruct* GuessedOrder, TTEntry TTMove, uint8_t depth)
{
	const int32_t MAX_INDEX = LegalMoves.m_NumOfLegalMoves - ((TTMove.IsNull()) ? 1 : 0);//IF IGNORED -> SEGFAULT;
	uint32_t Index = 0;
	uint8_t count = 0;
	//bool flag = true; use when implementing go search

	if (!TTMove.IsNull())//there is a ttmove
	{
		GuessedOrder[0] = GuessStruct(TTMove.BestMove.s_BoardSquare, TTMove.BestMove.s_Move, TTMove.BestMove.s_PromotionType, TTMove.Evaluation);
		Index++;
	}

	for (MOVE_BIT piece : LegalMoves.moves)
	{
		if (Index > MAX_INDEX) { break; }
		uint8_t move = 0;
		uint64_t Moves_Copy = piece.TargetSquares;
		while(true)
		{
			if (!bit::pop_lsb(Moves_Copy, move) or Index > MAX_INDEX) { break; }
			int32_t GuessedEval = 0;
			bool IsWhite = Board::IsPieceColorWhite(fun_BoardSquare[count]);
			if (fun_BoardSquare[move] != NONE)
			{
				GuessedEval += (2 * Evaluator::ConvertPieceTypeToMatValue(fun_BoardSquare[move])) - 0.2*Evaluator::ConvertPieceTypeToMatValue(fun_BoardSquare[count]);
			}

			if (LegalMoves.OppositeAttackedSquares[move])
			{
				GuessedEval -= (2/3)*Evaluator::ConvertPieceTypeToMatValue(fun_BoardSquare[count]);
			}

			//Is Pv move
			if (FollowPV and !m_PreviousPV->moves[m_PreviousPV->NumOfMoves - depth + 1].IsNull() and m_PreviousPV->moves[m_PreviousPV->NumOfMoves - depth + 1].s_BoardSquare == count and m_PreviousPV->moves[m_PreviousPV->NumOfMoves - depth + 1].s_Move == move and LegalMoves.IsMoveLegal(m_PreviousPV->moves[m_PreviousPV->NumOfMoves - depth + 1]))
			{
				GuessedEval = 5000000;//IS pv move, make the guessed eval big, just to simulate a good move
			}

			if (piece.Promotion.Promotion != 0 and (piece.Promotion.Promotion & PromotionMask) != 0)
			{
				if (IsWhite)
				{
					piece.Promotion.ResetPromotionSide(move - count - 7, count);
				}
				else
				{
					piece.Promotion.ResetPromotionSide(move - count + 9, count);
				}
				for (uint8_t i = 0; i < 4; ++i)
				{
					if (IsWhite)
					{
						GuessedEval += Evaluator::ConvertPieceTypeToMatValue(i + 18);
					}
					else
					{
						GuessedEval += Evaluator::ConvertPieceTypeToMatValue(i + 10);
					}
					GuessedOrder[Index] = GuessStruct(count, move, (IsWhite ? i + 18 : i + 10), GuessedEval);//ADDED here
					Index++;
				}
			}
			else
			{
				GuessedOrder[Index] = GuessStruct(count, move, 65, GuessedEval);//AND here
				Index++;
			}

			
		}
		count++;
	}

	std::sort(GuessedOrder + !TTMove.IsNull(), GuessedOrder + LegalMoves.m_NumOfLegalMoves,
		[](const auto& a, const auto& b) {
			return a.GuessedEval > b.GuessedEval;
		});

}


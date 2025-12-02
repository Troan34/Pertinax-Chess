#include "Search.h"
static TranspositionTable TT;
static bool FollowPV = true;

Search::Search(const Position& ChessPosition, const uint8_t depth, std::vector<Move>& SearchMoves, const size_t& HashSize)
	:m_ChessPosition(ChessPosition), m_depth(depth), m_SearchMoves(SearchMoves), HashSize(HashSize)
{
	TT.ChangeHashSize(HashSize);
}


int32_t Search::GetBestMoveWithEval(pv_line& PV)
{
	//auto start = std::chrono::high_resolution_clock::now();

	GenerateLegalMoves LegalMoves(m_ChessPosition, false);
	ZobristHashing m_Hash(LegalMoves, m_ChessPosition);

	m_PreviousPV = &PV;

	FollowPV = true;
	int32_t Eval = NegaMax(m_Hash, m_ChessPosition, m_depth, -INT32_MAX, INT32_MAX, m_PreviousPV);
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

int32_t Search::NegaMax(ZobristHashing& m_Hash, Position ChessPosition, uint8_t depth, int32_t alpha, int32_t beta, pv_line* previousPVLine)
{
	TT.ResizeTT();
	TT.AgeIncrementOnNewSearch();
	int BestEvaluation = -INT32_MAX;
	NodesVisited++;

	GenerateLegalMoves LegalMoves(ChessPosition, false);
	if (LegalMoves.isItCheckmate == true)
	{
		return -INT32_MAX;
	}

	if (depth == 0)
	{
		Evaluator evaluator(LegalMoves);
		evaluator.SetParameters(ChessPosition);
		BestEvaluation = evaluator.Evaluate();//to change with a quiescent fun
		//BestEvaluation = -QuietSearch(ChessPosition, -beta, -alpha);
		previousPVLine->NumOfMoves = 0;
		return BestEvaluation;
	}

	Move BestMove;
	Move CutOffMove;
	pv_line PVLine;

	auto const cChessPosition = ChessPosition;
	auto const cHash = m_Hash.Hash;

	//Probe TT and manage its bound
	auto FoundTT = TT.TTprobe(m_Hash.Hash);

	if (FoundTT.first)
	{
		TThits++;
		if (FoundTT.second.Depth >= depth and LegalMoves.IsMoveLegal(FoundTT.second.BestMove))
		{
			if (depth == m_depth)//this is to protect against an empty pv line
			{
				m_PreviousPV->moves[0] = FoundTT.second.BestMove;
			}
			return FoundTT.second.Evaluation;
		}
	}

	//make a variable length array, for SEGFAULT or STACK OVERFLOW check here first
	GuessStruct* GuessedOrder = static_cast<GuessStruct*>(alloca(LegalMoves.m_NumOfLegalMoves * sizeof(GuessStruct)));
	OrderMoves(LegalMoves, ChessPosition.BoardSquare, GuessedOrder, depth);

	int32_t SmallWindowBeta = beta;
	int32_t Evaluation;
	for (uint8_t MoveIndex = 0; MoveIndex < LegalMoves.m_NumOfLegalMoves; MoveIndex++)
	{
		GuessStruct Guess = GuessedOrder[MoveIndex];

		Move Move_(Guess.BoardSquare, Guess.Move, Guess.PromotionType);
		if (Move_.IsNull()) { continue; }//this usually happens because of the first element being null
		MakeMove(LegalMoves, m_Hash, Move_, ChessPosition);

		ChessPosition.MoveNum++;
		Evaluation = -NegaMax(m_Hash, ChessPosition, depth - 1, -SmallWindowBeta, -alpha, &PVLine);

		if ((Evaluation > alpha) and (Evaluation < beta) and (MoveIndex > 0))
		{
			Evaluation = -NegaMax(m_Hash, ChessPosition, depth - 1, -beta, -alpha, &PVLine);
		}

		//undo move
		ChessPosition = cChessPosition;
		m_Hash.Hash = cHash;

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
			if (Evaluation > beta)
			{
				Cutoffs++;
				m_Hash.Hash = cHash;//make sure we give the tt the right hash
				goto after_search;
			}
		}
		
		SmallWindowBeta = alpha + 1;//widen window

		
	}

	after_search:
	//Make a TT entry
	if (!BestMove.IsNull())//this happens on mates, where the "if (Evaluation > BestEvaluation)" block hasn't been entered
	{
		TT.AddEntry(BestMove, BestEvaluation, depth, m_Hash.Hash, alpha, beta);
	}

	return BestEvaluation;
}

void Search::MakeMove(const GenerateLegalMoves& LegalMoves, ZobristHashing& Hash, const Move Move_, Position& ChessPosition)
{
	Hash.UpdateHash(Move_, ChessPosition.BoardSquare[Move_.s_BoardSquare]);

	Board::WillCanCastleChange(Move_.s_BoardSquare, ChessPosition.CanCastle);
	ChessPosition.BoardSquare[Move_.s_Move] = ChessPosition.BoardSquare[Move_.s_BoardSquare];

	//castling
	if (ChessPosition.BoardSquare[Move_.s_BoardSquare] == WHITE_KING or ChessPosition.BoardSquare[Move_.s_BoardSquare] == BLACK_KING)
	{
		if (Move_.s_BoardSquare - Move_.s_Move == -2)
		{
			if (Move_.s_BoardSquare == 4)
			{
				ChessPosition.BoardSquare[5] = WHITE_ROOK;
				ChessPosition.BoardSquare[7] = 0;
			}
			else
			{
				ChessPosition.BoardSquare[61] = BLACK_ROOK;
				ChessPosition.BoardSquare[63] = 0;
			}
		}
		if (Move_.s_BoardSquare - Move_.s_Move == 2)
		{
			if (Move_.s_BoardSquare == 4)
			{
				ChessPosition.BoardSquare[3] = WHITE_ROOK;
				ChessPosition.BoardSquare[0] = 0;
			}
			else
			{
				ChessPosition.BoardSquare[59] = BLACK_ROOK;
				ChessPosition.BoardSquare[56] = 0;
			}
		}

	}

	switch (Move_.s_BoardSquare)
	{
	case a1:
		ChessPosition.CanCastle.WhiteLong = false;
		break;
	case h1:
		ChessPosition.CanCastle.WhiteShort = false;
		break;
	case e1:
		ChessPosition.CanCastle.WhiteLong = false;
		ChessPosition.CanCastle.WhiteShort = false;
		break;
	case a8:
		ChessPosition.CanCastle.BlackLong = false;
		break;
	case h8:
		ChessPosition.CanCastle.BlackShort = false;
		break;
	case e8:
		ChessPosition.CanCastle.BlackLong = false;
		ChessPosition.CanCastle.BlackShort = false;
		break;
	}
	switch (Move_.s_Move)
	{
	case a1:
		ChessPosition.CanCastle.WhiteLong = false;
		break;
	case h1:
		ChessPosition.CanCastle.WhiteShort = false;
		break;
	case e1:
		ChessPosition.CanCastle.WhiteLong = false;
		ChessPosition.CanCastle.WhiteShort = false;
		break;
	case a8:
		ChessPosition.CanCastle.BlackLong = false;
		break;
	case h8:
		ChessPosition.CanCastle.BlackShort = false;
		break;
	case e8:
		ChessPosition.CanCastle.BlackLong = false;
		ChessPosition.CanCastle.BlackShort = false;
		break;
	}

	//promoting and en passant
	if (Move_.s_PromotionType != NULL_OPTION)
	{
		ChessPosition.BoardSquare[Move_.s_Move] = Move_.s_PromotionType;
	}
	else//optimization
	{
		if ((Move_.s_Move % 8) == ChessPosition.EnPassant.EPIndex() and ChessPosition.BoardSquare[Move_.s_Move] == 0)
		{
			if (ChessPosition.BoardSquare[Move_.s_BoardSquare] == WHITE_PAWN)
			{
				ChessPosition.BoardSquare[Move_.s_Move + S] = 0;
			}
			else
			{
				ChessPosition.BoardSquare[Move_.s_Move + N] = 0;
			}
		}
	}

	ChessPosition.EnPassant.Reset();
	if (abs(Move_.s_BoardSquare - Move_.s_Move) == 16 and Board::GetPieceType2Uncolored(ChessPosition.BoardSquare[Move_.s_BoardSquare]) == PAWN)//double push
	{
		ChessPosition.EnPassant.SetEP(Move_.s_BoardSquare % 8);
	}

	ChessPosition.BoardSquare[Move_.s_BoardSquare] = 0;

	GenerateLegalMoves::SetDoNotEnPassant(false);
	if (abs(Move_.s_BoardSquare - Move_.s_Move) == 16 and LegalMoves.WhichBoardSquaresAreAbsPinned[Move_.s_Move] != 65)
	{
		GenerateLegalMoves::SetDoNotEnPassant(true);
	}

}

void Search::QMakeMove(const GenerateLegalMoves& LegalMoves, const Move Move_, Position& ChessPosition)
{

	Board::WillCanCastleChange(Move_.s_BoardSquare,ChessPosition.CanCastle);
	ChessPosition.BoardSquare[Move_.s_Move] = ChessPosition.BoardSquare[Move_.s_BoardSquare];

	//castling
	if (ChessPosition.BoardSquare[Move_.s_BoardSquare] == WHITE_KING or ChessPosition.BoardSquare[Move_.s_BoardSquare] == BLACK_KING)
	{
		if (Move_.s_BoardSquare - Move_.s_Move == -2)
		{
			if (Move_.s_BoardSquare == 4)
			{
				ChessPosition.BoardSquare[5] = WHITE_ROOK;
				ChessPosition.BoardSquare[7] = 0;
			}
			else
			{
				ChessPosition.BoardSquare[61] = BLACK_ROOK;
				ChessPosition.BoardSquare[63] = 0;
			}
		}
		if (Move_.s_BoardSquare - Move_.s_Move == 2)
		{
			if (Move_.s_BoardSquare == 4)
			{
				ChessPosition.BoardSquare[3] = WHITE_ROOK;
				ChessPosition.BoardSquare[0] = 0;
			}
			else
			{
				ChessPosition.BoardSquare[59] = BLACK_ROOK;
				ChessPosition.BoardSquare[56] = 0;
			}
		}

	}


	//promoting and en passant
	if (Move_.s_PromotionType != NULL_OPTION)
	{
		ChessPosition.BoardSquare[Move_.s_Move] = Move_.s_PromotionType;
	}
	else//optimization
	{
		if ((Move_.s_Move % 8) == ChessPosition.EnPassant.EPIndex() and ChessPosition.BoardSquare[Move_.s_Move] == 0)
		{
			if (ChessPosition.BoardSquare[Move_.s_BoardSquare] == WHITE_PAWN)
			{
				ChessPosition.BoardSquare[Move_.s_Move + S] = 0;
			}
			else
			{
				ChessPosition.BoardSquare[Move_.s_Move + N] = 0;
			}
		}
	}

	ChessPosition.EnPassant.Reset();
	if (abs(Move_.s_BoardSquare - Move_.s_Move) == 16 and Board::GetPieceType2Uncolored(ChessPosition.BoardSquare[Move_.s_BoardSquare]))//double push
	{
		ChessPosition.EnPassant.SetEP(Move_.s_BoardSquare % 8);
	}

	ChessPosition.BoardSquare[Move_.s_BoardSquare] = 0;

	GenerateLegalMoves::SetDoNotEnPassant(false);
	if (abs(Move_.s_BoardSquare - Move_.s_Move) == 16 and LegalMoves.WhichBoardSquaresAreAbsPinned[Move_.s_Move] != 65)
	{
		GenerateLegalMoves::SetDoNotEnPassant(true);
	}

}

//sorted best to worst
void Search::OrderMoves(const GenerateLegalMoves& LegalMoves, const std::array<uint8_t, 64>& fun_BoardSquare, GuessStruct* GuessedOrder, uint8_t depth)
{
	const int32_t MAX_INDEX = LegalMoves.m_NumOfLegalMoves;//IF IGNORED -> SEGFAULT;
	uint32_t Index = 0;
	uint8_t count = 0;


	for (MOVE_BIT piece : LegalMoves.moves)
	{
		if (Index > MAX_INDEX) { break; }
		uint8_t move = 0;
		uint64_t Moves_Copy = piece.TargetSquares;
		while(true)
		{
			if (Moves_Copy == 0 or !bit::pop_lsb(Moves_Copy, move) or Index > MAX_INDEX)[[likely]] { break; }
			int32_t GuessedEval = 0;
			bool IsWhite = Board::IsPieceColorWhite(fun_BoardSquare[count]);
			if (fun_BoardSquare[move] != NONE)
			{
				GuessedEval += (2 * Evaluator::ConvertPieceTypeToMatValue(fun_BoardSquare[move])) - (0.2f)*Evaluator::ConvertPieceTypeToMatValue(fun_BoardSquare[count]);
			}

			if (LegalMoves.OppositeAttackedSquares[move])
			{
				GuessedEval -= (0.6f)*Evaluator::ConvertPieceTypeToMatValue(fun_BoardSquare[count]);
			}

			if (depth <= m_PreviousPV->NumOfMoves)
			{
				//Is Pv move
				if (!m_PreviousPV->moves[m_PreviousPV->NumOfMoves - depth + 1].IsNull() and m_PreviousPV->moves[m_PreviousPV->NumOfMoves - depth + 1].s_BoardSquare == count and m_PreviousPV->moves[m_PreviousPV->NumOfMoves - depth + 1].s_Move == move and LegalMoves.IsMoveLegal(m_PreviousPV->moves[m_PreviousPV->NumOfMoves - depth + 1]))
				{
					GuessedEval = 5000000;//IS pv move, make the guessed eval big, just to simulate a good move
				}
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
						GuessedOrder[Index] = GuessStruct(count, move, i + 18, GuessedEval);
						GuessedEval -= Evaluator::ConvertPieceTypeToMatValue(i + 18);
					}
					else
					{
						GuessedEval += Evaluator::ConvertPieceTypeToMatValue(i + 10);
						GuessedOrder[Index] = GuessStruct(count, move, i + 10, GuessedEval);
						GuessedEval -= Evaluator::ConvertPieceTypeToMatValue(i + 10);
					}
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

	std::sort(GuessedOrder, GuessedOrder + LegalMoves.m_NumOfLegalMoves,
		[](const auto& a, const auto& b) {
			return a.GuessedEval > b.GuessedEval;
		});

}

void Search::QOrderMoves(const GenerateLegalMoves& LegalMoves, const std::array<uint8_t, 64>& fun_BoardSquare, GuessStruct* GuessedOrder, uint32_t NumOfTacticalMoves)
{
	const int32_t MAX_INDEX = NumOfTacticalMoves;//IF IGNORED -> SEGFAULT;
	uint32_t Index = 0;
	uint8_t count = 0;


	for (MOVE_BIT piece : LegalMoves.moves)
	{
		if (Index > MAX_INDEX) { break; }
		uint8_t move = 0;
		uint64_t Moves_Copy = piece.TargetSquares;
		while (true)
		{
			if (Moves_Copy == 0 or !bit::pop_lsb(Moves_Copy, move) or Index > MAX_INDEX) [[likely]] { break; }
			//if not a tactical move
			if (!((piece.TargetSquares[move] and fun_BoardSquare[move] != 0) or (piece.Promotion.Promotion & PromotionMask))) { continue; }

			int32_t GuessedEval = 0;
			bool IsWhite = Board::IsPieceColorWhite(fun_BoardSquare[count]);
			if (fun_BoardSquare[move] != NONE)
			{
				GuessedEval += (2 * Evaluator::ConvertPieceTypeToMatValue(fun_BoardSquare[move])) - (0.2f) * Evaluator::ConvertPieceTypeToMatValue(fun_BoardSquare[count]);
			}

			if (LegalMoves.OppositeAttackedSquares[move])
			{
				GuessedEval -= (0.6f) * Evaluator::ConvertPieceTypeToMatValue(fun_BoardSquare[count]);
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
						GuessedOrder[Index] = GuessStruct(count, move, i + 18, GuessedEval);
						GuessedEval -= Evaluator::ConvertPieceTypeToMatValue(i + 18);
					}
					else
					{
						GuessedEval += Evaluator::ConvertPieceTypeToMatValue(i + 10);
						GuessedOrder[Index] = GuessStruct(count, move, i + 10, GuessedEval);
						GuessedEval -= Evaluator::ConvertPieceTypeToMatValue(i + 10);
					}
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

	std::sort(GuessedOrder, GuessedOrder + NumOfTacticalMoves,
		[](const auto& a, const auto& b) {
			return a.GuessedEval > b.GuessedEval;
		});

}

int32_t Search::QuietSearch(Position ChessPosition, int32_t alpha, int32_t beta)
{
	NodesVisited++;
	GenerateLegalMoves LegalMoves(ChessPosition, false);
	uint32_t NumOfTacticalMoves = LegalMoves.GetNumOfTacticalMoves();
	Evaluator evaluator(LegalMoves);
	evaluator.SetParameters(ChessPosition);
	int32_t Eval = evaluator.Evaluate();//to change with a quiescent fun

	if (NumOfTacticalMoves == 0)
	{
		return Eval;
	}

	int32_t BestEval = Eval;
	if (BestEval >= beta)
	{
		return BestEval;
	}
	if (BestEval > alpha)
	{
		alpha = BestEval;
	}

	auto const cChessPosition = ChessPosition;

	
	GuessStruct* GuessedOrder = static_cast<GuessStruct*>(alloca(NumOfTacticalMoves * sizeof(GuessStruct)));
	QOrderMoves(LegalMoves, ChessPosition.BoardSquare, GuessedOrder, NumOfTacticalMoves);

	for (uint8_t MoveIndex = 0; MoveIndex < NumOfTacticalMoves; MoveIndex++)
	{
		GuessStruct Guess = GuessedOrder[MoveIndex];

		Move Move_(Guess.BoardSquare, Guess.Move, Guess.PromotionType);
		if (Move_.IsNull()) { continue; }//this usually happens because of the first element being null
		QMakeMove(LegalMoves, Move_, ChessPosition);

		ChessPosition.MoveNum--;
		Eval = -QuietSearch(ChessPosition, -beta, -alpha);

		if (Eval >= beta)
			return Eval;
		if (Eval > BestEval)
			BestEval = Eval;
		if (Eval > alpha)
			alpha = Eval;

		//undo move
		ChessPosition = cChessPosition;
	}

	return BestEval;
}

//Swap algorithm SEE
int32_t Search::SEE(const Position& ChessPosition, Move Move)
{
	GenerateLegalMoves PseudoLegalMoves(ChessPosition, true);
	return 0;
}


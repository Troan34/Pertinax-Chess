#include "ChessThings/Engine/Transposition/TT.h"



TranspositionTable::TranspositionTable(size_t TTSize)
	:HashSize(TTSize)
{
	TT.reserve(floor(HashSize / SIZE_OF_HASHMAP_ELEMENT));
}

TranspositionTable::TranspositionTable()
{
}


void TranspositionTable::TTprobe(int32_t& alpha, int32_t& beta, int32_t& eval, const uint64_t& Hash, const uint8_t& depth)
{
	auto TTentry = TT.find(Hash);
	if (TTentry != TT.end())
	{
		switch (GetBound(TTentry->second.AgeBound))
		{
		case(EXACT):
			eval = TTentry->second.Evaluation;
			break;
		case(LOWER_BOUND):
			alpha = std::max(alpha, TTentry->second.Evaluation);
			break;
		case(UPPER_BOUND):
			beta = std::min(beta, TTentry->second.Evaluation);
			break;
		default:
			ASSERT(false)
		}
	}
	else {
		eval = NOT_FOUND_EXACT_BOUND_FLAG;
		return;
	}
}

void TranspositionTable::AddEntry(Move BestMove, int32_t Eval, uint8_t Depth, uint64_t Hash, const int32_t& alpha, const int32_t& beta)
{
	TTEntry Entry;
	if (Eval >= beta)
	{
		Entry.AgeBound = LOWER_BOUND;
	}
	else if (Eval <= alpha)
	{
		Entry.AgeBound = UPPER_BOUND;
	}
	else
	{
		Entry.AgeBound = EXACT;
	}

	Entry.BestMove = BestMove;
	Entry.Depth = Depth;
	Entry.Evaluation = Eval;

	if (HashSize > (TT.size() * SIZE_OF_HASHMAP_ELEMENT))
	{
		TT[Hash] = Entry;
	}
	else
	{
		ResizeTT();
		TT[Hash] = Entry;
	}
}

void inline TranspositionTable::AgeIncrementOnNewSearch() { CurrentAge += AGE_DELTA; }

size_t TranspositionTable::GetTTSize() const
{
	return (TT.size() * SIZE_OF_HASHMAP_ELEMENT);
}

void TranspositionTable::ResizeTT()//Just age(no other strategies)
{
	uint16_t NumOfDeletedEntries = 0;
	uint8_t MinimumAgeDelta = 100; //SIMULATE (or whatever else) TO GET A BETTER NUMBER
	uint8_t MinAgeDeltaIncrement = 9; //THIS TOO

	while (NumOfDeletedEntries < NUM_OF_ENTRIES_TO_BE_DELETED)
	{
		for (auto& it : TT)
		{
			if (RelativeAge(it.second.AgeBound) > MinimumAgeDelta and (GetBound(it.second.AgeBound) != EXACT));//MIGHT BE A BUG, WITH A TT FULL OF PV
			{
				TT.erase(it.first);//This might be a bug, SAME bug as that one in LegalMoves(when i deleted the element i was iterating)
				NumOfDeletedEntries++;
				if (NumOfDeletedEntries < NUM_OF_ENTRIES_TO_BE_DELETED)
					continue;
				else
					break;
			}
		}
		MinimumAgeDelta -= MinAgeDeltaIncrement;
	}
	
}

uint8_t inline TranspositionTable::GetAge(const uint8_t& AgeBound) { return AgeBound & AGE_MASK; }

uint8_t inline TranspositionTable::GetBound(const uint8_t& AgeBound) { return AgeBound & BOUND_MASK; }

uint8_t TranspositionTable::RelativeAge(const uint8_t& AgeBound)
{
	return abs(CurrentAge - GetAge(AgeBound));
}

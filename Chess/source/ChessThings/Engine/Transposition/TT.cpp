#include "ChessThings/Engine/Transposition/TT.h"



TranspositionTable::TranspositionTable(size_t TTSize)
	:m_HashSize(TTSize)
{
	TT.reserve(floor(m_HashSize / SIZE_OF_HASHMAP_ELEMENT));
}

TranspositionTable::TranspositionTable()
{
	TT.reserve(floor(m_HashSize / SIZE_OF_HASHMAP_ELEMENT));
}

TranspositionTable::~TranspositionTable()
{
}


std::pair<bool, TTEntry> TranspositionTable::TTprobe(const uint64_t& Hash)
{
	auto TTentry = TT.find(Hash);
	
	if (TTentry != TT.end())
	{
		return { true, TTentry->second } ;
	}
	else {
		return { false, TTEntry{} };// look out for this, possible bug if not checked
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



	TT[Hash] = Entry;


	//std::println("Storing TT entry: \nHash={}\nDepth={}\nEval={}\nBound={}\nMove={}", Hash, Depth + '0', Eval, GetBound(Entry.AgeBound) + '0', Board::Move2ALG(BestMove));
}

void TranspositionTable::AgeIncrementOnNewSearch() { CurrentAge += AGE_DELTA; }

size_t TranspositionTable::GetTTSize() const
{
	return (TT.size() * SIZE_OF_HASHMAP_ELEMENT);
}

uint16_t TranspositionTable::GetTTFullness() const
{
	return 1000*(TT.size()*SIZE_OF_HASHMAP_ELEMENT)/(m_HashSize);
}

float inline TranspositionTable::GetTTSizeInMB() const
{
	return (TT.size() * static_cast<float>(SIZE_OF_HASHMAP_ELEMENT)) / 1000000;
}

void TranspositionTable::ClearTT()
{
	TT.clear();
}

void TranspositionTable::ChangeHashSize(const size_t& HashSize)
{
	if (HashSize < m_HashSize)
	{
		while ((GetTTSizeInMB() * 1000000) < HashSize)
		{
			ResizeTT();
		}
	}
	m_HashSize = HashSize;
	TT.reserve(floor(m_HashSize / SIZE_OF_HASHMAP_ELEMENT));
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
			if (RelativeAge(it.second.AgeBound) > MinimumAgeDelta and (GetBound(it.second.AgeBound) != EXACT))//MIGHT BE A BUG, WITH A TT FULL OF PV
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

#include "ChessThings/Engine/Transposition/TT.h"


TranspositionTable::TranspositionTable(size_t TTSize)
	:m_HashSize(TTSize)
{
	TT.reserve(ceil(m_HashSize / SIZE_OF_HASHMAP_ELEMENT));
}

TranspositionTable::TranspositionTable()
{
	TT.reserve(ceil(m_HashSize / SIZE_OF_HASHMAP_ELEMENT));
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
	uint8_t AgeBound;
	if (Eval >= beta)
	{
		AgeBound = LOWER_BOUND;
	}
	else if (Eval <= alpha)
	{
		AgeBound = UPPER_BOUND;
	}
	else
	{
		AgeBound = EXACT;
	}


	TT.try_emplace(Hash, BestMove, AgeBound, Eval, Depth);


	//std::println("Storing TT entry: \nHash={}\nDepth={}\nEval={}\nBound={}\nMove={}", Hash, Depth + '0', Eval, GetBound(Entry.AgeBound) + '0', Board::Move2ALG(BestMove));
}

void TranspositionTable::AgeIncrementOnNewDepth() { CurrentAge += AGE_DELTA; }

void TranspositionTable::AgeDecrementOnNewDepth() { CurrentAge -= AGE_DELTA; }

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
		while (TT.size() > NUM_OF_ENTRIES_TO_BE_DELETED)
		{
			ResizeTT();
		}
	}
	m_HashSize = HashSize;
	TT.reserve(ceil(m_HashSize / SIZE_OF_HASHMAP_ELEMENT));
}

void TranspositionTable::ResizeTT()//Just age(no other strategies)
{
	if (GetTTFullness() < 980) { return; }
	uint16_t NumOfDeletedEntries = 0;
	uint8_t MinimumAgeDelta = 25; //SIMULATE (or whatever else) TO GET A BETTER NUMBER - has to be LESS than MAX_NORMALIZED_AGE

	while (NumOfDeletedEntries < NUM_OF_ENTRIES_TO_BE_DELETED)
	{
		for (auto it = TT.begin(); it != TT.end(); it++)
		{
			if (RelativeAge(it->second.AgeBound) >= MinimumAgeDelta and (GetBound(it->second.AgeBound) != EXACT))//MIGHT BE A BUG, WITH A TT FULL OF PV
			{
				it = TT.erase(it);//This might be a bug, SAME bug as that one in LegalMoves(when i deleted the element i was iterating)
				NumOfDeletedEntries++;
				if (NumOfDeletedEntries > NUM_OF_ENTRIES_TO_BE_DELETED)
					break;
			}
		}
		MinimumAgeDelta -= 3;
	}
}

uint8_t inline TranspositionTable::GetAge(const uint8_t AgeBound) { return AgeBound & AGE_MASK; }

uint8_t inline TranspositionTable::GetBound(const uint8_t AgeBound) { return AgeBound & BOUND_MASK; }

uint8_t TranspositionTable::RelativeAge(const uint8_t AgeBound)
{
	return abs(CurrentAge - GetAge(AgeBound));
}

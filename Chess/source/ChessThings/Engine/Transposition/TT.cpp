#include "ChessThings/Engine/Transposition/TT.h"

void TranspositionTable::TTprobe(int32_t& alpha, int32_t& beta, int32_t& eval, const uint64_t& Hash, const uint8_t& depth)
{
	auto TTentry = TT.find(Hash);
	if (TTentry != TT.end())
	{
		if (TTentry->second.Hash == Hash and TTentry->second.Depth >= depth)
		{
			if (TTentry->second.BoundType == EXACT)
			{
				eval = TTentry->second.Evaluation;
			}
			else if (TTentry->second.BoundType == LOWER_BOUND)
			{
				alpha = std::max(alpha, TTentry->second.Evaluation);
			}
			else if (TTentry->second.BoundType == UPPER_BOUND)
			{
				beta = std::min(beta, TTentry->second.Evaluation);
			}
		}

	}
}

void TranspositionTable::AddEntry(const TTEntry& Entry, uint64_t Hash)
{

}

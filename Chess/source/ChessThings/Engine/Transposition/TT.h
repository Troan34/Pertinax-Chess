#pragma once
#include "ChessThings/Board.h"

constexpr int32_t NOT_FOUND_EXACT_BOUND_FLAG = -1329548202;//random number, hopefully this doesn't get hit

struct TTEntry
{
	uint64_t Hash;
	Move BestMove;
	uint8_t BoundType;
	int32_t Evaluation;
	uint8_t Depth;
};

class TranspositionTable
{
private:
	std::unordered_map<uint64_t, TTEntry> TT;

public:
	
	void TTprobe(int32_t& alpha, int32_t& beta, int32_t& eval, const uint64_t& Hash, const uint8_t& depth);
	void AddEntry(const TTEntry& Entry, uint64_t Hash);
};


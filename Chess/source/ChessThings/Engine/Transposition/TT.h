#pragma once
#include "ChessThings/Board.h"

static constexpr int16_t NOT_FOUND_EXACT_BOUND_FLAG = 0xFFF2;//random number, hopefully this doesn't get hit
static constexpr size_t SIZE_OF_TTENTRY = sizeof(TTEntry);
static constexpr size_t SIZE_OF_HASHMAP_ELEMENT = sizeof(TTEntry) + sizeof(uint64_t); //bucket + key

//Could explain this but stockfish explains it better, and i just barely understand it.
//Basically we compress some variables into AgeBound (Age, pv, bound) and we have to do some extra steps to store information,
	//this means the AgeBound looks like this 0bAAAAApbb

static constexpr uint8_t MISC_BITS = 3;
static constexpr uint8_t AGE_DELTA = 0b00001000;//this is basically adding 1 to the AGE bits
static constexpr uint8_t AGE_CYCLE = 0xFF + AGE_DELTA;//this just stops us from accidentally touching the misc bits
static constexpr uint8_t AGE_MASK = 0b11111000; //when we & with these bits, we are sure to get all the age bits

struct TTEntry
{
	uint64_t Hash;
	Move BestMove;
	int16_t Evaluation;
	uint8_t Depth;
	uint8_t AgeBound;
};

class TranspositionTable
{
private:
	std::unordered_map<uint64_t, TTEntry> TT;
	size_t HashSize;

	void ResizeTT();
public:
	TranspositionTable(size_t TTSize);
	void TTprobe(int32_t& alpha, int32_t& beta, int32_t& eval, const uint64_t& Hash, const uint8_t& depth);
	void AddEntry(const TTEntry& Entry, uint64_t Hash);
};


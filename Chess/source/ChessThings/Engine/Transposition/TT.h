#pragma once
#include "ChessThings/Board.h"

static constexpr int32_t NOT_FOUND_EXACT_BOUND_FLAG = 1524334251;//random number, hopefully this doesn't get hit
static constexpr size_t SIZE_OF_TTENTRY = 12;
static constexpr size_t SIZE_OF_HASHMAP_ELEMENT = 12 + sizeof(uint64_t); //bucket + key
static constexpr uint16_t NUM_OF_ENTRIES_TO_BE_DELETED = 1024;//felt like a good number, TO BE OPTIMIZED

//Could explain this but stockfish explains it better, and i just barely understand it.
//Basically we compress some variables into AgeBound (Age, pv, bound) and we have to do some extra steps to store information,
	//this means the AgeBound looks like this 0bAAAAANbb
static constexpr uint8_t MISC_BITS = 3;
static constexpr uint8_t AGE_DELTA = 0b00001000;//this is basically adding 1 to the AGE bits
static constexpr uint16_t AGE_CYCLE = 0xFF + AGE_DELTA;//this just stops us from accidentally touching the misc bits
static constexpr uint8_t AGE_MASK = 0b11111000; //when we & with these bits, we are sure to get all the age bits
static constexpr uint8_t BOUND_MASK = 0b00000011;

struct TTEntry
{
	Move BestMove;
	uint8_t AgeBound;
	int32_t Evaluation;
	uint8_t Depth;
};

class TranspositionTable
{
private:
	std::unordered_map<uint64_t, TTEntry> TT;
	size_t HashSize = 64000000;
	uint8_t CurrentAge = 0;

	void ResizeTT();
	uint8_t GetAge(const uint8_t& AgeBound);
	uint8_t GetBound(const uint8_t& AgeBound);
	uint8_t RelativeAge(const uint8_t& AgeBound);
public:
	TranspositionTable(size_t TTSize);
	TranspositionTable();
	void TTprobe(int32_t& alpha, int32_t& beta, int32_t& eval, const uint64_t& Hash, const uint8_t& depth);
	void AddEntry(Move BestMove, int32_t Eval, uint8_t Depth, uint64_t Hash, const int32_t& alpha, const int32_t& beta);
	void AgeIncrementOnNewSearch();
	size_t GetTTSize() const;
	uint16_t GetTTFullness() const;
};


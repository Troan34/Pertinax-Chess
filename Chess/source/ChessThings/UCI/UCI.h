#pragma once
#include"ChessThings/Board.h"
#include <iostream>
#include <atomic>
#include <thread>

//options
static constexpr std::string_view DEPTH_COMMAND = "depth";
static constexpr std::string_view ENGINE_ON_COMMAND = "EngineOn";

//position commands
static constexpr std::string_view POSITION_COMMAND = "position";
static constexpr std::string_view STARTPOS_COMMAND = "startpos";
static constexpr std::string_view FEN_COMMAND = "fen";
static constexpr std::string_view UCINEWGAME_COMMAND = "ucinewgame";

//go commands
static constexpr std::string_view GO_COMMAND = "go";
static constexpr std::string_view SEARCHMOVES_COMMAND = "searchmoves";
static constexpr std::string_view DEPTH_COMMAND = "depth";



struct UciVars_p
{
	uint8_t* depth;
	bool* EngineOn;
	std::array<uint8_t, 64Ui64>* BoardSquare;
	std::array<uint8_t, 64Ui64>* previousBoardSquare;//also used to 'simulate' an e.p. opportunity
	uint32_t* MoveNum;
	canCastle* CanCastle;
	std::vector<Move>* SearchMoves;
	Timer* timer;
};

class UCI
{
private:
	std::atomic_bool QuitRequested{ false };
	std::string Command;
	UciVars_p Vars_p;

	void RunCommand();
public:

	UCI(UciVars_p Vars);
	void UCIInfoesToSend(UCIInfoes UCIInfo, bool IncludeFrequentInfo);


};

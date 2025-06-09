#pragma once
#include"ChessThings/Board.h"
#include <iostream>
#include <atomic>
#include <thread>

//options
constexpr std::string_view DEPTH_COMMAND = "depth";
constexpr std::string_view ENGINE_ON_COMMAND = "EngineOn";

//go and other position commands
constexpr std::string_view POSITION_COMMAND = "position";
constexpr std::string_view STARTPOS_COMMAND = "startpos";
constexpr std::string_view FEN_COMMAND = "fen";



struct UciVars_p
{
	uint8_t* depth;
	bool* EngineOn;
	std::array<uint8_t, 64Ui64>* BoardSquare;
	std::array<uint8_t, 64Ui64>* previousBoardSquare;
	uint32_t* MoveNum;
	canCastle* CanCastle;
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



};

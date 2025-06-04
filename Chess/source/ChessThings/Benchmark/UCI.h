#pragma once
#include"ChessThings/Board.h"
#include <iostream>
#include <atomic>
#include <thread>

//options
constexpr std::string_view DEPTH = "depth";
constexpr std::string_view ENGINE_ON = "EngineOn";



struct UciVars_p
{
	uint8_t* depth;
	bool* EngineOn;
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

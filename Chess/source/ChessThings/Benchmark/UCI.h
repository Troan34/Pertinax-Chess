#pragma once
#include"ChessThings/Board.h"
#include <iostream>
#include <atomic>
#include <thread>

//options
constexpr std::string_view DEPTH = "depth";


class UCI
{
private:
	std::atomic_bool QuitRequested{ false };
	std::string Command;

	void RunCommand();
public:
	UCI();



};

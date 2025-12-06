#pragma once
#include"ChessThings/Board.h"
#include"ChessThings/Engine/IterativeD.h"
#include <iostream>
#include <atomic>
#include <thread>

//options
static constexpr std::string_view DEPTH_COMMAND = "Depth";
static constexpr std::string_view ENGINE_ON_COMMAND = "EngineOn";
static constexpr std::string_view HASH_COMMAND = "Hash";
static constexpr std::string_view GUI_COMMAND = "Gui";

//position commands
static constexpr std::string_view POSITION_COMMAND = "position";
static constexpr std::string_view STARTPOS_COMMAND = "startpos";
static constexpr std::string_view FEN_COMMAND = "fen";
static constexpr std::string_view UCINEWGAME_COMMAND = "ucinewgame";

//go commands
static constexpr std::string_view GO_COMMAND = "go";
static constexpr std::string_view SEARCHMOVES_COMMAND = "searchmoves";
static constexpr std::string_view WTIME = "wtime";
static constexpr std::string_view BTIME = "btime";
static constexpr std::string_view WINC = "winc";
static constexpr std::string_view BINC = "binc";
static constexpr std::string_view INFINITE_COMMAND = "infinite";
static constexpr std::string_view PERFT_COMMAND = "perft";

static constexpr std::string_view STOP_COMMAND = "stop";



struct UciVars_p
{
	uint8_t* depth;
	bool* EngineOn;
	bool* GUI;
	std::array<uint8_t, 64Ui64>* BoardSquare;
	EP* EnPassant;
	uint32_t* MoveNum;
	CastlingAbility* CanCastle;
	std::vector<Move>* SearchMoves;
	Timer* timer;
	size_t* HashSize;
	
};

class UCI
{
private:
	std::atomic_bool QuitRequested{ false };
	std::string Command;
	UciVars_p Vars_p;
	bool stop = false;
	bool exit = false;
	std::atomic<bool> IsReady = true;
	std::atomic<bool> Outputting = false;

	void RunCommand();
	void Go();

public:

	UCI(UciVars_p Vars);
	static uint32_t Perft(std::array<uint8_t, 64Ui64> BoardSquare, EP EnPassant, CastlingAbility CanCastle, bool isNextMoveForWhite, uint8_t depth, bool DivideFunON, unsigned int PerftMoveNum);
};

#include "ChessThings/Benchmark/UCI.h"

UCI::UCI(UciVars_p Vars)
	:Vars_p(Vars)
{
	while (true)
	{
		std::getline(std::cin, Command);
		RunCommand();
	}
}

void UCI::RunCommand()
{
	if (Command.find("isready") != std::string::npos)
		std::cout << "readyok\n";
	if (Command.find("setoption name") != std::string::npos)
	{
		if (Command.substr(15, 5) == DEPTH_COMMAND)
		{
			if (isdigit(Command[21]))
			{
				*Vars_p.depth = Command[21] - '0';
			}


		}

		if (Command.substr(15, 8) == ENGINE_ON_COMMAND)
			*Vars_p.EngineOn = !*Vars_p.EngineOn;



		//
		//TODO: do all the commands AND how to access those variables from this class
		//
	}

	if (Command.find(POSITION_COMMAND) != std::string::npos)
	{
		if (Command.find(STARTPOS_COMMAND, 8) != std::string::npos)//startpos
		{
			Board board{ std::string(STARTPOS) };
			*Vars_p.BoardSquare = board.GetPositionFromFEN();
			*Vars_p.MoveNum = 0;
		}
		else
		{
			if (Command.find("moves") != std::string::npos)
			{
				//Finish this
			}
			else 
			{
				auto Fen = Command.substr(8, std::string::npos);
				Board board{ Fen };
				*Vars_p.BoardSquare = board.GetPositionFromFEN();
				*Vars_p.MoveNum = board.MoveNum();
			}

		}
	}
}

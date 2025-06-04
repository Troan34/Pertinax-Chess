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
		if (Command.substr(15, 5) == DEPTH)
		{
			if (isdigit(Command[21]))
			{
				*Vars_p.depth = Command[21] - '0';
			}


		}

		if (Command.substr(15, 8) == ENGINE_ON)
			*Vars_p.EngineOn = !*Vars_p.EngineOn;

		//
		//TODO: do all the commands AND how to access those variables from this class
		//
	}
}

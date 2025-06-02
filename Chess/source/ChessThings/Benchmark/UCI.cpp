#include "ChessThings/Benchmark/UCI.h"

UCI::UCI()
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
		if (Command.substr(15, std::string::npos) == DEPTH)
		{

		}
		//
		//TODO: do all the commands AND how to access those variables from this class
		//
	}
}

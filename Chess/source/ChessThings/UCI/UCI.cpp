#include "ChessThings/UCI/UCI.h"

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

	}

	if (Command.find(UCINEWGAME_COMMAND) != std::string::npos)//this should do other things, but i will work on that later
	{
		Board board{ std::string(STARTPOS) };
		*Vars_p.BoardSquare = board.GetPositionFromFEN();
		*Vars_p.MoveNum = board.MoveNum();
	}

	if (Command.find(POSITION_COMMAND) != std::string::npos)
	{
		if (Command.find(STARTPOS_COMMAND, 8) != std::string::npos)//startpos
		{
			Board board{ std::string(STARTPOS) };
			*Vars_p.BoardSquare = board.GetPositionFromFEN();
			*Vars_p.MoveNum = board.MoveNum();
		}
		else
		{
			if (Command.find(FEN_COMMAND, 8) != std::string::npos)
			{
				auto Fen = Command.substr(8, std::string::npos);
				Board board{ Fen };
				*Vars_p.BoardSquare = board.GetPositionFromFEN();
				*Vars_p.BoardSquare = board.GetPositionFromFEN();
				*Vars_p.MoveNum = board.MoveNum();
				if (board.GetPawnMoveSquare() != 65)
				{
					*Vars_p.previousBoardSquare = Board::PrevBoardSquareFromEP(*Vars_p.BoardSquare, board.GetPawnMoveSquare());
				}

				if (Command.find("moves") != std::string::npos)
				{
					size_t Index = Command.find(' ', Command.find("moves"));

					while (true)
					{
						if (Index > SIZE_MAX - 20)//if Index overflowed, it means we are at the end of string
							break;
						Move move = Board::LongALG2Move(Command.substr(Index + 1, Command.find(' ', Index + 1)));
						Index = Command.find(' ', Index + 1);//will overflow when it reaches the end
						Board::MakeMove(move, *Vars_p.BoardSquare, *Vars_p.previousBoardSquare, *Vars_p.CanCastle);
						*Vars_p.MoveNum += 1;
					}
				}
			}

		}
	}

	if (Command.find(GO_COMMAND) != std::string::npos)
	{
		if (Command.find(SEARCHMOVES_COMMAND, 2) != std::string::npos)
		{
			size_t Index = Command.find(' ', Command.find(SEARCHMOVES_COMMAND));

			while (true)
			{
				if (Index > SIZE_MAX - 20)//if Index overflowed, it means we are at the end of string
					break;
				Move move = Board::LongALG2Move(Command.substr(Index + 1, Command.find(' ', Index + 1)));
				Index = Command.find(' ', Index + 1);//will overflow when it reaches the end
				Vars_p.SearchMoves->push_back(move);
			}
		}

		if (Command.find(DEPTH_COMMAND, 2) != std::string::npos)
		{
			*Vars_p.depth = (Command.find(' ', Command.find(SEARCHMOVES_COMMAND)) + 1) - '0';
		}
		else
		{
			*Vars_p.depth = UINT8_MAX;
			//!!!
			//! THE ENGINE DOESN'T CHOSE A MOVE (or selects pv nodes) IF DEPTH IS NOT FINITE, add ID
			//!!!
		}

	}
}

void UCI::UCIInfoesToSend(UCIInfoes UCIInfo, bool IncludeFrequentInfo)
{
	std::cout << "info " << "depth " << *UCIInfo.Depth << " nodes " << *UCIInfo.NumOfNodes << " nps " << *UCIInfo.NpS << " hashfull " << *UCIInfo.HashFull
		<< std::endl;
}

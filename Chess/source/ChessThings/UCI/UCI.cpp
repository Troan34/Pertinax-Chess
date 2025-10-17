#include "ChessThings/UCI/UCI.h"

UCI::UCI(UciVars_p Vars)
	:Vars_p(Vars)
{
	while (!exit)
	{
		std::getline(std::cin, Command);
		RunCommand();
	}
}

void UCI::RunCommand()
{

	if ((Command.find("isready") != std::string::npos))
	{
		while (Outputting.load()) { Sleep(1); }
		
		Outputting.store(true);
		std::cout << "readyok\n";
		Sleep(1);
		Outputting.store(false);
		
	}
	else if (Command.find("setoption name") != std::string::npos)
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

		if (Command.find(HASH_COMMAND, 15) != std::string::npos)
		{
			*Vars_p.HashSize = stoi(Command.substr(Command.find(HASH_COMMAND, 15) + 5)) * 1000000;
		}

		if (Command.find(GUI_COMMAND) != std::string::npos)
		{
			*Vars_p.GUI = !*Vars_p.GUI;
		}
	}
	else if (Command.find("uci") != std::string::npos) //UCI mode
	{
		if(Command.find("off") != std::string::npos)
		{
			exit = true;
			std::cout << "You left uci, type 'help'\n";
		}
		else
		{
			std::cout <<
				"id name Pertinax Chess 0.1\n" <<
				"id author R.Bukaci (github.com/Troan34)\n\n" <<
				"option name type spin Depth default " << static_cast<int>(*Vars_p.depth) << " min 2 max 255\n" <<
				"option name type button EngineOn default On\n" <<
				"option name type spin Hash default " << *Vars_p.HashSize / 1000000 << " min 1 max 512\n" <<
				"option name type button Gui default Off\n" <<
				"uciok\n" << std::endl;
		}
	}

	if (Command.find(UCINEWGAME_COMMAND) != std::string::npos)//this should do other things, but i will work on that later
	{
		Board board{ std::string(STARTPOS) };
		*Vars_p.BoardSquare = board.GetPositionFromFEN();
		*Vars_p.MoveNum = board.MoveNum();
		Search::ClearTT();
	}

	if (Command.find(POSITION_COMMAND) != std::string::npos)
	{
		if (Command.find(STARTPOS_COMMAND, 8) != std::string::npos)//startpos
		{
			Board board{ std::string(STARTPOS) };
			*Vars_p.BoardSquare = board.GetPositionFromFEN();
			*Vars_p.MoveNum = board.MoveNum();

		}
		else if (Command.find(FEN_COMMAND, 8) != std::string::npos)
		{	
			auto Fen = Command.substr(Command.find(FEN_COMMAND, 8) + 4, std::string::npos);
			Board board{ Fen };
			*Vars_p.BoardSquare = board.GetPositionFromFEN();
			*Vars_p.MoveNum = board.MoveNum();
			*Vars_p.CanCastle = board.GetCanCastle();
			if (board.GetPawnMoveSquare() != 65)
			{
				*Vars_p.previousBoardSquare = Board::PrevBoardSquareFromEP(*Vars_p.BoardSquare, board.GetPawnMoveSquare());
			}

		}

		if (Command.find("moves") != std::string::npos)
		{
			size_t Index = Command.find(' ', Command.find("moves"));

			while (true)
			{
				if (Index > SIZE_MAX - 20)//if Index overflowed, it means we are at the end of string
					break;
				Move move = Board::LongALG2Move(Command.substr(Index + 1, Command.find(' ', Index + 1) - Index - 1));
				Index = Command.find(' ', Index + 1);//will overflow when it reaches the end
				Board::MakeMove(move, *Vars_p.BoardSquare, *Vars_p.previousBoardSquare, *Vars_p.CanCastle);
				*Vars_p.MoveNum += 1;
			}
		}
	}

	if (Command.find(GO_COMMAND) != std::string::npos)
	{
		bool RunGo = true;
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
			*Vars_p.depth = (Command.find(' ', Command.find(DEPTH_COMMAND)) + 1) - '0';
		}
		else
		{
			*Vars_p.depth = UINT8_MAX;
		}

		if (Command.find(INFINITE_COMMAND, 2) != std::string::npos)
		{ 
			Vars_p.timer->BTime = static_cast<std::chrono::milliseconds>(INT32_MAX);
			Vars_p.timer->WTime = static_cast<std::chrono::milliseconds>(INT32_MAX);
		}

		if (Command.find(PERFT_COMMAND, 2) != std::string::npos)
		{
			std::string depth = Command.substr(9, std::string::npos);
			if (!IsStringANum(depth))
			{
				std::cout << "Wrong syntax: depth wasn't a digit\n" << std::endl;
			}
			uint8_t depthNum = std::stoi(depth);
			auto result = Perft(*Vars_p.BoardSquare, *Vars_p.previousBoardSquare, *Vars_p.CanCastle, WHITE_TURN(*Vars_p.MoveNum), depthNum, true, *Vars_p.MoveNum);
			std::cout << "\nNodes searched: " << result << "\n\n";
			RunGo = false;
		}
		//tc
		{
			if (Command.find(WTIME, 2) != std::string::npos)
				Vars_p.timer->WTime = static_cast<std::chrono::milliseconds>(stoi(Command.substr(Command.find(' ', Command.find(WTIME)) + 1, Command.find(' ', Command.find(' ', Command.find(WTIME)) + 1))));
			if (Command.find(BTIME, 2) != std::string::npos)
				Vars_p.timer->BTime = static_cast<std::chrono::milliseconds>(stoi(Command.substr(Command.find(' ', Command.find(BTIME)) + 1, Command.find(' ', Command.find(' ', Command.find(BTIME)) + 1))));
			if (Command.find(WINC, 2) != std::string::npos)
				Vars_p.timer->WIncrement = static_cast<std::chrono::milliseconds>(stoi(Command.substr(Command.find(' ', Command.find(WINC)) + 1, Command.find(' ', Command.find(' ', Command.find(WINC)) + 1))));
			if (Command.find(BINC, 2) != std::string::npos)
				Vars_p.timer->BIncrement = static_cast<std::chrono::milliseconds>(stoi(Command.substr(Command.find(' ', Command.find(BINC)) + 1, Command.find(' ', Command.find(' ', Command.find(BINC)) + 1))));
		}

		if (stop)
		{
			while (!(IsReady.load())) { Sleep(20); };
		}
		if (RunGo)
		{
			std::thread Thread(&UCI::Go, this);
			Thread.detach();
		}
	}

	if (Command.find(STOP_COMMAND) != std::string::npos)
	{
		stop = true;
	}


}

void UCI::Go()
{
	IsReady.store(false);

	IterativeDeepening ID(Position{ *Vars_p.BoardSquare, *Vars_p.previousBoardSquare, *Vars_p.CanCastle, *(uint16_t*)(Vars_p.MoveNum) }, *Vars_p.SearchMoves, *Vars_p.HashSize, *Vars_p.timer, *Vars_p.depth, !((*Vars_p.MoveNum) % 2), &stop);
	std::string Bestmove = Board::Move2ALG(ID.GetBestMove(true));

	while (Outputting.load()) { Sleep(1); }
	
	Outputting.store(true);
	std::cout << "bestmove " << Bestmove << std::endl;
	Sleep(1);
	Outputting.store(false);
	

	IsReady.store(true);
}

uint32_t UCI::Perft(std::array<uint8_t, 64Ui64> BoardSquare, std::array<uint8_t, 64> perftPreviousBoardSquare, canCastle CanCastle, bool isNextMoveForWhite, uint8_t depth, bool DivideFunON, unsigned int& PerftMoveNum)
{
	uint32_t NumOfMoves = 0;

	GenerateLegalMoves LegalMoves(Position{ BoardSquare, perftPreviousBoardSquare, CanCastle, static_cast<uint16_t>(PerftMoveNum) }, false);

	const auto ConstBoardSquare = BoardSquare;
	const auto ConstPreviousBoardSquare = perftPreviousBoardSquare;
	const canCastle ConstCanCastle = CanCastle;
	uint8_t count = 0;


	for (MOVE_BIT& piece : LegalMoves.moves)
	{
		uint8_t move = 0;
		auto TargetSquaresCopy = piece.TargetSquares;
		while (true)
		{
			if (!bit::pop_lsb(TargetSquaresCopy, move)) { break; }
			//perft the promotions, this if is basically a blunt .find()
			if (piece.Promotion.Promotion != 0 and (piece.Promotion.Promotion & PromotionMask) != 0)
			{
				bool IsWhite = Board::IsPieceColorWhite(BoardSquare[count]);

				for (uint8_t i = 0; i != 4; ++i)
				{
					if (depth == 1)
					{
						if (DivideFunON)
						{
							if (IsWhite)
								std::cout << +count << " " << +move << " to " << Board::PieceType2letter(i + 18) << ": 1" << '\n';
							else
								std::cout << +count << " " << +move << " to " << Board::PieceType2letter(i + 10) << ": 1" << '\n';
						}
						NumOfMoves++;
					}
					else
					{
						Move Move_(count, move);

						if (IsWhite)
							Move_.s_PromotionType = i + 18;
						else
							Move_.s_PromotionType = i + 10;

						Board::MakeMove(Move_, BoardSquare, perftPreviousBoardSquare, CanCastle);
						if (DivideFunON)
						{
							uint32_t DivideFunNum = 0;
							DivideFunNum += Perft(BoardSquare, perftPreviousBoardSquare, CanCastle, !isNextMoveForWhite, depth - 1, false, PerftMoveNum);
							NumOfMoves += DivideFunNum;
							if (IsWhite)
								std::cout << +count << " " << +move << ": " << DivideFunNum << " to " << Board::PieceType2letter(i + 18) << '\n';
							else
								std::cout << +count << " " << +move << ": " << DivideFunNum << " to " << Board::PieceType2letter(i + 10) << '\n';
						}
						else
							NumOfMoves += Perft(BoardSquare, perftPreviousBoardSquare, CanCastle, !isNextMoveForWhite, depth - 1, false, PerftMoveNum);

						BoardSquare = perftPreviousBoardSquare;
					}
				}

				if (depth != 1)
				{
					BoardSquare = ConstBoardSquare;
					perftPreviousBoardSquare = ConstPreviousBoardSquare;
					CanCastle = ConstCanCastle;
				}

				if (IsWhite)
					piece.Promotion.ResetPromotionSide(move - count - 7, count);
				else
					piece.Promotion.ResetPromotionSide(move - count + 9, count);

				continue;
			}

			if (depth == 1)
			{
				if (DivideFunON)
					std::cout << +count << " " << +move << ": 1" << '\n';
				NumOfMoves++;
			}
			else
			{
				Move Move_(count, move);

				Board::MakeMove(Move_, BoardSquare, perftPreviousBoardSquare, CanCastle);
				if (DivideFunON)
				{
					uint32_t DivideFunNum = 0;
					DivideFunNum += Perft(BoardSquare, perftPreviousBoardSquare, CanCastle, !isNextMoveForWhite, depth - 1, false, PerftMoveNum);
					NumOfMoves += DivideFunNum;
					std::cout << +count << " " << +move << ": " << DivideFunNum << '\n';
				}
				else
					NumOfMoves += Perft(BoardSquare, perftPreviousBoardSquare, CanCastle, !isNextMoveForWhite, depth - 1, false, PerftMoveNum);

				BoardSquare = ConstBoardSquare;
				perftPreviousBoardSquare = ConstPreviousBoardSquare;
				CanCastle = ConstCanCastle;
			}
		}
		count++;
	}

	return NumOfMoves;
}

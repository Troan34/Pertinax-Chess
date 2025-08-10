#include "RenderChesspcs.h"

static float RememberTexID;
static bool wasStatic_BoardSquareCreated = false;
static bool wasStatic_previousBoardsquareCreated = false;

//(mostly) Drop fun vars
static int BoardSquareBeingSelected = -1;
int AttackedSquare = -1;
std::unordered_set<uint8_t> LegalMovesForSelectedSquare;
static bool WaitingForUserPromotion = false;
static uint8_t rememberAttackedSquareForUserPromotion;
static bool WaitingForEnemyMove = false;
static MouseInput g_mouseInput;

//multithreading vars
static std::atomic<bool> IsGetCommandRunning = true;
static std::string Command;
static std::atomic<bool> ReceivedACommand(false);
static std::atomic<bool> RunCommandThread(true);

//UCI
static std::thread Thread;
static bool UCImode = false;

//Vars shared with UCI
static unsigned int MoveNum = 0;
static canCastle CanCastle;
static std::array<uint8_t, 64Ui64> static_BoardSquare;
static std::array<uint8_t, 64Ui64> previousBoardsquare;
static bool StartEngine = false;
static bool GUI = false;
static bool PreviousGuiOption = true;
	//Engine vars
	static bool EngineOn = false;
	static uint8_t EngineDepth = 10;
	static std::vector<Move> SearchMoves{};
	static std::chrono::milliseconds WTime(999999);
	static Timer timer(WTime, WTime, static_cast<std::chrono::milliseconds>(0), static_cast<std::chrono::milliseconds>(0));//max(for now)
	static size_t HashSize = 64000000;//default 64MB

static void RunUCI()//this is a workaround
{
	UciVars_p Vars_p;
	Vars_p.depth = &EngineDepth;
	Vars_p.EngineOn = &EngineOn;
	Vars_p.BoardSquare = &static_BoardSquare;
	Vars_p.MoveNum = &MoveNum;
	Vars_p.previousBoardSquare = &previousBoardsquare;
	Vars_p.CanCastle = &CanCastle;
	Vars_p.SearchMoves = &SearchMoves;
	Vars_p.timer = &timer;
	Vars_p.HashSize = &HashSize;
	Vars_p.GUI = &GUI;
	UCI uci(Vars_p);
}

RenderChessPieces::RenderChessPieces(GLFWwindow* window)
	:window(window)
{

}
RenderChessPieces::~RenderChessPieces()
{

}

static void cursorPositionCallBack(GLFWwindow* window, double xPosition, double yPosition)
{
	g_mouseInput.xPos = (float)xPosition;
	g_mouseInput.yPos = (float)yPosition;
}

static void mouseButtonCallBack(GLFWwindow* window, int button, int action, int mods)
{
	

	if (button == GLFW_MOUSE_BUTTON_LEFT and action == GLFW_PRESS)
	{
		g_mouseInput.LeftButtonPressed = true;
	}
	else if (action != GLFW_PRESS)
	{
		g_mouseInput.LeftButtonPressed = false;
	}
}

//waits for a command to be written
static void GetCommand()
{
	if (RunCommandThread)
	{
		std::getline(std::cin, Command);
		if (Command == "uci")
			UCImode = true;
		ReceivedACommand = true;
		IsGetCommandRunning = false;
	}
}

static std::thread CommandThread(GetCommand);

/*********MAIN FUNCTION*********does quad calculations, calls command thread, drag and drop...**************/
std::array<std::array<VertexStructure, 4Ui64>, 135> RenderChessPieces::CreateObjects()
{
	ComputeHeavy();
	int square;
	uint64_t ComputedSquares = 0;
	std::array<uint64_t, 64> RookMagics{};

	while (RookMagics[0] == 0)
	{
		auto Magic = MagicFinder(0, true);
		if (Magic != 0) { RookMagics[0] = Magic; }
		ComputedSquares++;
		std::cout << "\rTries: " << ComputedSquares << std::flush;
	}
	std::cout << RookMagics[0];
	/*
	for (square = 0; square < 64; square++)
	{
		if (RookMagics[square] == 0)
		{
			auto Magic = MagicFinder(square, true);
			RookMagics[square] = Magic;
			if (Magic != 0) 
			{
				ComputedSquares++;
				std::println("\rMagics Found ({}/64), Last Square found: {}, Magic: {}", ComputedSquares, square, Magic);
			}
			
		}
		if (square == 63 and (std::find(RookMagics.begin(), RookMagics.end(), 0) != RookMagics.end()))
		{
			square = 0;
		}
	}
	for (square = 0; square < 64; square++)
	{
		std::println("Val= {}, for {}", RookMagics[square], square);
	}*/
		
	/*
	printf("const uint64 BMagic[64] = {\n");
	for (square = 0; square < 64; square++)
		printf("  0x%llxULL,\n", MagicFinder(square, 1));
	printf("};\n\n");*/

	//Console Commands and threads
	if (!IsGetCommandRunning and !UCImode)
	{
		CommandThread.join();
		CommandThread = std::thread(GetCommand);
		IsGetCommandRunning = true;
	}

	//processes commands
	if (ReceivedACommand == true)  
	{
		if (Command == "help")
		{
			std::cout << "__________________________________Help menu__________________________________" << std::endl;
			std::cout << "\n'perft' {depth}            --Run perft with current position\n\n" <<
				"Engine things: " << '\n' <<
				"   'Engine off'            --The engine won't respond to your moves" << '\n' <<
				"   'Engine on'             --The engine will respond to your moves" << '\n' <<
				"   'Engine depth' {depth}  --The engine will search to the specified depth\n" <<
				"   'Engine settings'       --Shows current settings for the engine" << "\n\n" <<
				"Default Engine settings: On, depth 6" << '\n' <<
				"_____________________________________________________________________________" << "\n\n";
		}
		else if (Command.find("perft") != std::string::npos)//perft command
		{
			char depth = Command.at(6);
			if (!isdigit(depth))
			{
				std::cout << "Wrong syntax: depth wasn't a digit\n" << std::endl;
			}
			else
			{
				depth = (uint8_t)depth - '0';
				void_Futures.push_back(std::async(std::launch::async, &RenderChessPieces::CreatePerft, this, depth));
				//std::cout << "Main CPU: " << GetCurrentProcessorNumber() << std::endl;
			}
		}
		else if (Command.find("Engine") != std::string::npos)//engine commands
		{
			if (Command.find("off", Command.find("Engine") + 7) != std::string::npos)         //turn off engine
			{
				EngineOn = false;
				std::cout << "Engine was successfully turned off!\n" << std::endl;
			}
			else if (Command.find("on", Command.find("Engine") + 7) != std::string::npos)     //turn on engine
			{
				EngineOn = true;
				std::cout << "Engine was successfully turned on!\n" << std::endl;
			}
			else if (Command.find("depth", Command.find("Engine") + 7) != std::string::npos)  //set engine depth
			{
                std::string depthStr = Command.substr(Command.find("depth", Command.find("Engine") + 7) + 6);
                if (!std::all_of(depthStr.begin(), depthStr.end(), ::isdigit))
                {
                    std::cout << "Wrong syntax: depth wasn't a digit\n" << std::endl;
                }
                else
                {
                    uint8_t depth = std::stoi(depthStr);
                    std::cout << "Engine will search to depth " << static_cast<int>(depth) << "!\n" << std::endl;
					EngineDepth = depth;
                }
			}
			else if (Command.find("settings", Command.find("Engine") + 7) != std::string::npos) //show engine settings
			{
				std::cout << "Engine is ";
				if (EngineOn)
					std::cout << "on";
				else
					std::cout << "off";
				std::cout << " and will search to depth " << static_cast<int>(EngineDepth) << "!\n" << std::endl;
			}
			else
			{
				std::cout << "Wrong syntax: couldn't understand after 'Engine'\n" << std::endl;
			}
		}
		else if (Command == "uci") //UCI mode
		{
			std::cout <<
				"id name Pertinax Chess 0.1\n" <<
				"id author R.Bukaci (github.com/Troan34)\n\n" <<
				"option name type spin Depth default " << static_cast<int>(EngineDepth) << " min 2 max 255\n" <<
				"option name type button EngineOn default On\n" <<
				"option name type spin Hash default " << HashSize / 1000000 << " min 1 max 512\n" <<
				"option name type button Gui default Off\n" <<
				"uciok\n" << std::endl;

			RunCommandThread = false;
		}
		else if (Command.find("gui") != std::string::npos)
		{
			GUI = !GUI;
		}
		else
		{
			std::cout << "Wrong syntax, couldn't recognize first word\n" << std::endl;
		}

		if (!IsGetCommandRunning and !UCImode)
		{
			CommandThread.join();
			CommandThread = std::thread(GetCommand);
			IsGetCommandRunning = true;
		}

		if (UCImode)
		{
			std::thread Thread(RunUCI);
			Thread.detach();
		}
		ReceivedACommand = false;
	}

	bool isNextMoveForWhite = true;
	if (MoveNum % 2 != 0)
		isNextMoveForWhite = false;
	

	if (GUI == PreviousGuiOption)
	{
		if (GUI)
		{
			glfwShowWindow(window);
			PreviousGuiOption = !GUI;
		}
		else
		{
			glfwHideWindow(window);
			PreviousGuiOption = !GUI;
		}
	}
	
	//Legal Moves
	GenerateLegalMoves* p_LegalMoves = nullptr;
	if (BoardSquareBeingSelected != -1)
	{
		float xxDifference = 0.0f;
		float yyDifference = 0.0f;
		static_BoardSquare[BoardSquareBeingSelected] = GetPieceTypefromTexID(RememberTexID);
		std::array<uint8_t, 64Ui64>* p_prevBoardSquare;
		if (wasStatic_previousBoardsquareCreated)
			p_prevBoardSquare = &previousBoardsquare;
		else
			p_prevBoardSquare = nullptr;

		GenerateLegalMoves LegalMoves(static_BoardSquare, p_prevBoardSquare, CanCastle, isNextMoveForWhite, MoveNum, false);
		for (uint8_t j = 0; j <= MAX_SQUARE; j++)
		{
			if (LegalMoves.moves[BoardSquareBeingSelected].TargetSquares[j] == false) { break; }
			xxDifference = j * 87.5f;
			yyDifference = 0;
			while (xxDifference >= 650.0f)
			{
				xxDifference -= 700.0f;
				yyDifference += 87.5f;
			}
			quads[j + 66] = CreateQuad(-350.0f + xxDifference, -350.0f + yyDifference, 87.5f, 14.0f);
			LegalMovesForSelectedSquare.insert(j);
		}
		p_LegalMoves = &LegalMoves;
		
		static_BoardSquare[BoardSquareBeingSelected] = 0;
	}

	
	//Make engine move
	if (WaitingForEnemyMove and EngineOn and !WaitingForUserPromotion)
	{
		bool stop = false;
		IterativeDeepening ID(static_BoardSquare, previousBoardsquare, CanCastle, MoveNum, SearchMoves, HashSize, timer, EngineDepth, true, &stop);
		Move BestMove = ID.GetBestMove(false);
		GenerateLegalMoves LegalMoves(static_BoardSquare, &previousBoardsquare, CanCastle, isNextMoveForWhite, MoveNum, false);
		MakeMove(LegalMoves, BestMove, static_BoardSquare, previousBoardsquare, CanCastle);
		WaitingForEnemyMove = false;
		MoveNum++;
		isNextMoveForWhite = true;
		if (MoveNum % 2 != 0)
			ASSERT(false);
	}
	
	//rendering part
	quads[0] = CreateQuad(-350.0f, -350.0f, 700.0f, 0.0f);
	float xDifference = 0.0f;
	float yDifference = 0.0f;

 	for (int i = 1; i < 65; i++)
	{
		if (xDifference > 650.0f)
		{
			xDifference = 0.0f;
			yDifference += 87.5f;
		}

		float PieceTexID = GetPieceTextureID(static_BoardSquare, i - 1);
		bool HasDragAndDropFunHappened = false;

		//drag and drop and promotion
		if (g_mouseInput.LeftButtonPressed and g_mouseInput.WasLeftButtonPressed == false and PieceTexID != 0.0f)
		{
			if (!WaitingForUserPromotion)
			{
				//next two ifs are to check which objects have to be dragged
				if ((g_mouseInput.xPos - 350.0f) > (-350.0f + xDifference) and (g_mouseInput.xPos - 350.0f) < (-350.0f + xDifference + 87.5f))
				{
					if ((-g_mouseInput.yPos + 360.0f) > (-350.0f + yDifference) and (-g_mouseInput.yPos + 360.0f) < (-350.0f + yDifference + 87.5f))
					{

						quads[65] = CreateQuad(g_mouseInput.xPos - 393.5f, -g_mouseInput.yPos + 319.25f, 87.5f, PieceTexID);
						RememberTexID = PieceTexID;
						static_BoardSquare[i - 1] = (unsigned int)0;
						BoardSquareBeingSelected = i - 1;

					}
				}
			}
		}
		
		//this if is to "drop" the object ( and other things )
		if (g_mouseInput.LeftButtonPressed == false and g_mouseInput.WasLeftButtonPressed == true and !WaitingForUserPromotion and BoardSquareBeingSelected != -1)
		{
			if ((g_mouseInput.xPos - 350.0f) > (-350.0f + xDifference) and (g_mouseInput.xPos - 350.0f) < (-350.0f + xDifference + 87.5f))
			{
				if ((-g_mouseInput.yPos + 360.0f) > (-350.0f + yDifference) and (-g_mouseInput.yPos + 360.0f) < (-350.0f + yDifference + 87.5f))
				{
					if (RememberTexID != 13.0f and RememberTexID != 0.0f)
					{
						//the if under here is to check if the move is legal
						if (LegalMovesForSelectedSquare.find(i - 1) != LegalMovesForSelectedSquare.end())
						{
							if(isNextMoveForWhite == Board::IsPieceColorWhite(GetPieceTypefromTexID(RememberTexID)))
							{
								static_BoardSquare[BoardSquareBeingSelected] = GetPieceTypefromTexID(RememberTexID);
								previousBoardsquare = static_BoardSquare;
								static_BoardSquare[BoardSquareBeingSelected] = 0;
								static_BoardSquare[i - 1] = GetPieceTypefromTexID(RememberTexID);
								PieceTexID = RememberTexID;
								if (i != BoardSquareBeingSelected + 1)
								{
									wasStatic_previousBoardsquareCreated = true;
									MoveNum++;
								}
								AttackedSquare = i - 1;

								//castling and en passant
								if (AttackedSquare != -1)
								{
									if (RememberTexID == 12.0f or RememberTexID == 6.0f)
									{
										if (abs(BoardSquareBeingSelected - AttackedSquare) == 2)
										{
											if (BoardSquareBeingSelected - AttackedSquare == -2)
											{
												if (BoardSquareBeingSelected == 4)
												{
													static_BoardSquare[5] = WHITE_ROOK;
													static_BoardSquare[7] = 0;
												}
												else
												{
													static_BoardSquare[61] = BLACK_ROOK;
													static_BoardSquare[63] = 0;
												}
											}
											if (BoardSquareBeingSelected - AttackedSquare == 2)
											{
												if (BoardSquareBeingSelected == 4)
												{
													static_BoardSquare[3] = WHITE_ROOK;
													static_BoardSquare[0] = 0;
												}
												else
												{
													static_BoardSquare[59] = BLACK_ROOK;
													static_BoardSquare[56] = 0;
												}
											}
										}
									}

									//White en passant
									if (RememberTexID == 7)
									{
										if (previousBoardsquare[AttackedSquare] == 0)
										{
											if (BoardSquareBeingSelected - AttackedSquare == -7 or BoardSquareBeingSelected - AttackedSquare == -9)
											{
												static_BoardSquare[AttackedSquare - 8] = 0;
											}
										}
									}
									//Black en passant
									if (RememberTexID == 1)
									{
										if (previousBoardsquare[AttackedSquare] == 0)
										{
											if (BoardSquareBeingSelected - AttackedSquare == 7 or BoardSquareBeingSelected - AttackedSquare == 9)
											{
												static_BoardSquare[AttackedSquare + 8] = 0;
											}
										}
									}
								}
								Board::WillCanCastleChange(previousBoardsquare[BoardSquareBeingSelected], BoardSquareBeingSelected, AttackedSquare, CanCastle);

								//promoting
								if (AttackedSquare >= 56 and GetPieceTypefromTexID(RememberTexID) == WHITE_PAWN or AttackedSquare <= 7 and GetPieceTypefromTexID(RememberTexID) == BLACK_PAWN )
								{
									WaitingForUserPromotion = true;
									MoveNum--;
									rememberAttackedSquareForUserPromotion = AttackedSquare;
								}


								LegalMovesForSelectedSquare.clear();
								GenerateLegalMoves::SetDoNotEnPassant(false);
								if (abs(BoardSquareBeingSelected - AttackedSquare) == 16 and p_LegalMoves->WhichBoardSquaresAreAbsPinned[AttackedSquare] != 65)
								{
									GenerateLegalMoves::SetDoNotEnPassant(true);
								}
								BoardSquareBeingSelected = -1;
								WaitingForEnemyMove = true;
							}
							else
							{
								static_BoardSquare[BoardSquareBeingSelected] = GetPieceTypefromTexID(RememberTexID);
								BoardSquareBeingSelected = -1;
							}
						}
						else
						{
							static_BoardSquare[BoardSquareBeingSelected] = GetPieceTypefromTexID(RememberTexID);
							BoardSquareBeingSelected = -1;
						}
						
					}

				}
			}
		}

		if (g_mouseInput.LeftButtonPressed and g_mouseInput.WasLeftButtonPressed and RememberTexID != 13.0f and !WaitingForUserPromotion)
		{
			quads[65] = CreateQuad(g_mouseInput.xPos - 393.5f, -g_mouseInput.yPos + 319.25f, 87.5f, RememberTexID); //took way too much time,
			//func to get the y from yPos is f(x) = -x + 360
		}

		quads[i] = CreateQuad(-350.0f + xDifference, -350.0f + yDifference, 87.5f, PieceTexID);
		
		xDifference += 87.5f;


	}

	//does promotion things
	if(WaitingForUserPromotion)
	{
		if (isNextMoveForWhite)//White promotion quads
		{
			quads[130] = CreateQuad(-87.5f, 0.0f, 175.0f, 15.0f);
			quads[131] = CreateQuad(-87.5f, 0.0f, 87.5f, 11.0f);
			quads[132] = CreateQuad(0.0f, 0.0f, 87.5f, 10.0f);
			quads[133] = CreateQuad(-87.5f, 87.5f, 87.5f, 9.0f);
			quads[134] = CreateQuad(0.0f, 87.5f, 87.5f, 8.0f);
		}
		else//Black promotion quads
		{
			quads[130] = CreateQuad(-87.5f, -175.0f, 175.0f, 15.0f);
			quads[131] = CreateQuad(-87.5f, -175.0f, 87.5f, 6.0f);
			quads[132] = CreateQuad(0.0f, -175.0f, 87.5f, 4.0f);
			quads[133] = CreateQuad(-87.5f, -87.5f, 87.5f, 3.0f);
			quads[134] = CreateQuad(0.0f, -87.5f, 87.5f, 2.0f);
		}

		//check which promotion type was selected, the code is HIGHLY unreadable, comments may help you
		if (g_mouseInput.LeftButtonPressed and g_mouseInput.WasLeftButtonPressed == false)
		{
			if (isNextMoveForWhite)//when White promotes
			{
				if (-g_mouseInput.yPos + 360.0f < 87.5f and -g_mouseInput.yPos + 360.0f >= 0.0f)//the 'bottom' ones
				{
					if (g_mouseInput.xPos - 350.0f < 0.0f and g_mouseInput.xPos - 350.0f > -87.5f)//Q
					{
						static_BoardSquare[rememberAttackedSquareForUserPromotion] = WHITE_QUEEN;
						WaitingForUserPromotion = false;
						MoveNum++;
					}
					else if (g_mouseInput.xPos - 350.0f < 87.5f and g_mouseInput.xPos - 350.0f > 0.0f)//R
					{
						static_BoardSquare[rememberAttackedSquareForUserPromotion] = WHITE_ROOK;
						WaitingForUserPromotion = false;
						MoveNum++;
					}
				}
				else if (-g_mouseInput.yPos + 360.0f <= 175.0f and -g_mouseInput.yPos + 360.0f >= 87.5f)//the 'top' ones
				{
					if (g_mouseInput.xPos - 350.0f < 0.0f and g_mouseInput.xPos - 350.0f > -87.5f)//N
					{
						static_BoardSquare[rememberAttackedSquareForUserPromotion] = WHITE_KNIGHT;
						WaitingForUserPromotion = false;
						MoveNum++;
					}
					else if (g_mouseInput.xPos - 350.0f < 87.5f and g_mouseInput.xPos - 350.0f > 0.0f)//B
					{
						static_BoardSquare[rememberAttackedSquareForUserPromotion] = WHITE_BISHOP;
						WaitingForUserPromotion = false;
						MoveNum++;
					}
				}
			}
			else//Black promotes
			{
				if (-g_mouseInput.yPos + 360.0f <= -87.5f and -g_mouseInput.yPos + 360.0f >= -175.0f)//the 'bottom' ones
				{
					if (g_mouseInput.xPos - 350.0f < 0.0f and g_mouseInput.xPos - 350.0f > -87.5f)//q
					{
						static_BoardSquare[rememberAttackedSquareForUserPromotion] = BLACK_QUEEN;
						WaitingForUserPromotion = false;
						MoveNum++;
					}
					else if (g_mouseInput.xPos - 350.0f < 87.5f and g_mouseInput.xPos - 350.0f > 0.0f)//r
					{
						static_BoardSquare[rememberAttackedSquareForUserPromotion] = BLACK_ROOK;
						WaitingForUserPromotion = false;
						MoveNum++;
					}
				}
				else if (-g_mouseInput.yPos + 360.0f <= 0.0f and -g_mouseInput.yPos + 360.0f >= -87.5f)//the 'top' ones
				{
					if (g_mouseInput.xPos - 350.0f < 0.0f and g_mouseInput.xPos - 350.0f > -87.5f)//n
					{
						static_BoardSquare[rememberAttackedSquareForUserPromotion] = BLACK_KNIGHT;
						WaitingForUserPromotion = false;
						MoveNum++;
					}
					else if (g_mouseInput.xPos - 350.0f < 87.5f and g_mouseInput.xPos - 350.0f > 0.0f)//b
					{
						static_BoardSquare[rememberAttackedSquareForUserPromotion] = BLACK_BISHOP;
						WaitingForUserPromotion = false;
						MoveNum++;
					}
				}
			}
		}

	}


	return quads;
}

//could output a vector, but this is chess, not a game engine
std::array<VertexStructure, 540> RenderChessPieces::MemcopyObjects(std::array<std::array<VertexStructure, 4Ui64>,135>& quads)
{

	std::array<VertexStructure, 540> positions{};
	for (int i = 0; i < 135; i++)
	{
		int PositionCount = i * 4;
		positions[PositionCount].Position = quads[i][0].Position;
		positions[PositionCount].TexCoords = quads[i][0].TexCoords;
		positions[PositionCount].TexID = quads[i][0].TexID;
		positions[PositionCount+1].Position = quads[i][1].Position;
		positions[PositionCount+1].TexCoords = quads[i][1].TexCoords;
		positions[PositionCount+1].TexID = quads[i][1].TexID;
		positions[PositionCount+2].Position = quads[i][2].Position;
		positions[PositionCount+2].TexCoords = quads[i][2].TexCoords;
		positions[PositionCount+2].TexID = quads[i][2].TexID;
		positions[PositionCount+3].Position = quads[i][3].Position;
		positions[PositionCount+3].TexCoords = quads[i][3].TexCoords;
		positions[PositionCount+3].TexID = quads[i][3].TexID;
	}
	
	return positions;
}

float RenderChessPieces::GetPieceTextureID(const std::array<uint8_t, 64>& BoardSquare, unsigned int i)
{
	
	switch (BoardSquare[i]) 
	{
		case 0:
			return 13.0f;//NONE
		case BLACK_PAWN:
			return 1.0f;//p
		case BLACK_BISHOP:
			return 2.0f;//b
		case BLACK_KNIGHT:
			return 3.0f;//n
		case BLACK_ROOK:
			return 4.0f;//r
		case BLACK_QUEEN:
			return 5.0f;//q
		case BLACK_KING:
			return 6.0f;//k
		case WHITE_PAWN:
			return 7.0f;//P
		case WHITE_BISHOP:
			return 8.0f;//B
		case WHITE_KNIGHT:
			return 9.0f;//N
		case WHITE_ROOK:
			return 10.0f;//R
		case WHITE_QUEEN:
			return 11.0f;//Q
		case WHITE_KING:
			return 12.0f;//K
		default:
			ASSERT(false);
			return -1;
	}
}

uint8_t RenderChessPieces::GetPieceTypefromTexID(float TexID)
{
	if (TexID == 13.0f)
	{
		return 0;//NONE
	}
	else if (TexID == 1.0f) { return BLACK_PAWN; }//p
	else if (TexID == 2.0f) { return BLACK_BISHOP; }//b
	else if (TexID == 3.0f) { return BLACK_KNIGHT; }//n
	else if (TexID == 4.0f) { return BLACK_ROOK; }//r
	else if (TexID == 5.0f) { return BLACK_QUEEN; }
	else if (TexID == 6.0f) { return BLACK_KING; }
	else if (TexID == 7.0f) { return WHITE_PAWN; }
	else if (TexID == 8.0f) { return WHITE_BISHOP; }
	else if (TexID == 9.0f) { return WHITE_KNIGHT; }
	else if (TexID == 10.0f) { return WHITE_ROOK; }
	else if (TexID == 11.0f) { return WHITE_QUEEN; }
	else if (TexID == 12.0f) { return WHITE_KING; }
	else
	{
		ASSERT(false);
		return -1;
	}
}

void RenderChessPieces::WasLeftButtonPressed()
{
	if (g_mouseInput.LeftButtonPressed == true)
	{
		g_mouseInput.WasLeftButtonPressed = true;
	}
	if (g_mouseInput.LeftButtonPressed == false)
		g_mouseInput.WasLeftButtonPressed = false;
}

void RenderChessPieces::GetMouseInput()
{
	glfwSetCursorPosCallback(window, cursorPositionCallBack);
	glfwSetMouseButtonCallback(window, mouseButtonCallBack);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
	//std::cout << g_mouseInput.xPos << ":" << g_mouseInput.yPos << '\n';
	//std::cout << "mouseLeft pressed = " << g_mouseInput.LeftButtonPressed << '\n';
	//std::cout << "was Left pressed = " << g_mouseInput.WasLeftButtonPressed << '\n';
 
}

void RenderChessPieces::SetStaticBoardSquare(const std::array<uint8_t, 64>& BoardSquare)
{
	if (wasStatic_BoardSquareCreated == false)
	{
		static_BoardSquare = BoardSquare;
		wasStatic_BoardSquareCreated = true;
	}
}

uint32_t RenderChessPieces::Perft(std::array<uint8_t, 64Ui64> BoardSquare, std::array<uint8_t, 64> perftPreviousBoardSquare, canCastle CanCastle, bool isNextMoveForWhite, uint8_t depth, bool DivideFunON, unsigned int& PerftMoveNum)
{
	uint32_t NumOfMoves = 0;

	GenerateLegalMoves LegalMoves(BoardSquare, &perftPreviousBoardSquare, CanCastle, isNextMoveForWhite, MoveNum, false);

	const auto ConstBoardSquare = BoardSquare;
	const auto ConstPreviousBoardSquare = perftPreviousBoardSquare;
	const canCastle ConstCanCastle = CanCastle;
	uint8_t count = 0;


	for (MOVE_BIT& piece : LegalMoves.moves)
	{
		for(uint8_t move = 0; move <= MAX_SQUARE; move++)
		{
			if (piece.TargetSquares[move] != false) { continue; }
			//perft the promotions, this if is basically a blunt .find()
			if (piece.Promotion[0] != 65 and piece.Promotion[0] == move or piece.Promotion[1] != 65 and piece.Promotion[1] == move or piece.Promotion[2] != 65 and piece.Promotion[2] == move)
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

						MakeMove(LegalMoves, Move_ , BoardSquare, perftPreviousBoardSquare, CanCastle);
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
					piece.Promotion[move - count - 7] = 65;
				else
					piece.Promotion[move - count + 9] = 65;

				continue;
			}

			if (depth == 1)
			{
				if(DivideFunON)
					std::cout << +count << " " << +move << ": 1" << '\n';
				NumOfMoves++;
			}
			else
			{
				Move Move_(count, move);
				
				MakeMove(LegalMoves, Move_, BoardSquare, perftPreviousBoardSquare, CanCastle);
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

void RenderChessPieces::MakeMove(const GenerateLegalMoves& LegalMoves, Move move, std::array<uint8_t, 64>& fun_BoardSquare, std::array<uint8_t, 64>& fun_previousBoardSquare, canCastle& Castle)
{
	fun_previousBoardSquare = fun_BoardSquare;
	Board::WillCanCastleChange(fun_BoardSquare[move.s_BoardSquare], move.s_BoardSquare, move.s_Move, Castle);
	fun_BoardSquare[move.s_Move] = fun_BoardSquare[move.s_BoardSquare];

	uint8_t PieceTypeToPromoteTo = 65;

	if (move.s_PromotionType < 9)
	{
		if (Board::IsPieceColorWhite(fun_BoardSquare[move.s_BoardSquare]) and move.s_PromotionType != 65)
			PieceTypeToPromoteTo = move.s_PromotionType + WHITE;
		else
			PieceTypeToPromoteTo = move.s_PromotionType + BLACK;
	}
	else
	{
		PieceTypeToPromoteTo = move.s_PromotionType;
	}

	//castling
	if (fun_BoardSquare[move.s_BoardSquare] == WHITE_KING or fun_BoardSquare[move.s_BoardSquare] == BLACK_KING)
	{
		if (move.s_BoardSquare - move.s_Move == -2)
		{
			if (move.s_BoardSquare == 4)
			{
				fun_BoardSquare[5] = WHITE_ROOK;
				fun_BoardSquare[7] = 0;
			}
			else
			{
				fun_BoardSquare[61] = BLACK_ROOK;
				fun_BoardSquare[63] = 0;
			}
		}
		if (move.s_BoardSquare - move.s_Move == 2)
		{
			if (move.s_BoardSquare == 4)
			{
				fun_BoardSquare[3] = WHITE_ROOK;
				fun_BoardSquare[0] = 0;
			}
			else
			{
				fun_BoardSquare[59] = BLACK_ROOK;
				fun_BoardSquare[56] = 0;
			}
		}

	}

	//promoting and en passant
	if (PieceTypeToPromoteTo != 65)
	{
		fun_BoardSquare[move.s_Move] = PieceTypeToPromoteTo;
	}
	else//optimization
	{
		//White en passant
		if (fun_BoardSquare[move.s_BoardSquare] == WHITE_PAWN)
		{
			if (fun_previousBoardSquare[move.s_Move] == 0)
			{
				if (move.s_BoardSquare - move.s_Move == -7 or move.s_BoardSquare - move.s_Move == -9)
				{
					fun_BoardSquare[move.s_Move - 8] = 0;
				}
			}
		}
		//Black en passant
		if (fun_BoardSquare[move.s_BoardSquare] == BLACK_PAWN)
		{
			if (fun_previousBoardSquare[move.s_Move] == 0)
			{
				if (move.s_BoardSquare - move.s_Move == 7 or move.s_BoardSquare - move.s_Move == 9)
				{
					fun_BoardSquare[move.s_Move + 8] = 0;
				}
			}
		}
	}

	fun_BoardSquare[move.s_BoardSquare] = 0;
	
	GenerateLegalMoves::SetDoNotEnPassant(false);
	if (abs(move.s_BoardSquare - move.s_Move) == 16 and LegalMoves.WhichBoardSquaresAreAbsPinned[move.s_Move] != 65)
	{
		GenerateLegalMoves::SetDoNotEnPassant(true);
	}
}

void RenderChessPieces::CreatePerft(uint8_t PerftDepth)
{
	//std::cout << "Worker at start CPU: " << GetCurrentProcessorNumber() << std::endl;
	canCastle perftCastle = CanCastle;
	auto perftBoardsquare = static_BoardSquare;
	auto perftPreviousBoardsquare = previousBoardsquare;
	auto Movenum = MoveNum;

	bool isNextMoveForWhite = true;
	if (Movenum % 2 != 0)
		isNextMoveForWhite = false;

	auto start = std::chrono::high_resolution_clock::now();
	uint32_t result = Perft(perftBoardsquare, perftPreviousBoardsquare, perftCastle, isNextMoveForWhite, PerftDepth, true, Movenum);
	std::cout << "Nodes searched: " << result << '\n';
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	std::cout << "In " << duration.count() << " ms" << '\n' << std::endl;

	//std::cout << "Worker CPU: " << GetCurrentProcessorNumber() << std::endl;
}

void RenderChessPieces::SetMoveNum(const uint32_t& Movenum)
{
	MoveNum = Movenum;
}

void RenderChessPieces::SetCanCastle(const canCastle& f_canCastle)
{
	CanCastle = f_canCastle;
}

void RenderChessPieces::SetPrevBoardSquare_FEN_EP(const uint32_t& f_BoardSquare)
{
	if (f_BoardSquare == 65)
		return;
	uint32_t f_previousBoardSquare = 0;
	wasStatic_previousBoardsquareCreated = true;
	if(f_BoardSquare < 32)
		f_previousBoardSquare = f_BoardSquare - 16;
	else
		f_previousBoardSquare = f_BoardSquare + 16;
	auto BS_copy = static_BoardSquare;
	auto f_PieceType = BS_copy[f_BoardSquare];
	BS_copy[f_BoardSquare] = 0;
	BS_copy[f_previousBoardSquare] = f_PieceType;
	previousBoardsquare = BS_copy;
}

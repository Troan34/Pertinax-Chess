#include "RenderChesspcs.h"
static MouseInput g_mouseInput;
static float RememberTexID;
static int RememberBoardSquareID;
static std::array<unsigned int, 64Ui64> static_BoardSquare;
static bool wasStatic_BoardSquareCreated = false;

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

RenderChessPieces::RenderChessPieces()
{
}
RenderChessPieces::~RenderChessPieces()
{

}

std::array<std::array<VertexStructure, 4Ui64>, 66> RenderChessPieces::CreateObjects()
{
	std::array<std::array<VertexStructure, 4Ui64>, 66> quads{};
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

		//drag and drop
		if (g_mouseInput.LeftButtonPressed and g_mouseInput.WasLeftButtonPressed == false and PieceTexID != 0.0f)
		{
			//next two ifs are to check which objects has to be dragged
			if ((g_mouseInput.xPos - 350.0f) > (-350.0f + xDifference) and (g_mouseInput.xPos - 350.0f) < (-350.0f + xDifference + 87.5f))
			{
				if ((-g_mouseInput.yPos + 360.0f) > (-350.0f + yDifference) and (-g_mouseInput.yPos + 360.0f) < (-350.0f + yDifference + 87.5f))
				{
					quads[65] = CreateQuad(g_mouseInput.xPos - 393.5f, -g_mouseInput.yPos + 319.25f, 87.5f, PieceTexID);
					RememberTexID = PieceTexID;
					static_BoardSquare[i - 1] = (unsigned int)0;					
				}
			}
		}
		
		//this if is to "drop" the object
		//TODO: when we have rules this cant be just here, like if a move is illegal the piece return here from the cursor, or you cant just eat your pieces...
		if (g_mouseInput.LeftButtonPressed == false and g_mouseInput.WasLeftButtonPressed == true)
		{
			if ((g_mouseInput.xPos - 350.0f) > (-350.0f + xDifference) and (g_mouseInput.xPos - 350.0f) < (-350.0f + xDifference + 87.5f))
			{
				if ((-g_mouseInput.yPos + 360.0f) > (-350.0f + yDifference) and (-g_mouseInput.yPos + 360.0f) < (-350.0f + yDifference + 87.5f))
				{
					static_BoardSquare[static_cast<std::array<unsigned int, 64Ui64>::size_type>(i) - 1] = GetBoardSquarefromTexID(RememberTexID);
					PieceTexID = RememberTexID;
				}
			}
		}
		if (g_mouseInput.LeftButtonPressed and g_mouseInput.WasLeftButtonPressed and RememberTexID != 0)
		{
			quads[65] = CreateQuad(g_mouseInput.xPos - 393.5f, -g_mouseInput.yPos + 319.25f, 87.5f, RememberTexID); //took way too much time,
			//func to get the y from yPos is f(x) = -x + 360
		}

		quads[i] = CreateQuad(-350.0f + xDifference, -350.0f + yDifference, 87.5f, PieceTexID);
		

		
		

		xDifference += 87.5f;
	}
	
	return quads;
}

//could output a vector, but this is chess, not a game engine
std::array<VertexStructure, 264> RenderChessPieces::MemcopyObjects(std::array<std::array<VertexStructure, 4Ui64>,66> quads)
{

	std::array<VertexStructure, 264> positions{};
	for (int i = 0; i < 66; i++)
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


float RenderChessPieces::GetPieceTextureID(std::array<unsigned int, 64> BoardSquare, unsigned int i)
{
	
	switch (BoardSquare[i]) 
	{
		case 0:
			return 13.0f;//none
		case 9:
			return 1.0f;//p
		case 10:
			return 2.0f;//b
		case 11:
			return 3.0f;//n
		case 12:
			return 4.0f;//r
		case 13:
			return 5.0f;//q
		case 14:
			return 6.0f;//k
		case 17:
			return 7.0f;//P
		case 18:
			return 8.0f;//B
		case 19:
			return 9.0f;//N
		case 20:
			return 10.0f;//R
		case 21:
			return 11.0f;//Q
		case 22:
			return 12.0f;//K
		default:
			ASSERT(false);
			return -1;
	}
}

float RenderChessPieces::GetBoardSquarefromTexID(float TexID)
{
	if (TexID == 13.0f)
	{
		return 0;//none
	}
	else if (TexID == 1.0f) { return 9; }//p
	else if (TexID == 2.0f) { return 10; }//b
	else if (TexID == 3.0f) { return 11; }//n
	else if (TexID == 4.0f) { return 12; }//r
	else if (TexID == 5.0f) { return 13; }
	else if (TexID == 6.0f) { return 14; }
	else if (TexID == 7.0f) { return 17; }
	else if (TexID == 8.0f) { return 18; }
	else if (TexID == 9.0f) { return 19; }
	else if (TexID == 10.0f) { return 20; }
	else if (TexID == 11.0f) { return 21; }
	else if (TexID == 12.0f) { return 22; }
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

void RenderChessPieces::GetMouseInput(GLFWwindow* window)
{
	glfwSetCursorPosCallback(window, cursorPositionCallBack);
	glfwSetMouseButtonCallback(window, mouseButtonCallBack);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
	//std::cout << g_mouseInput.xPos << ":" << g_mouseInput.yPos << '\n';
	//std::cout << "mouseLeft pressed = " << g_mouseInput.LeftButtonPressed << '\n';
	//std::cout << "was Left pressed = " << g_mouseInput.WasLeftButtonPressed << '\n';
 
}

void RenderChessPieces::SetStaticBoardSquare(std::array<unsigned int, 64> BoardSquare)
{
	if (wasStatic_BoardSquareCreated == false)
	{
		static_BoardSquare = BoardSquare;
		wasStatic_BoardSquareCreated = true;
	}
}




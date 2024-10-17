#include "RenderChesspcs.h"
static MouseInput g_mouseInput;

static void cursorPositionCallBack(GLFWwindow* window, double xPosition, double yPosition)
{
	g_mouseInput.xPos = (float)xPosition;
	g_mouseInput.yPos = (float)yPosition;
}

static void mouseButtonCallBack(GLFWwindow* window, int button, int action, int mods)
{
	if (g_mouseInput.LeftButtonPressed)
	{
		g_mouseInput.WasLeftButtonPressed = true;
	}	

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
std::array<std::array<VertexStructure, 4Ui64>, 66> RenderChessPieces::CreateObjects(std::array<unsigned int, 64Ui64> BoardSquare)
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
		float PieceTexID = GetPieceTextureID(BoardSquare, i - 1);
		quads[i] = CreateQuad(-350.0f + xDifference, -350.0f + yDifference, 87.5f, PieceTexID);
		xDifference += 87.5f;

		//drag and drop
		if (g_mouseInput.LeftButtonPressed and g_mouseInput.WasLeftButtonPressed == false)
		{
			if ((g_mouseInput.xPos + 353.0f) < (-350.0f + xDifference) and (g_mouseInput.xPos + 353.0f) >(-350.0f + (xDifference - 87.5f)))
			{
				if ((g_mouseInput.yPos + 363.0f) < (-350.0f + yDifference + 87.5f) and (g_mouseInput.yPos + 363.0f) > (-350.0f + yDifference))
				{
					quads[65] = CreateQuad(g_mouseInput.xPos, g_mouseInput.yPos + 319.25f, 87.5f, PieceTexID);
				}
			}
		}
		if (g_mouseInput.LeftButtonPressed and g_mouseInput.WasLeftButtonPressed)
		{
			quads[65] = CreateQuad(g_mouseInput.xPos - 393.5f, g_mouseInput.yPos + 319.25f, 87.5f, PieceTexID);
		}
	}
	
	return quads;
}
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
void RenderChessPieces::BindEveryTexture()
{
	m_TextureChessBoard = std::make_unique<Texture>("res/textures/Chessboard.png");
	m_TextureP = std::make_unique<Texture>("res/textures/bp.png");
	m_TextureB = std::make_unique<Texture>("res/textures/bb.png");
	m_TextureN = std::make_unique<Texture>("res/textures/bn.png");
	m_TextureR = std::make_unique<Texture>("res/textures/br.png");
	m_TextureQ = std::make_unique<Texture>("res/textures/bq.png");
	m_TextureK = std::make_unique<Texture>("res/textures/bk.png");
	m_Texturep = std::make_unique<Texture>("res/textures/wp.png");
	m_Textureb = std::make_unique<Texture>("res/textures/wb.png");
	m_Texturen = std::make_unique<Texture>("res/textures/wn.png");
	m_Texturer = std::make_unique<Texture>("res/textures/wr.png");
	m_Textureq = std::make_unique<Texture>("res/textures/wq.png");
	m_Texturek = std::make_unique<Texture>("res/textures/wk.png");
	m_TextureEmpty = std::make_unique<Texture>("res/textures/Empty.png");
	m_TextureChessBoard->Bind(0);
	m_TextureP->Bind(1);
	m_TextureB->Bind(2);
	m_TextureN->Bind(3);
	m_TextureR->Bind(4);
	m_TextureQ->Bind(5);
	m_TextureK->Bind(6);
	m_Texturep->Bind(7);
	m_Textureb->Bind(8);
	m_Texturen->Bind(9);
	m_Texturer->Bind(10);
	m_Textureq->Bind(11);
	m_Texturek->Bind(12);
	m_TextureEmpty->Bind(13);

}

float RenderChessPieces::GetPieceTextureID(std::array<unsigned int, 64> BoardSquare, unsigned int i)
{
	
	switch (BoardSquare[i]) 
	{
		case 0:
			return 13.0f;//none
		case 9:
			return 1.0f;//P
		case 10:
			return 2.0f;//B
		case 11:
			return 3.0f;//N
		case 12:
			return 4.0f;//R
		case 13:
			return 5.0f;//Q
		case 14:
			return 6.0f;//K
		case 17:
			return 7.0f;//p
		case 18:
			return 8.0f;//b
		case 19:
			return 9.0f;//n
		case 20:
			return 10.0f;//r
		case 21:
			return 11.0f;//q
		case 22:
			return 12.0f;//k
		default:
			ASSERT(false);
			return -1;
	}
}

void RenderChessPieces::GetMouseInput(GLFWwindow* window)
{
	glfwSetCursorPosCallback(window, cursorPositionCallBack);
	glfwSetMouseButtonCallback(window, mouseButtonCallBack);

	std::cout << g_mouseInput.xPos << ":" << g_mouseInput.yPos << '\n';
	std::cout << "mouseLeft pressed = " << g_mouseInput.LeftButtonPressed << '\n';
	

}



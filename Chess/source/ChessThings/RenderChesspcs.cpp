#include "RenderChesspcs.h"
RenderChessPieces::RenderChessPieces()
{
}
RenderChessPieces::~RenderChessPieces()
{

}
std::array<std::array<VertexStructure, 4Ui64>, 65> RenderChessPieces::CreateObjects(std::array<unsigned int, 64Ui64> BoardSquare)
{
	std::array<std::array<VertexStructure, 4Ui64>, 65> quads{};
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
		quads[i] = CreateQuad(-350.0f + xDifference, -350.0f + yDifference, 87.5f, GetPieceTextureID(BoardSquare, i-1));
		xDifference += 87.5f;
	}
	return quads;
}
std::array<VertexStructure, 260> RenderChessPieces::MemcopyObjects(std::array<std::array<VertexStructure, 4Ui64>,65> quads)
{

	std::array<VertexStructure, 260> positions{};
	for (int i = 0; i < 65; i++)
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



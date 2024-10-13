#pragma once
#include "CreateQuadDynamic.h"
#include "Renderer.h"
#include "Texture.h"
#include <memory>
#include <array>
#include <iostream>
class RenderChessPieces
{
private:
	std::unique_ptr<Texture> m_TextureChessBoard;
	std::unique_ptr<Texture> m_TextureP;
	std::unique_ptr<Texture> m_TextureB;
	std::unique_ptr<Texture> m_TextureN;
	std::unique_ptr<Texture> m_TextureR;
	std::unique_ptr<Texture> m_TextureQ;
	std::unique_ptr<Texture> m_TextureK;
	std::unique_ptr<Texture> m_Texturep;
	std::unique_ptr<Texture> m_Textureb;
	std::unique_ptr<Texture> m_Texturen;
	std::unique_ptr<Texture> m_Texturer;
	std::unique_ptr<Texture> m_Textureq;
	std::unique_ptr<Texture> m_Texturek;
	std::unique_ptr<Texture> m_TextureEmpty;
	std::array<unsigned int, 64> BoardSquare;

public:
	RenderChessPieces();
	~RenderChessPieces();

	std::array<std::array<VertexStructure, 4Ui64>,65> CreateObjects(std::array<unsigned int, 64> BoardSquare);
	std::array<VertexStructure, 260> MemcopyObjects(std::array<std::array<VertexStructure, 4Ui64>,65> quads);
	void BindEveryTexture();
	float GetPieceTextureID(std::array<unsigned int, 64> BoardSquare, unsigned int i);
};
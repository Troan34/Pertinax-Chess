#pragma once
#include "CreateQuadDynamic.h"
#include "Renderer.h"
#include "Texture.h"
#include <memory>
#include <array>
#include <iostream>
#include<GL/glew.h>
#include <GLFW/glfw3.h>
#include "LegalMoves.h"

struct MouseInput
{
	float xPos, yPos;
	bool LeftButtonPressed;
	bool WasLeftButtonPressed;
};

struct Vec2MouseInput
{
	float xPoss, yPoss;
};

class RenderChessPieces
{
private:
	std::array<unsigned int, 64> BoardSquare;
	

public:
	RenderChessPieces();
	~RenderChessPieces();
	std::array<std::array<VertexStructure, 4Ui64>,130> CreateObjects();
	std::array<VertexStructure, 520> MemcopyObjects(std::array<std::array<VertexStructure, 4Ui64>,130> quads);
	void WillCanCastleChange(unsigned int PieceType, unsigned int BoardSquareNum);
	void WasLeftButtonPressed();
	float GetPieceTypefromTexID(float TexID);
	float GetPieceTextureID(std::array<unsigned int, 64> BoardSquare, unsigned int i);
	void SetStaticBoardSquare(std::array<unsigned int, 64> BoardSquare);
	void GetMouseInput(GLFWwindow* window);
};
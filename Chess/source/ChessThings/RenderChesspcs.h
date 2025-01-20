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
#include <algorithm>
#include <unordered_set>
#include <chrono>
#include <string>
#include <atomic>
#include <thread>
#include <mutex>
#include <windows.h>


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
	std::array<VertexStructure, 520> MemcopyObjects(std::array<std::array<VertexStructure, 4Ui64>,130>& quads);
	void WillCanCastleChange(unsigned int PieceTypeThatMoved, unsigned int BoardSquareNum, canCastle& Castle);
	void WasLeftButtonPressed();
	float GetPieceTypefromTexID(float TexID);
	float GetPieceTextureID(std::array<unsigned int, 64> BoardSquare, unsigned int i);
	void SetStaticBoardSquare(const std::array<unsigned int, 64>& BoardSquare);
	void GetMouseInput(GLFWwindow* window);
	uint32_t Perft(std::array<unsigned int, 64Ui64> BoardSquare, std::array<unsigned int, 64> previousBoardSquare, canCastle CanCastle, bool isNextMoveForWhite, uint8_t depth, bool DivideFunON);
	void MakeMove(unsigned int BoardSquare, unsigned int move, std::array<unsigned int, 64>& fun_BoardSquare, std::array<unsigned int, 64>& fun_previousBoardSquare, canCastle& Castle);
	void CreatePerft(uint8_t PerftDepth);
};
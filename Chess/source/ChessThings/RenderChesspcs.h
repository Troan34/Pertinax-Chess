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
#include <future>


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
	std::vector<std::future<void>> void_Futures;
	std::array<std::array<VertexStructure, 4Ui64>, 135> quads;
public:
	RenderChessPieces();
	~RenderChessPieces();

	std::array<std::array<VertexStructure, 4Ui64>,135> CreateObjects();
	std::array<VertexStructure, 540> MemcopyObjects(std::array<std::array<VertexStructure, 4Ui64>,135>& quads);

	void WillCanCastleChange(unsigned int PieceTypeThatMoved, unsigned int BoardSquareItMovedFrom, const unsigned int& BoardSquareItMovedTo, canCastle& Castle);
	void WasLeftButtonPressed();
	uint8_t GetPieceTypefromTexID(float TexID);
	float GetPieceTextureID(std::array<unsigned int, 64> BoardSquare, unsigned int i);
	void SetStaticBoardSquare(const std::array<unsigned int, 64>& BoardSquare);
	void GetMouseInput(GLFWwindow* window);
	
	uint32_t Perft(std::array<unsigned int, 64Ui64> BoardSquare, std::array<unsigned int, 64> previousBoardSquare, canCastle CanCastle, bool isNextMoveForWhite, uint8_t depth, bool DivideFunON, unsigned int& PerftMoveNum);
	void MakeMove(unsigned int BoardSquare, unsigned int move, unsigned int& PerftMoveNum, std::array<unsigned int, 64>& fun_BoardSquare, std::array<unsigned int, 64>& fun_previousBoardSquare, canCastle& Castle, const uint8_t& PieceTypeToPromoteTo);
	void CreatePerft(uint8_t PerftDepth);
	
	void SetMoveNum(const uint32_t& Movenum);
	void SetCanCastle(const canCastle& f_canCastle);
	void SetPrevBoardSquare_FEN_EP(const uint32_t& f_BoardSquare);


};
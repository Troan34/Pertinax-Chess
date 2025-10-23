#pragma once
#include "CreateQuadDynamic.h"
#include "Renderer.h"
#include "Texture.h"
#include <memory>
#include <array>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "LegalMoves.h"
#include <algorithm>
#include <chrono>
#include <string>
#include <atomic>
#include <thread>
#include <mutex>
#include <windows.h>
#include <future>
#include "ChessThings/Engine/Search.h"
#include "ChessThings/UCI/UCI.h"
#include "ChessThings/Engine/IterativeD.h"


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
	std::array<uint8_t, 64> BoardSquare;
	std::vector<std::future<void>> void_Futures;
	std::array<std::array<VertexStructure, 4Ui64>, 135> quads;
	GLFWwindow* window;
public:
	RenderChessPieces(GLFWwindow* window);

	std::array<std::array<VertexStructure, 4Ui64>,135> CreateObjects();
	std::array<VertexStructure, 540> MemcopyObjects(std::array<std::array<VertexStructure, 4Ui64>,135>& quads);


	void WasLeftButtonPressed();
	uint8_t GetPieceTypefromTexID(float TexID);
	float GetPieceTextureID(const std::array<uint8_t, 64>& BoardSquare, unsigned int i);
	void SetStaticBoardSquare(const std::array<uint8_t, 64>& BoardSquare);
	void GetMouseInput();

	static void MakeMove(const GenerateLegalMoves& LegalMoves,Move move, std::array<uint8_t, 64>& fun_BoardSquare, std::array<uint8_t, 64>& fun_previousBoardSquare, CastlingAbility& Castle);
	void CreatePerft(uint8_t PerftDepth);
	
	void SetMoveNum(const uint32_t& Movenum);
	void SetCanCastle(const CastlingAbility& f_canCastle);
	void SetPrevBoardSquare_FEN_EP(const uint32_t& f_BoardSquare);


};
#pragma once
#include "Test.h"
#include<GL/glew.h>
#include <GLFW/glfw3.h>
#include "VertexBufferLayout.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include <memory>
#include "CreateQuadDynamic.h"
#include "ChessThings/Board.h"
#include "ChessThings/RenderChesspcs.h"
#include <string>
#include "ChessThings/LegalMoves.h"

namespace test {

	class TestTexture2D : public Test
	{
	public:
		TestTexture2D();
		~TestTexture2D();

		void OnUpdate(float deltaTime) override;
		void OnRender(GLFWwindow* window) override;
		void OnImGuiRender() override;
	private:
		
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<Shader> m_Shader;

		//rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
		//r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1
		//8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1

		
		glm::mat4 m_Proj, m_View;
		glm::vec3 m_TranslationA, m_TranslationB;


		GLuint TEX_ChessBoard, TEX_B, TEX_N, TEX_R, TEX_Q, TEX_P, TEX_p, TEX_b, TEX_n, TEX_r, TEX_K, TEX_k, TEX_q, TEX_Empty, TEX_Red, TEX_Prom;
		
	};

}

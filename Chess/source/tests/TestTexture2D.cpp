#include "TestTexture2D.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Renderer.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "CreateQuadDynamic.h"
#include "ChessThings/Board.h"
#include "ChessThings/RenderChesspcs.h"

namespace test {
	TestTexture2D::TestTexture2D()
		:m_Proj(glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f, -1.0f, 1.0f)),
		m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
		m_TranslationA(353,363,0), m_TranslationB(400,200,0)
		
	{
		glm::vec3 translationA(200, 200, 0);


		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		m_VAO = std::make_unique<VertexArray>();
		
		m_VertexBuffer = std::make_unique<VertexBuffer>(nullptr, sizeof(VertexStructure)*2000);
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		layout.Push<float>(1);
		m_VAO -> AddBuffer(*m_VertexBuffer, layout);
		m_IndexBuffer = std::make_unique<IndexBuffer>(66);

		m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
		m_Shader->Bind();

		//texture sampling part
		int samplers[14] = { 0, 1, 2, 3, 4, 5 ,6, 7, 8, 9, 10, 11, 12 ,13 };
		m_Shader->SetUniform1iv("u_Textures", 14, *samplers);

		

	}

	TestTexture2D::~TestTexture2D()
	{

	}

	void TestTexture2D::OnUpdate(float deltaTime)
	{

	}

	void TestTexture2D::OnRender(GLFWwindow* window)//TODO: i have to do in some way both cursor position and Left mouse click, to drag and drop pieces
	{


		Board board;
		RenderChessPieces renderChessPieces;
		renderChessPieces.GetMouseInput(window);
		renderChessPieces.SetStaticBoardSquare(board.GetPositionFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
		auto position = renderChessPieces.MemcopyObjects(renderChessPieces.CreateObjects());
		
		
		
		m_VertexBuffer->SetDynamicVB(&position, sizeof(position));

		Renderer renderer;
		
		renderChessPieces.BindEveryTexture();//this is so slow because of stbi_load_images, it's probably because i create a tex every render, i should stop using unique_ptr
		//but uhhh, i have an idea on how to do that, but it will take time, so i will do in later in the project when i have some good experience
		//TODO: Actually, i have no idea how to fix the artifacts

		renderChessPieces.WasLeftButtonPressed();

		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);
			glm::mat4 mvp = m_Proj * m_View * model;
			m_Shader->Bind();
			m_Shader->SetUniformMat4f("u_MVP", mvp);
			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
		}
		
	}

	void TestTexture2D::OnImGuiRender()
	{
		ImGui::SliderFloat3("Translation A", &m_TranslationA.x, 0.0f, 1280.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	
	}

}
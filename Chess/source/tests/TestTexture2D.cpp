#include "TestTexture2D.h"
#include <GL/glew.h>
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
		m_IndexBuffer = std::make_unique<IndexBuffer>(65);

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

	void TestTexture2D::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
		/*
		float positions[] = {
			-350.0f, -350.0f, 0.0f, 0.0f, 0.0f,
			350.0f, -350.0f, 1.0f, 0.0f, 0.0f,
			350.0f, 350.0f, 1.0f, 1.0f, 0.0f,
			-350.0f, 350.0f, 0.0f, 1.0f, 0.0f,

			85.5f, 262.5f, 0.0f, 0.0f, 1.0f,
			175.0f, 262.5f, 1.0f, 0.0f, 1.0f,
			175.0f, 350.0f, 1.0f, 1.0f, 1.0f,
			87.5f, 350.0f, 0.0f, 1.0f, 1.0f
		};
		*/
		
		Board board;
		RenderChessPieces renderChessPieces;
		
		auto position = renderChessPieces.MemcopyObjects(renderChessPieces.CreateObjects(board.GetPositionFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")));
		
		
		m_VertexBuffer->SetDynamicVB(&position, sizeof(position));

		Renderer renderer;
		
		renderChessPieces.BindEveryTexture();//this is so slow because of stbi_load_images TODO: look what is wrong
		

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
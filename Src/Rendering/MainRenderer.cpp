#include "MainRenderer.h"

#include <Managers/ModelManager.h>
#include <Managers/InputManager.h>
#include <Rendering/Objects/Model.h>
#include <glm/gtc/matrix_transform.hpp>

using namespace Rendering;

MainRenderer* MainRenderer::m_Instance = nullptr;

MainRenderer* MainRenderer::GetInstance()
{
	if (!m_Instance)
		m_Instance = new MainRenderer();

	return m_Instance;
}

void MainRenderer::DestroyInstance()
{
	if (!m_Instance)
		return;

	delete m_Instance;
	m_Instance = nullptr;
}

//////////////////////////////////////////////////////////////////////////

MainRenderer::MainRenderer()
{

}

MainRenderer::~MainRenderer()
{

}

bool MainRenderer::Init()
{
	m_ProjectionMatrix = glm::perspective(glm::radians(70.0f), 4.0f / 3.0f, 0.1f, 1000.f);

	m_ViewMatrix = glm::lookAt(
		glm::vec3(0, 3, 0.0001),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);

	m_LightPosition = glm::vec3(4, 4, 4);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	return true;
}

void MainRenderer::Update(double p_Delta)
{

}

void MainRenderer::Render(double p_Delta)
{
	auto s_Models = Managers::ModelManager::GetInstance()->GetModels();

	for (auto s_ModelGroup : s_Models)
	{
		for (auto s_Model : s_ModelGroup.second)
		{
			if (Managers::InputManager::GetInstance()->IsKeyboardKeyPressed(GLFW_KEY_D))
				s_Model->Translate(glm::vec3(0.0005f, 0.f, 0.f));

			if (Managers::InputManager::GetInstance()->IsKeyboardKeyPressed(GLFW_KEY_Q))
				s_Model->Translate(glm::vec3(0.f, 0.0005f, 0.f));

			if (Managers::InputManager::GetInstance()->IsKeyboardKeyPressed(GLFW_KEY_A))
				s_Model->Translate(glm::vec3(-0.0005f, 0.f, 0.f));

			if (Managers::InputManager::GetInstance()->IsKeyboardKeyPressed(GLFW_KEY_E))
				s_Model->Translate(glm::vec3(0.f, -0.0005f, 0.f));

			if (Managers::InputManager::GetInstance()->IsKeyboardKeyPressed(GLFW_KEY_S))
				s_Model->Translate(glm::vec3(0.f, 0.f, 0.0005f));

			if (Managers::InputManager::GetInstance()->IsKeyboardKeyPressed(GLFW_KEY_W))
				s_Model->Translate(glm::vec3(0.f, 0.f, -0.0005f));
		}
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.f, 1.f, 1.f, 1.f);

	RenderModels();
}

void MainRenderer::RenderModels()
{
	auto s_Models = Managers::ModelManager::GetInstance()->GetModels();

	// Models are grouped by the shader program they use.
	// Iterate the groups, start using the program, and render every model in the group.
	for (auto s_ModelGroup : s_Models)
	{
		glUseProgram(s_ModelGroup.first);

		// TODO: This should be precached. Introduce a new shader wrapper class which caches these.
		GLuint s_ProjectionMatrixLocation = glGetUniformLocation(s_ModelGroup.first, "projection");
		GLuint s_ViewMatrixLocation = glGetUniformLocation(s_ModelGroup.first, "view");
		GLuint s_ModelMatrixLocation = glGetUniformLocation(s_ModelGroup.first, "model");
		GLuint s_LightVectorLocation = glGetUniformLocation(s_ModelGroup.first, "light");

		for (auto s_Model : s_ModelGroup.second)
		{
			// Set all the matrices.
			glUniformMatrix4fv(s_ProjectionMatrixLocation, 1, GL_FALSE, &m_ProjectionMatrix[0][0]);
			glUniformMatrix4fv(s_ViewMatrixLocation, 1, GL_FALSE, &m_ViewMatrix[0][0]);
			glUniformMatrix4fv(s_ModelMatrixLocation, 1, GL_FALSE, &s_Model->ModelMatrix()[0][0]);

			// Set the light position.
			glUniform3f(s_LightVectorLocation, m_LightPosition.x, m_LightPosition.y, m_LightPosition.z);

			// Set Vertices.
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, s_Model->VertexBuffer());
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

			// Set UVs.
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, s_Model->UVBuffer());
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

			// Set Normals.
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, s_Model->NormalBuffer());
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

			// Set Indices.
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Model->IndexBuffer());

			// Draw all our triangles.
			glDrawElements(GL_TRIANGLES, (GLsizei) s_Model->IndexCount(), GL_UNSIGNED_SHORT, nullptr);

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
		}
	}
}
#include "MainRenderer.h"

#include <Application.h>
#include <Managers/ModelManager.h>
#include <Managers/InputManager.h>
#include <Managers/CameraManager.h>
#include <Managers/ShaderManager.h>
#include <Managers/TextureManager.h>
#include <Rendering/Objects/Model.h>

#include <Rendering/Textures/Texture.h>
#include <Rendering/Textures/CharacterTexture.h>

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
	m_ProjectionMatrix = glm::perspective(glm::radians(Managers::CameraManager::GetInstance()->FOV()),
		(float) Application::GetInstance()->WindowWidth() / (float) Application::GetInstance()->WindowHeight(), 
		0.1f, 1000.f);

	m_OrthoProjection = glm::ortho(0.f, (float) Application::GetInstance()->WindowWidth(), (float) Application::GetInstance()->WindowHeight(), 0.f);

	m_ViewMatrix = glm::lookAt(
		Managers::CameraManager::GetInstance()->EyePosition(),
		Managers::CameraManager::GetInstance()->LookAtPosition(),
		glm::vec3(0, 1, 0)
	);

	m_LightPosition = glm::vec3(50, 50, 50);

	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	return true;
}

void MainRenderer::Update(double p_Delta)
{

}

glm::vec3 MainRenderer::ScreenToWorld(int p_X, int p_Y)
{
	auto s_ViewProjection = glm::inverse(m_ProjectionMatrix * m_ViewMatrix);

	auto s_WorldCoordinates = s_ViewProjection * glm::vec4(
		2.0 * Managers::InputManager::GetInstance()->GetCursorX() / Application::GetInstance()->WindowWidth() - 1, 
		-2.0 * Managers::InputManager::GetInstance()->GetCursorY() / Application::GetInstance()->WindowHeight() + 1, 
		0.0,
		1.0
	);

	s_WorldCoordinates *= Managers::CameraManager::GetInstance()->EyePosition().y;

	return glm::vec3(s_WorldCoordinates.x, s_WorldCoordinates.y, s_WorldCoordinates.z);
}

void MainRenderer::Render(double p_Delta)
{
	if (Managers::CameraManager::GetInstance()->Dirty())
	{
		Managers::CameraManager::GetInstance()->Dirty(false);

		m_ProjectionMatrix = glm::perspective(glm::radians(Managers::CameraManager::GetInstance()->FOV()),
			(float) Application::GetInstance()->WindowWidth() / (float) Application::GetInstance()->WindowHeight(),
			0.1f, 1000.f);

		m_OrthoProjection = glm::ortho(0.f, (float) Application::GetInstance()->WindowWidth(), (float) Application::GetInstance()->WindowHeight(), 0.f);

		m_ViewMatrix = glm::lookAt(
			Managers::CameraManager::GetInstance()->EyePosition(),
			Managers::CameraManager::GetInstance()->LookAtPosition(),
			glm::vec3(0, 1, 0)
		);
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
		if (s_ModelGroup.second.size() == 0)
			continue;

		auto s_Program = (*s_ModelGroup.second.begin())->ShaderProgram();

		glUseProgram(s_Program->GetProgram());

		GLuint s_ProjectionMatrixLocation = s_Program->GetUniformLocation("p");
		GLuint s_ViewMatrixLocation = s_Program->GetUniformLocation("v");
		GLuint s_ModelMatrixLocation = s_Program->GetUniformLocation("m");
		GLuint s_LightVectorLocation = s_Program->GetUniformLocation("l");
		GLuint s_ColorVectorLocation = s_Program->GetUniformLocation("c");
		GLuint s_TextureLocation = s_Program->GetUniformLocation("t");

		for (auto s_Model : s_ModelGroup.second)
		{
			// Set all the matrices.
			if (s_ProjectionMatrixLocation != -1)
				glUniformMatrix4fv(s_ProjectionMatrixLocation, 1, GL_FALSE, &m_ProjectionMatrix[0][0]);

			if (s_ViewMatrixLocation != -1)
				glUniformMatrix4fv(s_ViewMatrixLocation, 1, GL_FALSE, &m_ViewMatrix[0][0]);

			if (s_ModelMatrixLocation != -1)
				glUniformMatrix4fv(s_ModelMatrixLocation, 1, GL_FALSE, &s_Model->ModelMatrix()[0][0]);

			// Set the light position.
			if (s_LightVectorLocation != -1)
				glUniform3f(s_LightVectorLocation, m_LightPosition.x, m_LightPosition.y, m_LightPosition.z);

			// Set the model color.
			if (s_ColorVectorLocation != -1)
				glUniform3f(s_ColorVectorLocation, s_Model->Color().x, s_Model->Color().y, s_Model->Color().z);

			// Set the model texture.
			if (s_TextureLocation != -1 && s_Model->Texture() != nullptr)
			{
				glBindTexture(GL_TEXTURE_2D, s_Model->Texture()->GetGLTexture());
				glUniform1i(s_TextureLocation, 0);
			}

			// Set Vertices.
			glEnableVertexAttribArray(s_Program->GetAttributeLocation("mv"));
			glBindBuffer(GL_ARRAY_BUFFER, s_Model->VertexBuffer());
			glVertexAttribPointer(s_Program->GetAttributeLocation("mv"), 3, GL_FLOAT, GL_FALSE, 0, nullptr);


			// Set UVs.
			glEnableVertexAttribArray(s_Program->GetAttributeLocation("mu"));
			glBindBuffer(GL_ARRAY_BUFFER, s_Model->UVBuffer());
			glVertexAttribPointer(s_Program->GetAttributeLocation("mu"), 2, GL_FLOAT, GL_FALSE, 0, nullptr);

			// Set Normals.
			glEnableVertexAttribArray(s_Program->GetAttributeLocation("mn"));
			glBindBuffer(GL_ARRAY_BUFFER, s_Model->NormalBuffer());
			glVertexAttribPointer(s_Program->GetAttributeLocation("mn"), 3, GL_FLOAT, GL_FALSE, 0, nullptr);

			// Set Indices.
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Model->IndexBuffer());

			// Draw all our triangles.
			glDrawElements(GL_TRIANGLES, (GLsizei) s_Model->IndexCount(), GL_UNSIGNED_SHORT, nullptr);

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);

			if (s_TextureLocation != -1 && s_Model->Texture() != nullptr)
				glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}

void MainRenderer::OnResize(int p_Width, int p_Height)
{
	m_ProjectionMatrix = glm::perspective(glm::radians(Managers::CameraManager::GetInstance()->FOV()),
		(float) p_Width / (float) p_Height,
		0.1f, 1000.f);

	m_OrthoProjection = glm::ortho(0.f, (float) p_Width, (float) p_Height, 0.f);
}

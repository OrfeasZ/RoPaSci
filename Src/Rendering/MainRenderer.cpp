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
	FT_Done_Face(m_MainFontFace);
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
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	std::string s_FontData;
	if (!VFS::FileSystem::GetInstance()->ReadFile("/data/fonts/MainFont.ttf", s_FontData))
	{
		Logger(Util::LogLevel::Error, "Could not read main font file.");
		return false;
	}

	if (FT_New_Memory_Face(Application::GetInstance()->GetFreeType(), (const FT_Byte*) s_FontData.data(), s_FontData.size(), 0, &m_MainFontFace))
	{
		Logger(Util::LogLevel::Error, "Could not parse main font face.");
		return false;
	}

	FT_Set_Pixel_Sizes(m_MainFontFace, 0, 120);

	// Create font character glyphs.
	for (uint8_t c = 0; c < 128; ++c)
	{
		if (FT_Load_Char(m_MainFontFace, c, FT_LOAD_RENDER))
		{
			Logger(Util::LogLevel::Warn, "Could not load glyph '%c'.", (char) c);
			continue;
		}

		m_CharacterTextures[c] = Textures::CharacterTexture::Create(m_MainFontFace, c);
	}

	// Text buffers.

	glGenVertexArrays(1, &m_TextVAO);
	glBindVertexArray(m_TextVAO);

	// make and bind the VBO
	glGenBuffers(1, &m_TextVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_TextVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 4, nullptr, GL_STATIC_DRAW);

	glGenBuffers(1, &m_TextEBO);

	GLuint s_TextElements [] = 
	{
		0, 1, 2,
		2, 3, 0
	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_TextEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_TextElements), s_TextElements, GL_STATIC_DRAW);

	auto s_Program = Managers::ShaderManager::GetInstance()->GetShaderProgram("TexturedColored");

	// connect the xyz to the "vert" attribute of the vertex shader
	glEnableVertexAttribArray(s_Program->GetAttributeLocation("v"));
	glVertexAttribPointer(s_Program->GetAttributeLocation("v"), 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	// connect the uv coords to the "vertTexCoord" attribute of the vertex shader
	glEnableVertexAttribArray(s_Program->GetAttributeLocation("tc"));
	glVertexAttribPointer(s_Program->GetAttributeLocation("tc"), 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*) (2 * sizeof(GLfloat)));

	// unbind the VAO
	glBindVertexArray(0);

	/*g_Texture = Managers::TextureManager::GetInstance()->GetTexture("paper");

	glGenVertexArrays(1, &gVAO);
	glBindVertexArray(gVAO);

	// make and bind the VBO
	glGenBuffers(1, &gVBO);
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);

	GLfloat vertexData [] = {
		//  X  Y     U     V
		100, 100, 0.0f, 0.0f, // Top-left
		200, 100, 1.0f, 0.0f, // Top-right
		200, 200, 1.0f, 1.0f, // Bottom-right
		100, 200, 0.0f, 1.0f  // Bottom-left
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);

	GLuint elements [] = {
		0, 1, 2,
		2, 3, 0
	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	s_Program = Managers::ShaderManager::GetInstance()->GetShaderProgram("Textured");

	// connect the xyz to the "vert" attribute of the vertex shader
	glEnableVertexAttribArray(s_Program->GetAttributeLocation("v"));
	glVertexAttribPointer(s_Program->GetAttributeLocation("v"), 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	// connect the uv coords to the "vertTexCoord" attribute of the vertex shader
	glEnableVertexAttribArray(s_Program->GetAttributeLocation("tc"));
	glVertexAttribPointer(s_Program->GetAttributeLocation("tc"), 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*) (2 * sizeof(GLfloat)));

	// unbind the VAO
	glBindVertexArray(0);*/

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

	/*auto s_Program = Managers::ShaderManager::GetInstance()->GetShaderProgram("Textured");

	glUseProgram(s_Program->GetProgram());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_Texture->GetGLTexture());

	glUniform1i(s_Program->GetUniformLocation("t"), 0);
	glUniformMatrix4fv(s_Program->GetUniformLocation("p"), 1, GL_FALSE, &m_OrthoProjection[0][0]);

	glBindVertexArray(gVAO);

	// draw the VAO
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// unbind the VAO, the program and the texture
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);*/
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
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, s_Model->Texture()->GetGLTexture());
				glUniform1i(s_TextureLocation, 0);
			}

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

void MainRenderer::RenderText(const std::string& p_Text, float p_X, float p_Y, float p_Scale, glm::vec3 p_Color)
{
	auto s_Program = Managers::ShaderManager::GetInstance()->GetShaderProgram("TexturedColored");

	glUseProgram(s_Program->GetProgram());

	glUniformMatrix4fv(s_Program->GetUniformLocation("p"), 1, GL_FALSE, &m_OrthoProjection[0][0]);
	glUniform3f(s_Program->GetUniformLocation("c"), p_Color.x, p_Color.y, p_Color.z);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(m_TextVAO);

	for (auto it = p_Text.begin(); it != p_Text.end(); ++it)
	{
		auto s_CharacterTexture = m_CharacterTextures[*it];

		float s_XOffset = p_X + s_CharacterTexture->Bearing().x * p_Scale;
		float s_YOffset = p_Y - s_CharacterTexture->Bearing().y * p_Scale;

		float s_Width = s_CharacterTexture->GetCreateDesc().Width * p_Scale;
		float s_Height = s_CharacterTexture->GetCreateDesc().Height * p_Scale;

		GLfloat s_TextVertices[] = 
		{
			s_XOffset, s_YOffset, 0.0f, 0.0f,
			s_XOffset + s_Width, s_YOffset, 1.0f, 0.0f,
			s_XOffset + s_Width, s_YOffset + s_Height, 1.0f, 1.0f,
			s_XOffset, s_YOffset + s_Height, 0.0f, 1.0f 
		};

		glBindTexture(GL_TEXTURE_2D, s_CharacterTexture->GetGLTexture());

		glBindBuffer(GL_ARRAY_BUFFER, m_TextVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(s_TextVertices), s_TextVertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// draw the VAO
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		p_X += (s_CharacterTexture->AdvanceX() >> 6) * p_Scale;
		p_Y += (s_CharacterTexture->AdvanceY() >> 6) * p_Scale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
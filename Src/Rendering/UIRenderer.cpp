#include "UIRenderer.h"

#include <Application.h>

#include <Managers/ModelManager.h>
#include <Managers/InputManager.h>
#include <Managers/CameraManager.h>
#include <Managers/ShaderManager.h>
#include <Managers/TextureManager.h>

#include <Rendering/MainRenderer.h>
#include <Rendering/Textures/Texture.h>
#include <Rendering/Textures/CharacterTexture.h>

using namespace Rendering;

UIRenderer* UIRenderer::m_Instance = nullptr;

UIRenderer* UIRenderer::GetInstance()
{
	if (!m_Instance)
		m_Instance = new UIRenderer();

	return m_Instance;
}

void UIRenderer::DestroyInstance()
{
	if (!m_Instance)
		return;

	delete m_Instance;
	m_Instance = nullptr;
}

//////////////////////////////////////////////////////////////////////////

UIRenderer::UIRenderer()
{
}

UIRenderer::~UIRenderer()
{
	FT_Done_Face(m_MainFontFace);
}

bool UIRenderer::Init()
{
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

	glEnableVertexAttribArray(s_Program->GetAttributeLocation("v"));
	glVertexAttribPointer(s_Program->GetAttributeLocation("v"), 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	glEnableVertexAttribArray(s_Program->GetAttributeLocation("tc"));
	glVertexAttribPointer(s_Program->GetAttributeLocation("tc"), 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*) (2 * sizeof(GLfloat)));

	glBindVertexArray(0);

	return true;
}

void UIRenderer::Update(double p_Delta)
{

}

void UIRenderer::Render(double p_Delta)
{

}

void UIRenderer::OnResize(int p_Width, int p_Height)
{

}

void UIRenderer::RenderText(const std::string& p_Text, float p_X, float p_Y, float p_Scale, glm::vec3 p_Color)
{
	auto s_Program = Managers::ShaderManager::GetInstance()->GetShaderProgram("TexturedColored");

	glUseProgram(s_Program->GetProgram());

	glUniformMatrix4fv(s_Program->GetUniformLocation("p"), 1, GL_FALSE, &MainRenderer::GetInstance()->OrthoProjection()[0][0]);
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

		GLfloat s_TextVertices [] =
		{
			s_XOffset, s_YOffset, 0.0f, 0.0f,
			s_XOffset + s_Width, s_YOffset, 1.0f, 0.0f,
			s_XOffset + s_Width, s_YOffset + s_Height, 1.0f, 1.0f,
			s_XOffset, s_YOffset + s_Height, 0.0f, 1.0f
		};

		glBindTexture(GL_TEXTURE_2D, s_CharacterTexture->GetGLTexture());

		// Update the vertex buffer.
		glBindBuffer(GL_ARRAY_BUFFER, m_TextVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(s_TextVertices), s_TextVertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Draw.
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		p_X += (s_CharacterTexture->AdvanceX() >> 6) * p_Scale;
		p_Y += (s_CharacterTexture->AdvanceY() >> 6) * p_Scale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void UIRenderer::RenderTexture(Textures::Texture* p_Texture, float p_Top, float p_Left, float p_Bottom, float p_Right)
{
	auto s_Program = Managers::ShaderManager::GetInstance()->GetShaderProgram("Textured");

	glUseProgram(s_Program->GetProgram());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, p_Texture->GetGLTexture());

	GLfloat s_Vertices [] = 
	{
		p_Left, p_Top, 0.0f, 0.0f,
		p_Right, p_Top, 1.0f, 0.0f,
		p_Right, p_Bottom, 1.0f, 1.0f,
		p_Left, p_Bottom, 0.0f, 1.0f
	};

	glBindBuffer(GL_ARRAY_BUFFER, p_Texture->VBO());
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(s_Vertices), s_Vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUniform1i(s_Program->GetUniformLocation("t"), 0);
	glUniformMatrix4fv(s_Program->GetUniformLocation("p"), 1, GL_FALSE, &MainRenderer::GetInstance()->OrthoProjection()[0][0]);

	glBindVertexArray(p_Texture->VAO());

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
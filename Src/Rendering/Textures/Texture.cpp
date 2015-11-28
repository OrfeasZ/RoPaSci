#include "Texture.h"

#include <Managers/ShaderManager.h>

using namespace Rendering::Textures;

Texture::Texture(TextureCreateDesc p_Desc) :
	m_TextureID(0),
	m_Desc(p_Desc),
	m_VAO(0),
	m_VBO(0),
	m_EBO(0)
{
	glGenTextures(1, &m_TextureID);
	glBindTexture(m_Desc.Target, m_TextureID);

	glTexImage2D(m_Desc.Target, m_Desc.Level, m_Desc.InternalFormat, m_Desc.Width, m_Desc.Height, m_Desc.Border, m_Desc.Format, m_Desc.Type, m_Desc.Data);

	glTexParameteri(m_Desc.Target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(m_Desc.Target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(m_Desc.Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(m_Desc.Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glGenerateMipmap(m_Desc.Target);
}

Texture::~Texture()
{

}

void Texture::Update(double p_Delta)
{

}

Texture* Texture::Create(TextureCreateDesc p_Desc, bool p_Buffers)
{
	auto s_Texture = new Texture(p_Desc);

	if (!p_Buffers)
		return s_Texture;

	auto s_Program = Managers::ShaderManager::GetInstance()->GetShaderProgram("Textured");

	glGenVertexArrays(1, &s_Texture->m_VAO);
	glBindVertexArray(s_Texture->m_VAO);

	glGenBuffers(1, &s_Texture->m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, s_Texture->m_VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 4, nullptr, GL_STATIC_DRAW);

	glGenBuffers(1, &s_Texture->m_EBO);

	GLuint s_Elements [] = 
	{
		0, 1, 2,
		2, 3, 0
	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Texture->m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_Elements), s_Elements, GL_STATIC_DRAW);

	glEnableVertexAttribArray(s_Program->GetAttributeLocation("v"));
	glVertexAttribPointer(s_Program->GetAttributeLocation("v"), 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	glEnableVertexAttribArray(s_Program->GetAttributeLocation("tc"));
	glVertexAttribPointer(s_Program->GetAttributeLocation("tc"), 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*) (2 * sizeof(GLfloat)));

	glBindVertexArray(0);
}
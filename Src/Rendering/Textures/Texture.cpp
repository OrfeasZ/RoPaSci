#include "Texture.h"

using namespace Rendering::Textures;

Texture::Texture(TextureCreateDesc p_Desc) :
	m_TextureID(0),
	m_Desc(p_Desc)
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

Texture* Texture::Create(TextureCreateDesc p_Desc)
{
	return new Texture(p_Desc);
}
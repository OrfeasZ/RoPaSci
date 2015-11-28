#include "Texture.h"

using namespace Rendering::Textures;

Texture::Texture() :
	m_TextureID(0)
{

}

Texture::~Texture()
{

}

void Texture::Update(double p_Delta)
{

}

Texture* Texture::Create(TextureCreateDesc p_Desc)
{
	Texture* s_Texture = new Texture();
	s_Texture->m_Desc = p_Desc;

	glGenTextures(1, &s_Texture->m_TextureID);
  
	glBindTexture(p_Desc.Target, s_Texture->m_TextureID);
  
	glTexImage2D(p_Desc.Target, p_Desc.Level, p_Desc.InternalFormat, p_Desc.Width, p_Desc.Height, p_Desc.Border, p_Desc.Format, p_Desc.Type, p_Desc.Data);
 
	glTexParameteri(p_Desc.Target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(p_Desc.Target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(p_Desc.Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(p_Desc.Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glGenerateMipmap(p_Desc.Target);

	return s_Texture;
}
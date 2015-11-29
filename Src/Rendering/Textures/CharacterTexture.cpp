#include "CharacterTexture.h"

using namespace Rendering::Textures;

CharacterTexture::CharacterTexture(TextureCreateDesc p_Desc)
	: Texture(p_Desc)
{
	glTexParameteri(m_Desc.Target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_Desc.Target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

CharacterTexture::~CharacterTexture()
{

}

CharacterTexture* CharacterTexture::Create(FT_Face p_Face, char p_Char)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if (FT_Load_Char(p_Face, p_Char, FT_LOAD_RENDER))
		return nullptr;
	
	TextureCreateDesc s_Desc;

	s_Desc.Target = GL_TEXTURE_2D;
	s_Desc.Level = 0;
	s_Desc.InternalFormat = GL_ALPHA8;
	s_Desc.Width = p_Face->glyph->bitmap.width;
	s_Desc.Height = p_Face->glyph->bitmap.rows;
	s_Desc.Border = 0;
	s_Desc.Format = GL_ALPHA;
	s_Desc.Type = GL_UNSIGNED_BYTE;
	s_Desc.Data = p_Face->glyph->bitmap.buffer;

	auto s_Texture = new CharacterTexture(s_Desc);

	s_Texture->m_AdvanceX = p_Face->glyph->advance.x;
	s_Texture->m_AdvanceY = p_Face->glyph->advance.y;

	s_Texture->m_Bearing = glm::vec2(p_Face->glyph->bitmap_left, p_Face->glyph->bitmap_top);

	return s_Texture;
}

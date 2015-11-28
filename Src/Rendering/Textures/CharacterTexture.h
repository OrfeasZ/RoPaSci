#pragma once

#include "Texture.h"

namespace Rendering
{
	namespace Textures
	{
		class CharacterTexture : public Texture
		{
		public:
			CharacterTexture(TextureCreateDesc p_Desc);
			~CharacterTexture();

		public:
			static CharacterTexture* Create(FT_Face p_Face, char p_Char);

		public:
			inline int32_t AdvanceX() const { return m_AdvanceX; }
			inline int32_t AdvanceY() const { return m_AdvanceY; }

			inline glm::vec2 Bearing() const { return m_Bearing; }

		protected:
			glm::vec2 m_Bearing;
			int m_AdvanceX;
			int m_AdvanceY;
		};
	}
}
#pragma once

#include "ITexture.h"
#include "TextureCreateDesc.h"

namespace Rendering
{
	namespace Textures
	{
		class Texture : public ITexture
		{
		public:
			Texture(TextureCreateDesc p_Desc);
			~Texture();

		public:
			static Texture* Create(TextureCreateDesc p_Desc);

		public:
			virtual GLuint GetGLTexture() override { return m_TextureID; }
			virtual void Update(double p_Delta) override;

		public:
			inline TextureCreateDesc GetCreateDesc() const { return m_Desc; }

		protected:
			GLuint m_TextureID;
			TextureCreateDesc m_Desc;
		};
	}
}
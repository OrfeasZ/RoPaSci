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
			static Texture* Create(TextureCreateDesc p_Desc, bool p_Buffers = false);

		public:
			virtual GLuint GetGLTexture() override { return m_TextureID; }
			virtual void Update(double p_Delta) override;

		public:
			inline TextureCreateDesc GetCreateDesc() const { return m_Desc; }

			inline GLuint VBO() const { return m_VBO; }
			inline GLuint VAO() const { return m_VAO; }
			inline GLuint EBO() const { return m_EBO; }

		protected:
			GLuint m_TextureID;
			TextureCreateDesc m_Desc;

		protected:
			GLuint m_VBO;
			GLuint m_VAO;
			GLuint m_EBO;
		};
	}
}
#pragma once

#include <Misc/StdAfx.h>

#include "IRenderer.h"
#include <unordered_map>

namespace Rendering
{
	namespace Textures
	{
		class CharacterTexture;
		class Texture;
	}

	class UIRenderer : public IRenderer
	{
	public:
		static UIRenderer* GetInstance();
		static void DestroyInstance();

	private:
		static UIRenderer* m_Instance;

	protected:
		UIRenderer();
		~UIRenderer();

	public:
		virtual bool Init() override;
		virtual void Update(double p_Delta) override;
		virtual void Render(double p_Delta) override;
		virtual void OnResize(int p_Width, int p_Height) override;

	public:
		void RenderText(const std::string& p_Text, float p_X, float p_Y, float p_Scale, glm::vec3 p_Color);
		void RenderTexture(Textures::Texture* p_Texture, float p_Top, float p_Left, float p_Bottom, float p_Right);

	protected:
		std::unordered_map<char, Textures::CharacterTexture*> m_CharacterTextures;
		FT_Face m_MainFontFace;
		GLuint m_TextVAO;
		GLuint m_TextVBO;
		GLuint m_TextEBO;
	};
}
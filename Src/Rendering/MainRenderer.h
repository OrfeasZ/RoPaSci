#pragma once

#include <Misc/StdAfx.h>
#include "IRenderer.h"

#include <unordered_map>

namespace Rendering
{
	namespace Textures
	{
		class CharacterTexture;
	}

	class MainRenderer : public IRenderer
	{
	public:
		static MainRenderer* GetInstance();
		static void DestroyInstance();

	private:
		static MainRenderer* m_Instance;

	protected:
		MainRenderer();
		~MainRenderer();

	public:
		virtual bool Init() override;
		virtual void Update(double p_Delta) override;
		virtual void Render(double p_Delta) override;
		virtual void OnResize(int p_Width, int p_Height) override;

	public:
		glm::vec3 ScreenToWorld(int p_X, int p_Y);
		void RenderText(const std::string& p_Text, float p_X, float p_Y, float p_Scale, glm::vec3 p_Color);

	public:
		inline glm::mat4 ProjectionMatrix() const { return m_ProjectionMatrix; }
		inline glm::mat4 ViewMatrix() const { return m_ViewMatrix; }

	protected:
		void RenderModels();

	protected:
		std::unordered_map<char, Textures::CharacterTexture*> m_CharacterTextures;
		FT_Face m_MainFontFace;
		GLuint m_TextVAO;
		GLuint m_TextVBO;
		GLuint m_TextEBO;

	protected:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_OrthoProjection;
		glm::mat4 m_ViewMatrix;
		glm::vec3 m_LightPosition;
	};
}

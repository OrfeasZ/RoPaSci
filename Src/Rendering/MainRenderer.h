#pragma once

#include <Misc/StdAfx.h>
#include "IRenderer.h"

namespace Rendering
{
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

	public:
		inline glm::mat4 ProjectionMatrix() const { return m_ProjectionMatrix; }
		inline glm::mat4 OrthoProjection() const { return m_OrthoProjection; }
		inline glm::mat4 ViewMatrix() const { return m_ViewMatrix; }

	protected:
		void RenderModels();

	protected:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_OrthoProjection;
		glm::mat4 m_ViewMatrix;
		glm::vec3 m_LightPosition;
	};
}

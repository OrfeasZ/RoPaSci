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

	protected:
		void RenderModels();

	protected:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::vec3 m_LightPosition;
	};
}

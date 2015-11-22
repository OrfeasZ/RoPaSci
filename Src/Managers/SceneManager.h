#pragma once

#include <Misc/StdAfx.h>

#include "IManager.h"
#include "TaskManager.h"

#include <Rendering/IRenderer.h>
#include <Util/EngineTimer.h>

#include <vector>

namespace Managers
{
	class SceneManager : public IManager
	{
	public:
		static SceneManager* GetInstance();
		static void DestroyInstance();

	private:
		static SceneManager* m_Instance;

	private:
		SceneManager();
		~SceneManager();

	public:
		virtual bool Init() override;
		void SetMaxFPS(int p_MaxFPS);
		bool RegisterRenderer(Rendering::IRenderer* p_Renderer);

	public:
		void Render();

	protected:
		GLFWwindow* m_Window;
		Util::EngineTimer m_RenderTimer;
		std::vector<Rendering::IRenderer*> m_Renderers;
	};
}
#pragma once

#include <Misc/StdAfx.h>

#include <SFML/Audio.hpp>
#include <queue>

namespace Game
{
	namespace Entities
	{
		class GridEntity;
	}

	class Main
	{
	public:
		enum GameState
		{
			IdleStart,
			Playing,
			GameOver
		};

	public:
		static Main* GetInstance();
		static void DestroyInstance();

	private:
		static Main* m_Instance;

	protected:
		Main();
		~Main();

	public:
		bool Init();
		void Update(double p_Delta);
		void Render(double p_Delta);

	protected:
		Entities::GridEntity* m_GridEntity;
		GameState m_State;
		float m_SelectedMoves;
		sf::Sound m_MainLoopAudio;
	};
}
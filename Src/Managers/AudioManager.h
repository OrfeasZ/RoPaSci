#pragma once

#include <Misc/StdAfx.h>

#include "IManager.h"

#include <SFML/Audio.hpp>
#include <unordered_map>

namespace Managers
{
	class AudioManager : public IManager
	{
	public:
		static AudioManager* GetInstance();
		static void DestroyInstance();

	private:
		static AudioManager* m_Instance;

	public:
		AudioManager();
		~AudioManager();

	public:
		virtual bool Init() override;

	public:
		void Update(double p_Delta);
		bool PrecacheAudio(const std::string& p_Name);
		sf::SoundBuffer* GetAudio(const std::string& p_Name);
		void PlayAudio(const std::string& p_Audio);

	protected:
		std::unordered_map<uint32_t, sf::SoundBuffer*> m_CachedAudio;
		std::vector<sf::Sound*> m_PlayingSounds;
	};
}
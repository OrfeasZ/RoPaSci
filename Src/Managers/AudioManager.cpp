#include "AudioManager.h"

#include <Util/Utils.h>
#include <VFS/FileSystem.h>

using namespace Managers;

AudioManager* AudioManager::m_Instance = nullptr;

AudioManager* AudioManager::GetInstance()
{
	if (!m_Instance)
		m_Instance = new AudioManager();

	return m_Instance;
}

void AudioManager::DestroyInstance()
{
	if (!m_Instance)
		return;

	delete m_Instance;
	m_Instance = nullptr;
}

//////////////////////////////////////////////////////////////////////////

AudioManager::AudioManager()
{
}

AudioManager::~AudioManager()
{
	for (auto s_Sound : m_PlayingSounds)
		delete s_Sound;

	m_PlayingSounds.clear();

	for (auto s_Audio : m_CachedAudio)
		delete s_Audio.second;

	m_CachedAudio.clear();
}

bool AudioManager::Init()
{
	if (!PrecacheAudio("background"))
		return false;

	if (!PrecacheAudio("game_over"))
		return false;

	if (!PrecacheAudio("swap"))
		return false;

	if (!PrecacheAudio("explosion"))
		return false;

	return true;
}

bool AudioManager::PrecacheAudio(const std::string& p_Name)
{
	uint32_t s_NameHash = Util::Utils::StringHash(p_Name);

	auto s_Iterator = m_CachedAudio.find(s_NameHash);

	if (s_Iterator != m_CachedAudio.end())
		return true;

	Logger(Util::LogLevel::Info, "PreCaching Audio file '%s'.", p_Name.c_str());

	std::string s_FullPath = "/data/audio/" + p_Name + ".flac";

	std::string s_AudioData;
	if (!VFS::FileSystem::GetInstance()->ReadFile(s_FullPath, s_AudioData))
	{
		Logger(Util::LogLevel::Warn, "Could not find Audio file '%s'.", p_Name.c_str());
		return false;
	}

	sf::SoundBuffer* s_Buffer = new sf::SoundBuffer();

	if (!s_Buffer->loadFromMemory(s_AudioData.data(), s_AudioData.size()))
	{
		delete s_Buffer;
		Logger(Util::LogLevel::Warn, "Could not load Audio file '%s'.", p_Name.c_str());
		return false;
	}

	m_CachedAudio[s_NameHash] = s_Buffer;
	return true;
}

sf::SoundBuffer* AudioManager::GetAudio(const std::string& p_Name)
{
	auto s_Iterator = m_CachedAudio.find(Util::Utils::StringHash(p_Name));

	if (s_Iterator == m_CachedAudio.end())
		return nullptr;

	return s_Iterator->second;
}

void AudioManager::Update(double p_Delta)
{
	for (auto it = m_PlayingSounds.begin(); it != m_PlayingSounds.end();)
	{ 
		if ((*it)->getStatus() == sf::Sound::Stopped)
		{
			delete *it;
			it = m_PlayingSounds.erase(it);
			continue;;
		}

		++it;
	}
}

void AudioManager::PlayAudio(const std::string& p_Audio)
{
	auto s_Buffer = GetAudio(p_Audio);

	if (!s_Buffer)
		return;

	sf::Sound* s_Sound = new sf::Sound(*s_Buffer);
	m_PlayingSounds.push_back(s_Sound);

	s_Sound->play();
}
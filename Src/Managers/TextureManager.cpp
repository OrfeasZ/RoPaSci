#include "TextureManager.h"

#include <Util/Utils.h>
#include <Application.h>
#include <VFS/FileSystem.h>
#include <Rendering/Textures/Texture.h>

using namespace Managers;

TextureManager* TextureManager::m_Instance = nullptr;

TextureManager* TextureManager::GetInstance()
{
	if (!m_Instance)
		m_Instance = new TextureManager();

	return m_Instance;
}

void TextureManager::DestroyInstance()
{
	if (!m_Instance)
		return;

	delete m_Instance;
	m_Instance = nullptr;
}

//////////////////////////////////////////////////////////////////////////

TextureManager::TextureManager()
{

}

TextureManager::~TextureManager()
{

}

bool TextureManager::Init()
{
	if (!PrecachePGMTexture("rock"))
		return false;

	if (!PrecachePGMTexture("paper"))
		return false;

	if (!PrecachePGMTexture("scissors"))
		return false;

	if (!PrecachePGMTexture("bomb"))
		return false;

	return true;
}

bool TextureManager::PrecachePGMTexture(const std::string& p_Name)
{
	uint32_t s_NameHash = Util::Utils::StringHash(p_Name);

	auto s_Iterator = m_CachedTextures.find(s_NameHash);

	if (s_Iterator != m_CachedTextures.end())
		return true;

	Logger(Util::LogLevel::Info, "PreCaching PGM Texture '%s'.", p_Name.c_str());

	std::string s_FullPath = "/data/pgm/" + p_Name + ".pgm";

	VFS::FSFile* s_File;
	if (!VFS::FileSystem::GetInstance()->OpenFile(s_FullPath, "rb", &s_File))
	{
		Logger(Util::LogLevel::Warn, "Could not find PGM texture file '%s'.", p_Name.c_str());
		return false;
	}

	int s_Width, s_Height;

	std::vector<float> s_PixelValues;
	int s_MaxValue;

	char s_FileType[256];

	if (s_File->Scan("%s", s_FileType) == EOF)
		return false;

	if (strcmp(s_FileType, "P2") == 0)
	{
		// Handle plain PGM.
		ReadPGMWhiteSpace(s_File);

		s_File->Scan("%d", &s_Width);

		ReadPGMWhiteSpace(s_File);
		
		s_File->Scan("%d", &s_Height);

		ReadPGMWhiteSpace(s_File);

		s_File->Scan("%d", &s_MaxValue);

		for (int i = 0; i < s_Width * s_Height; ++i)
		{
			ReadPGMWhiteSpace(s_File);

			int s_PixelValue;
			s_File->Scan("%d", &s_PixelValue);

			s_PixelValues.push_back((float) s_PixelValue / (float) s_MaxValue);
		}
	}
	else if (strcmp(s_FileType, "P5") == 0)
	{
		// Handle binary PGM.
		ReadPGMWhiteSpace(s_File);

		s_File->Scan("%d", &s_Width);

		ReadPGMWhiteSpace(s_File);
		
		s_File->Scan("%d", &s_Height);

		ReadPGMWhiteSpace(s_File);

		s_File->Scan("%d", &s_MaxValue);

		ReadPGMWhiteSpace(s_File);

		// Read binary data.
		for (int i = 0; i < s_Width * s_Height; ++i)
		{
			auto s_PixelData = s_File->Read(s_MaxValue < 256 ? 1 : 2);

			uint16_t s_PixelValue;

			if (s_MaxValue < 256)
			{
				s_PixelValue = (uint8_t) &s_PixelData[0];
			}
			else
			{
				s_PixelValue = (uint16_t) &s_PixelData[0];
				s_PixelValue = (s_PixelValue >> 8) | (s_PixelValue << 8);
			}

			s_PixelValues.push_back((float) s_PixelValue / (float) s_MaxValue);
		}
	}
	else
	{
		Logger(Util::LogLevel::Warn, "Tried loading an unrecognized PGM format '%s' for file '%s'.", s_FileType, p_Name.c_str());
		return false;
	}

	Rendering::Textures::TextureCreateDesc s_Desc;

	s_Desc.Target = GL_TEXTURE_2D;
	s_Desc.Level = 0;
	s_Desc.InternalFormat = GL_ALPHA32F_ARB;
	s_Desc.Width = s_Width;
	s_Desc.Height = s_Height;
	s_Desc.Border = 0;
	s_Desc.Format = GL_ALPHA;
	s_Desc.Type = GL_FLOAT;
	s_Desc.Data = &s_PixelValues[0];

	auto s_Texture = Rendering::Textures::Texture::Create(s_Desc);
	m_CachedTextures[s_NameHash] = s_Texture;

	return true;
}

Rendering::Textures::ITexture* TextureManager::GetTexture(const std::string& p_Name)
{
	auto s_Iterator = m_CachedTextures.find(Util::Utils::StringHash(p_Name));

	if (s_Iterator == m_CachedTextures.end())
		return nullptr;

	return s_Iterator->second;
}

void TextureManager::ReadPGMWhiteSpace(VFS::FSFile* p_File)
{
	auto s_Data = p_File->Read(1);

	while (s_Data == " " ||
		   s_Data == "\t" ||
		   s_Data == "\n" ||
		   s_Data == "\r" ||
		   s_Data == "#")
	{
		// Special handling for comments.
		if (s_Data == "#")
			while (p_File->Read(1) != "\n")
				;
		
		s_Data = p_File->Read(1);
	}

	p_File->Seek(-1, SEEK_CUR);
}
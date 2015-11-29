#include "TextureManager.h"

#include <Util/Utils.h>
#include <Application.h>
#include <VFS/FileSystem.h>
#include <Rendering/Textures/Texture.h>

#include <gli/gli.hpp>

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

	if (!PrecacheDDSTexture("main_ui"))
		return false;

	if (!PrecacheDDSTexture("main_ui_idle"))
		return false;

	if (!PrecacheDDSTexture("main_ui_gameover"))
		return false;

	if (!PrecacheDDSTexture("ui_background"))
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

bool TextureManager::PrecacheDDSTexture(const std::string& p_Name)
{
	uint32_t s_NameHash = Util::Utils::StringHash(p_Name);

	auto s_Iterator = m_CachedTextures.find(s_NameHash);

	if (s_Iterator != m_CachedTextures.end())
		return true;

	Logger(Util::LogLevel::Info, "PreCaching DDS Texture '%s'.", p_Name.c_str());

	std::string s_FullPath = "/data/textures/" + p_Name + ".dds";

	std::string s_TextureData;
	if (!VFS::FileSystem::GetInstance()->ReadFile(s_FullPath, s_TextureData))
	{
		Logger(Util::LogLevel::Warn, "Could not find DDS texture file '%s'.", p_Name.c_str());
		return false;
	}

	gli::texture s_Texture = gli::load(s_TextureData.data(), s_TextureData.size());

	if (s_Texture.empty())
	{
		Logger(Util::LogLevel::Warn, "Could not load DDS texture file '%s'.", p_Name.c_str());
		return false;
	}

	gli::gl s_GL;

	gli::gl::format s_Format = s_GL.translate(s_Texture.format());
	GLenum s_Target = s_GL.translate(s_Texture.target());

	GLuint s_TextureID = 0;

	glGenTextures(1, &s_TextureID);
	glBindTexture(s_Target, s_TextureID);

	glTexParameteri(s_Target, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(s_Target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(s_Texture.levels() - 1));

	glTexParameteri(s_Target, GL_TEXTURE_SWIZZLE_R, s_Format.Swizzle[0]);
	glTexParameteri(s_Target, GL_TEXTURE_SWIZZLE_G, s_Format.Swizzle[1]);
	glTexParameteri(s_Target, GL_TEXTURE_SWIZZLE_B, s_Format.Swizzle[2]);
	glTexParameteri(s_Target, GL_TEXTURE_SWIZZLE_A, s_Format.Swizzle[3]);

	glTexParameteri(s_Target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(s_Target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(s_Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(s_Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glm::tvec3<GLsizei> s_Dimensions(s_Texture.dimensions());
	GLsizei s_FaceTotal = static_cast<GLsizei>(s_Texture.layers() * s_Texture.faces());

	switch (s_Texture.target())
	{
	case gli::TARGET_1D:
		glTexStorage1D(s_Target, static_cast<GLint>(s_Texture.levels()), s_Format.Internal, s_Dimensions.x);
		break;

	case gli::TARGET_1D_ARRAY:
	case gli::TARGET_2D:
	case gli::TARGET_CUBE:
		glTexStorage2D(s_Target, static_cast<GLint>(s_Texture.levels()), s_Format.Internal,
			s_Dimensions.x, s_Texture.target() == gli::TARGET_2D ? s_Dimensions.y : s_FaceTotal);

		break;

	case gli::TARGET_2D_ARRAY:
	case gli::TARGET_3D:
	case gli::TARGET_CUBE_ARRAY:
		glTexStorage3D(s_Target, static_cast<GLint>(s_Texture.levels()), s_Format.Internal,
			s_Dimensions.x, s_Dimensions.y,
			s_Texture.target() == gli::TARGET_3D ? s_Dimensions.z : s_FaceTotal);
		
		break;
	
	default:
		return false;
	}

	for (std::size_t s_Layer = 0; s_Layer < s_Texture.layers(); ++s_Layer)
	{
		for (std::size_t s_Face = 0; s_Face < s_Texture.faces(); ++s_Face)
		{
			for (std::size_t s_Level = 0; s_Level < s_Texture.levels(); ++s_Level)
			{
				GLsizei s_LayerGL = static_cast<GLsizei>(s_Layer);

				glm::tvec3<GLsizei> s_Dimensions(s_Texture.dimensions(s_Level));

				s_Target = gli::is_target_cube(s_Texture.target())
					? static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + s_Face)
					: s_Target;

				switch (s_Texture.target())
				{
				case gli::TARGET_1D:
				{
					if (gli::is_compressed(s_Texture.format()))
					{
						glCompressedTexSubImage1D(
							s_Target, static_cast<GLint>(s_Level), 0, s_Dimensions.x,
							s_Format.Internal, static_cast<GLsizei>(s_Texture.size(s_Level)),
							s_Texture.data(s_Layer, s_Face, s_Level));
					}
					else
					{
						glTexSubImage1D(
							s_Target, static_cast<GLint>(s_Level), 0, s_Dimensions.x,
							s_Format.External, s_Format.Type,
							s_Texture.data(s_Layer, s_Face, s_Level));
					}

					break;
				}

				case gli::TARGET_1D_ARRAY:
				case gli::TARGET_2D:
				case gli::TARGET_CUBE:
				{
					if (gli::is_compressed(s_Texture.format()))
					{
						glCompressedTexSubImage2D(
							s_Target, static_cast<GLint>(s_Level),
							0, 0,
							s_Dimensions.x,
							s_Texture.target() == gli::TARGET_1D_ARRAY ? s_LayerGL : s_Dimensions.y,
							s_Format.Internal, static_cast<GLsizei>(s_Texture.size(s_Level)),
							s_Texture.data(s_Layer, s_Face, s_Level));
					}
					else
					{
						glTexSubImage2D(
							s_Target, static_cast<GLint>(s_Level),
							0, 0,
							s_Dimensions.x,
							s_Texture.target() == gli::TARGET_1D_ARRAY ? s_LayerGL : s_Dimensions.y,
							s_Format.External, s_Format.Type,
							s_Texture.data(s_Layer, s_Face, s_Level));
					}

					break;
				}

				case gli::TARGET_2D_ARRAY:
				case gli::TARGET_3D:
				case gli::TARGET_CUBE_ARRAY:
				{
					if (gli::is_compressed(s_Texture.format()))
					{
						glCompressedTexSubImage3D(
							s_Target, static_cast<GLint>(s_Level),
							0, 0, 0,
							s_Dimensions.x, s_Dimensions.y,
							s_Texture.target() == gli::TARGET_3D ? s_Dimensions.z : s_LayerGL,
							s_Format.Internal, static_cast<GLsizei>(s_Texture.size(s_Level)),
							s_Texture.data(s_Layer, s_Face, s_Level));
					}
					else
					{
						glTexSubImage3D(
							s_Target, static_cast<GLint>(s_Level),
							0, 0, 0,
							s_Dimensions.x, s_Dimensions.y,
							s_Texture.target() == gli::TARGET_3D ? s_Dimensions.z : s_LayerGL,
							s_Format.External, s_Format.Type,
							s_Texture.data(s_Layer, s_Face, s_Level));
					}
					
					break;
				}

				default: 
					return false;
				}
			}
		}
	}

	auto s_RealTexture = new Rendering::Textures::Texture(s_TextureID, s_Dimensions.x, s_Dimensions.y, true);
	m_CachedTextures[s_NameHash] = s_RealTexture;

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
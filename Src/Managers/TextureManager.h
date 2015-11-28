#pragma once

#include <Misc/StdAfx.h>
#include <VFS/FileSystem.h>

#include "IManager.h"

namespace Rendering
{
	namespace Textures
	{
		class Texture;
	}
}

namespace Managers
{
	class TextureManager : public IManager
	{
	public:
		static TextureManager* GetInstance();
		static void DestroyInstance();

	private:
		static TextureManager* m_Instance;

	protected:
		TextureManager();
		~TextureManager();

	public:
		virtual bool Init() override;

	public:
		Rendering::Textures::Texture* LoadPGMTexture(const std::string& p_Name);

	protected:
		void ReadPGMWhiteSpace(VFS::FSFile* p_File);
	};
}
#pragma once

#include <Misc/StdAfx.h>

namespace Rendering
{
	namespace Textures
	{
		struct TextureCreateDesc
		{
			GLenum Target;
			GLint Level;
			GLint InternalFormat;
			GLsizei Width;
			GLsizei Height;
			GLint Border;
			GLenum Format;
			GLenum Type;
			void* Data;
		};
	}
}
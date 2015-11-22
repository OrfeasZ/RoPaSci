#pragma once

#include <Misc/StdAfx.h>

namespace Rendering
{
	namespace Textures
	{
		class ITexture
		{
		public:
			virtual ~ITexture() {}

		public:
			virtual GLuint GetGLTexture() = 0;
			virtual void Update(double p_Delta) = 0;
		};
	}
}
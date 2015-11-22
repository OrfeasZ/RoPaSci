#pragma once

#include <Misc/StdAfx.h>
#include <vector>

namespace Managers
{
	namespace Model
	{
		struct ModelData
		{
			std::vector<glm::vec3> Vertices;
			std::vector<glm::vec2> UVs;
			std::vector<glm::vec3> Normals;
		};
	}
}
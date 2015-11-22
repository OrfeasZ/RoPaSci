#pragma once

#include <Misc/StdAfx.h> 
#include <Util/Utils.h>

#include <unordered_map>

namespace Managers
{
	namespace Shader
	{
		class Program
		{
		public:
			Program(GLuint p_Program) : m_Program(p_Program) {}

		public:
			inline GLuint GetProgram() const { return m_Program; }

		public:
			inline GLint GetUniformLocation(const std::string& p_Name)
			{
				uint32_t s_NameHash = Util::Utils::StringHash(p_Name);
				auto s_Iterator = m_CachedUniforms.find(s_NameHash);

				if (s_Iterator != m_CachedUniforms.end())
					return s_Iterator->second;

				GLint s_Location = glGetUniformLocation(m_Program, p_Name.c_str());
				m_CachedUniforms[s_NameHash] = s_Location;

				return s_Location;
			}

			inline GLint GetAttributeLocation(const std::string& p_Name)
			{
				uint32_t s_NameHash = Util::Utils::StringHash(p_Name);
				auto s_Iterator = m_CachedAttributes.find(s_NameHash);

				if (s_Iterator != m_CachedAttributes.end())
					return s_Iterator->second;

				GLint s_Location = glGetAttribLocation(m_Program, p_Name.c_str());
				m_CachedAttributes[s_NameHash] = s_Location;

				return s_Location;
			}

		private:
			GLuint m_Program;
			std::unordered_map<uint32_t, GLint> m_CachedUniforms;
			std::unordered_map<uint32_t, GLint> m_CachedAttributes;
		};
	}
}
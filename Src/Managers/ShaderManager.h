#pragma once

#include <Misc/StdAfx.h>

#include <GL/glew.h>
#include <unordered_map>

namespace Managers
{
	class ShaderManager
	{
	public:
		enum ShaderTypeFlags : uint8_t
		{
			VertexShader = 1,
			FragmentShader = 2,
			GeometryShader = 4,
			ComputeShader = 8,
			TessellationControlShader = 16,
			TessellationEvaluationShader = 32
		};

	public:
		ShaderManager();
		~ShaderManager();

	public:
		GLuint CreateShaderProgram(const std::string& p_Name, uint8_t p_ShaderTypes);
		GLuint CompileShader(GLenum p_ShaderType, const std::string& p_Name);

		GLuint GetShader(GLenum p_ShaderType, const std::string& p_Name) const;
		GLuint GetShaderProgram(const std::string& p_Name) const;

		inline std::unordered_map<uint32_t, GLuint> GetShaderPrograms() const { return m_ShaderPrograms; }
		inline std::unordered_map<uint32_t, std::unordered_map<GLenum, GLuint>> GetShaders() const { return m_Shaders; }

	protected:
		static std::string GetShaderExtension(GLenum p_ShaderType);
		static std::string GetShaderTypeString(GLenum p_ShaderType);

	protected:
		std::unordered_map<uint32_t, GLuint> m_ShaderPrograms;
		std::unordered_map<uint32_t, std::unordered_map<GLenum, GLuint>> m_Shaders;
	};
}
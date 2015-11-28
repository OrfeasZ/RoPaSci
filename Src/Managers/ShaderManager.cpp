#include "ShaderManager.h"

#include <Util/Utils.h>
#include <VFS/FileSystem.h>

using namespace Managers;

ShaderManager* ShaderManager::m_Instance = nullptr;

ShaderManager* ShaderManager::GetInstance()
{
	if (!m_Instance)
		m_Instance = new ShaderManager();

	return m_Instance;
}

void ShaderManager::DestroyInstance()
{
	if (!m_Instance)
		return;

	delete m_Instance;
	m_Instance = nullptr;
}

//////////////////////////////////////////////////////////////////////////

ShaderManager::ShaderManager()
{

}

ShaderManager::~ShaderManager()
{
	// Delete our programs.
	for (auto s_Program : m_ShaderPrograms)
	{
		glDeleteProgram(s_Program.second->GetProgram());
		delete s_Program.second;
	}

	// Delete our shaders.
	for (auto s_ShaderCollection : m_Shaders)
		for (auto s_Shader : s_ShaderCollection.second)
			glDeleteShader(s_Shader.second);
}

bool ShaderManager::Init()
{
	if (CreateShaderProgram("ShadedModel", VertexShader | FragmentShader) == 0)
		return false;

	if (CreateShaderProgram("TexturedModel", VertexShader | FragmentShader) == 0)
		return false;

	if (CreateShaderProgram("Textured", VertexShader | FragmentShader) == 0)
		return false;

	return true;
}

Shader::Program* ShaderManager::CreateShaderProgram(const std::string& p_Name, uint8_t p_ShaderTypes)
{
	if (p_ShaderTypes == 0)
		return nullptr;

	Logger(Util::LogLevel::Info, "Creating shader program '%s'.", p_Name.c_str());

	std::vector<GLuint> s_Shaders;

	// Compile the individual shaders.
	if (p_ShaderTypes & ShaderTypeFlags::VertexShader)
		s_Shaders.push_back(CompileShader(GL_VERTEX_SHADER, p_Name));

	if (p_ShaderTypes & ShaderTypeFlags::FragmentShader)
		s_Shaders.push_back(CompileShader(GL_FRAGMENT_SHADER, p_Name));

	if (p_ShaderTypes & ShaderTypeFlags::GeometryShader)
		s_Shaders.push_back(CompileShader(GL_GEOMETRY_SHADER, p_Name));

	if (p_ShaderTypes & ShaderTypeFlags::ComputeShader)
		s_Shaders.push_back(CompileShader(GL_COMPUTE_SHADER, p_Name));

	if (p_ShaderTypes & ShaderTypeFlags::TessellationControlShader)
		s_Shaders.push_back(CompileShader(GL_TESS_CONTROL_SHADER, p_Name));

	if (p_ShaderTypes & ShaderTypeFlags::TessellationEvaluationShader)
		s_Shaders.push_back(CompileShader(GL_TESS_EVALUATION_SHADER, p_Name));

	// Create the program and attach the shaders.
	GLuint s_Program = glCreateProgram();

	for (auto s_Shader : s_Shaders)
		if (s_Shader != 0)
			glAttachShader(s_Program, s_Shader);

	// Link the program.
	GLint s_LinkResult;
	glLinkProgram(s_Program);
	glGetProgramiv(s_Program, GL_LINK_STATUS, &s_LinkResult);

	// Was the link successful?
	if (s_LinkResult == GL_FALSE)
	{
		glDeleteProgram(s_Program);

		int s_InfoLogLength = 0;
		glGetProgramiv(s_Program, GL_INFO_LOG_LENGTH, &s_InfoLogLength);

		std::vector<char> s_InfoLog(s_InfoLogLength);
		glGetProgramInfoLog(s_Program, s_InfoLogLength, NULL, &s_InfoLog[0]);

		Logger(Util::LogLevel::Error, "Could not link shader program '%s': %s", p_Name.c_str(), &s_InfoLog[0]);
		return 0;
	}

	// Store the program in our local map.
	Shader::Program* s_ShaderProgram = new Shader::Program(s_Program);
	m_ShaderPrograms[Util::Utils::StringHash(p_Name)] = s_ShaderProgram;

	return s_ShaderProgram;
}

GLuint ShaderManager::GetShader(GLenum p_ShaderType, const std::string& p_Name) const
{
	uint32_t s_ShaderHash = Util::Utils::StringHash(p_Name);

	auto it = m_Shaders.find(s_ShaderHash);

	if (it == m_Shaders.end())
		return 0;

	auto s_ShaderIt = it->second.find(p_ShaderType);
	
	if (s_ShaderIt == it->second.end())
		return 0;

	return s_ShaderIt->second;
}

Shader::Program* ShaderManager::GetShaderProgram(const std::string& p_Name) const
{
	uint32_t s_ShaderHash = Util::Utils::StringHash(p_Name);

	auto it = m_ShaderPrograms.find(s_ShaderHash);

	if (it == m_ShaderPrograms.end())
		return 0;

	return it->second;
}

GLuint ShaderManager::CompileShader(GLenum p_ShaderType, const std::string& p_Name)
{
	Logger(Util::LogLevel::Info, "Compiling %s shader '%s'.", GetShaderTypeString(p_ShaderType).c_str(), p_Name.c_str());

	// Read the shader file.
	std::string s_ShaderData;
	if (!VFS::FileSystem::GetInstance()->ReadFile("/data/shaders/" + p_Name + GetShaderExtension(p_ShaderType), s_ShaderData))
	{ 
		Logger(Util::LogLevel::Error, "Could not find %s shader '%s'.", GetShaderTypeString(p_ShaderType).c_str(), p_Name.c_str());
		return 0;
	}

	const char* s_RawShaderData = s_ShaderData.data();
	GLint s_ShaderSize = (GLint) s_ShaderData.size();

	// Create the shader.
	GLuint s_Shader = glCreateShader(p_ShaderType);

	glShaderSource(s_Shader, 1, &s_RawShaderData, &s_ShaderSize);
	
	GLint s_CompilationResult;
	glCompileShader(s_Shader);
	glGetShaderiv(s_Shader, GL_COMPILE_STATUS, &s_CompilationResult);
	
	// Was the compilation successful?
	if (s_CompilationResult == GL_FALSE)
	{
		glDeleteShader(s_Shader);

		int s_InfoLogLength = 0;
		glGetShaderiv(s_Shader, GL_INFO_LOG_LENGTH, &s_InfoLogLength);

		std::vector<char> s_InfoLog(s_InfoLogLength);
		glGetShaderInfoLog(s_Shader, s_InfoLogLength, NULL, &s_InfoLog[0]);

		Logger(Util::LogLevel::Error, "Could not compile %s shader '%s': %s", GetShaderTypeString(p_ShaderType).c_str(), p_Name.c_str(), &s_InfoLog[0]);
		return 0;
	}

	// Store the shader in our local map.
	uint32_t s_ShaderHash = Util::Utils::StringHash(p_Name);

	if (m_Shaders.find(s_ShaderHash) == m_Shaders.end())
	{
		std::unordered_map<GLenum, GLuint> s_Shaders;
		m_Shaders[s_ShaderHash] = s_Shaders;
	}

	m_Shaders[s_ShaderHash][p_ShaderType] = s_Shader;

	return s_Shader;
}

std::string ShaderManager::GetShaderExtension(GLenum p_ShaderType)
{
	switch (p_ShaderType)
	{
	case GL_VERTEX_SHADER:
		return ".vs";

	case GL_FRAGMENT_SHADER:
		return ".fs";

	case GL_GEOMETRY_SHADER:
		return ".gs";

	case GL_COMPUTE_SHADER:
		return ".cs";

	case GL_TESS_CONTROL_SHADER:
		return ".tcs";

	case GL_TESS_EVALUATION_SHADER:
		return ".tes";

	default:
		return "";
	}
}

std::string ShaderManager::GetShaderTypeString(GLenum p_ShaderType)
{
	switch (p_ShaderType)
	{
	case GL_VERTEX_SHADER:
		return "vertex";

	case GL_FRAGMENT_SHADER:
		return "fragment";

	case GL_GEOMETRY_SHADER:
		return "geometry";

	case GL_COMPUTE_SHADER:
		return "compute";

	case GL_TESS_CONTROL_SHADER:
		return "tessellation control";

	case GL_TESS_EVALUATION_SHADER:
		return "tessellation evaluation";

	default:
		return "";
	}
}
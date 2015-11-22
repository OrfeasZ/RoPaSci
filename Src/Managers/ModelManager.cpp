#include "ModelManager.h"

#include <VFS/FileSystem.h>
#include <Util/Utils.h>
#include <Rendering/Objects/Model.h>

using namespace Managers;

ModelManager* ModelManager::m_Instance = nullptr;

ModelManager* ModelManager::GetInstance()
{
	if (!m_Instance)
		m_Instance = new ModelManager();

	return m_Instance;
}

void ModelManager::DestroyInstance()
{
	if (!m_Instance)
		return;

	delete m_Instance;
	m_Instance = nullptr;
}

//////////////////////////////////////////////////////////////////////////

ModelManager::ModelManager() 
{
}

ModelManager::~ModelManager()
{
	for (auto s_Model : m_CachedModels)
		delete s_Model.second;

	m_CachedModels.clear();
}

bool ModelManager::Init()
{
	return true;
}

bool ModelManager::PreCacheModel(const std::string& p_Name)
{
	uint32_t s_NameHash = Util::Utils::StringHash(p_Name);

	auto s_Iterator = m_CachedModels.find(s_NameHash);

	if (s_Iterator != m_CachedModels.end())
		return true;

	Logger(Util::LogLevel::Info, "PreCaching Model '%s'.", p_Name.c_str());

	std::string s_FullPath = "/data/models/" + p_Name + ".obj";

	VFS::FSFile* s_File;
	if (!VFS::FileSystem::GetInstance()->OpenFile(s_FullPath, "r", &s_File))
	{
		Logger(Util::LogLevel::Warn, "Could not find model file '%s'.", p_Name.c_str());
		return false;
	}

	Model::ModelData* s_ModelData = new Model::ModelData();

	std::vector<uint32_t> s_VertexIndices;
	std::vector<uint32_t> s_UVIndices;
	std::vector<uint32_t> s_NormalIndices;
	
	std::vector<glm::vec3> s_Vertices;
	std::vector<glm::vec2> s_UVs;
	std::vector<glm::vec3> s_Normals;

	while (true)
	{
		char s_LineType[256];

		if (s_File->Scan("%s", s_LineType) == EOF)
			break;

		// Vertex.
		if (strcmp(s_LineType, "v") == 0)
		{
			glm::vec3 s_Vertex;
			s_File->Scan("%f %f %f\n", &s_Vertex.x, &s_Vertex.y, &s_Vertex.z);

			s_Vertices.push_back(s_Vertex);

			continue;
		}

		// UV
		if (strcmp(s_LineType, "vt") == 0)
		{
			glm::vec2 s_UV;
			s_File->Scan("%f %f\n", &s_UV.x, &s_UV.y);

			s_UV.y = -s_UV.y;
			s_UVs.push_back(s_UV);

			continue;
		}

		// Normal
		if (strcmp(s_LineType, "vn") == 0)
		{
			glm::vec3 s_Normal;
			s_File->Scan("%f %f %f\n", &s_Normal.x, &s_Normal.y, &s_Normal.z);

			s_Normals.push_back(s_Normal);

			continue;
		}
		
		// Indices
		if (strcmp(s_LineType, "f") == 0)
		{
			uint32_t s_VertexIndex[3];
			uint32_t s_UVIndex[3];
			uint32_t s_NormalIndex[3];

			int s_Results = s_File->Scan("%d/%d/%d %d/%d/%d %d/%d/%d\n", 
				&s_VertexIndex[0], &s_UVIndex[0], &s_NormalIndex[0],
				&s_VertexIndex[1], &s_UVIndex[1], &s_NormalIndex[1], 
				&s_VertexIndex[2], &s_UVIndex[2], &s_NormalIndex[2]);

			if (s_Results != 9)
			{
				Logger(Util::LogLevel::Error, "Encountered invalid data when parsing OBJ file '%s'.", p_Name.c_str());

				delete s_ModelData;
				return false;
			}

			s_VertexIndices.push_back(s_VertexIndex[0]);
			s_VertexIndices.push_back(s_VertexIndex[1]);
			s_VertexIndices.push_back(s_VertexIndex[2]);

			s_UVIndices.push_back(s_UVIndex[0]);
			s_UVIndices.push_back(s_UVIndex[1]);
			s_UVIndices.push_back(s_UVIndex[2]);

			s_NormalIndices.push_back(s_NormalIndex[0]);
			s_NormalIndices.push_back(s_NormalIndex[1]);
			s_NormalIndices.push_back(s_NormalIndex[2]);

			continue;
		}

		// Unknown data; read till end of line.
		while (s_File->Read(1) != "\n")
			;
	}

	for (unsigned int i = 0; i < s_VertexIndices.size(); i++)
	{
		s_ModelData->Vertices.push_back(s_Vertices[s_VertexIndices[i] - 1]);
		s_ModelData->UVs.push_back(s_UVs[s_UVIndices[i] - 1]);
		s_ModelData->Normals.push_back(s_Normals[s_NormalIndices[i] - 1]);
	}

	m_CachedModels[s_NameHash] = s_ModelData;
	return true;
}

Model::ModelData* ModelManager::GetModelData(const std::string& p_Name) const
{
	auto s_Iterator = m_CachedModels.find(Util::Utils::StringHash(p_Name));

	if (s_Iterator == m_CachedModels.end())
		return nullptr;

	return s_Iterator->second;
}

void ModelManager::RegisterModel(Rendering::Objects::Model* p_Model)
{
	if (!p_Model)
		return;

	if (m_Models.find(p_Model->ShaderProgram()->GetProgram()) == m_Models.end())
		m_Models[p_Model->ShaderProgram()->GetProgram()] = std::unordered_set<Rendering::Objects::Model*>();

	m_Models[p_Model->ShaderProgram()->GetProgram()].insert(p_Model);
}

void ModelManager::RemoveModel(Rendering::Objects::Model* p_Model)
{
	if (!p_Model)
		return;

	auto s_Iterator = m_Models.find(p_Model->ShaderProgram()->GetProgram());

	if (s_Iterator == m_Models.end())
		return;

	s_Iterator->second.erase(p_Model);
}
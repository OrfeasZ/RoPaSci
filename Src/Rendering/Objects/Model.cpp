#include "Model.h"

using namespace Rendering::Objects;

Model::Model(Managers::Model::ModelData* p_Data, GLuint p_ShaderProgram) :
	m_ShaderProgram(p_ShaderProgram),
	m_ModelMatrix(1.0),
	m_Render(true)
{
	ParseModelData(p_Data);
}

Model::~Model()
{

}

void Model::ParseModelData(Managers::Model::ModelData* p_Data)
{
	for (size_t i = 0; i < p_Data->Vertices.size(); i++)
	{
		uint16_t s_Index = ~0;

		for (uint32_t j = 0; j < m_Vertices.size(); ++j)
		{
			if (fabs(p_Data->Vertices[i].x - m_Vertices[j].x) < 0.01f &&
				fabs(p_Data->Vertices[i].y - m_Vertices[j].y) < 0.01f &&
				fabs(p_Data->Vertices[i].z - m_Vertices[j].z) < 0.01f &&
				fabs(p_Data->UVs[i].x - m_UVs[j].x) < 0.01f &&
				fabs(p_Data->UVs[i].y - m_UVs[j].y) < 0.01f &&
				fabs(p_Data->Normals[i].x - m_Normals[j].x) < 0.01f &&
				fabs(p_Data->Normals[i].y - m_Normals[j].y) < 0.01f &&
				fabs(p_Data->Normals[i].z - m_Normals[j].z) < 0.01f)
			{
				s_Index = j;
				break;
			}
		}

		if (s_Index != ~0)
		{
			m_Indices.push_back(s_Index);
			continue;
		}

		m_Vertices.push_back(p_Data->Vertices[i]);
		m_UVs.push_back(p_Data->UVs[i]);
		m_Normals.push_back(p_Data->Normals[i]);

		m_Indices.push_back((uint16_t) m_Vertices.size() - 1);
	}

	// Generate buffers and bind data.
	glGenBuffers(1, &m_VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(glm::vec3), &m_Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_UVBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_UVBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_UVs.size() * sizeof(glm::vec2), &m_UVs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_NormalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_NormalBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_Normals.size() * sizeof(glm::vec3), &m_Normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_ElementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(uint16_t), &m_Indices[0], GL_STATIC_DRAW);
}

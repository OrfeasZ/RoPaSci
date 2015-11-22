#pragma once

#include <Misc/StdAfx.h>

#include <Managers/Model/ModelData.h>

#include <vector>
#include <glm/gtc/matrix_transform.hpp>

namespace Rendering
{
	namespace Objects
	{
		class Model
		{
		public:
			Model(Managers::Model::ModelData* p_Data, GLuint p_ShaderProgram);
			~Model();
			
		public:
			inline void ShouldRender(bool p_Render) { m_Render = p_Render; }
			inline bool ShouldRender() const { return m_Render; }

			inline void Translate(glm::vec3 p_Coordinates) { m_ModelMatrix = glm::translate(m_ModelMatrix, p_Coordinates); }
			inline void Rotate(float p_Angle, glm::vec3 p_Axis) { m_ModelMatrix = glm::rotate(m_ModelMatrix, p_Angle, p_Axis); }
			inline void Scale(glm::vec3 p_Ratio) { m_ModelMatrix = glm::scale(m_ModelMatrix, p_Ratio); }

			inline void Position(glm::vec3 p_Position) { m_ModelMatrix[3] = p_Position[0] + p_Position[1] + p_Position[2] + m_ModelMatrix[3]; }
			inline glm::vec3 Position() const { return glm::vec3(m_ModelMatrix[3]); }

			inline GLuint VertexBuffer() const { return m_VertexBuffer; }
			inline GLuint UVBuffer() const { return m_UVBuffer; }
			inline GLuint NormalBuffer() const { return m_NormalBuffer; }
			inline GLuint ElementBuffer() const { return m_ElementBuffer; }

		protected:
			void ParseModelData(Managers::Model::ModelData* p_Data);

		private:
			GLuint m_ShaderProgram;
			glm::mat4 m_ModelMatrix;
			bool m_Render;

		private:
			std::vector<uint16_t> m_Indices;
			std::vector<glm::vec3> m_Vertices;
			std::vector<glm::vec2> m_UVs;
			std::vector<glm::vec3> m_Normals;

		private:
			GLuint m_VertexBuffer;
			GLuint m_UVBuffer;
			GLuint m_NormalBuffer;
			GLuint m_ElementBuffer;
		};
	}
}
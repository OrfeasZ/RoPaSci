#pragma once

#include <Misc/StdAfx.h>

#include "IManager.h"
#include "Model/ModelData.h"

#include <unordered_map>
#include <unordered_set>

namespace Rendering
{
	namespace Objects
	{
		class Model;
	}
}

namespace Managers
{
	class ModelManager : public IManager
	{
	public:
		static ModelManager* GetInstance();
		static void DestroyInstance();

	private:
		static ModelManager* m_Instance;

	private:
		ModelManager();
		~ModelManager();

	public:
		virtual bool Init() override;
		
	public:
		bool PreCacheModel(const std::string& p_Name);
		Model::ModelData* GetModelData(const std::string& p_Name) const;

		void RegisterModel(Rendering::Objects::Model* p_Model);
		void RemoveModel(Rendering::Objects::Model* p_Model);

	public:
		inline std::unordered_map<GLuint, std::unordered_set<Rendering::Objects::Model*>> GetModels() const { return m_Models; }

	protected:
		std::unordered_map<uint32_t, Model::ModelData*> m_CachedModels;
		std::unordered_map<GLuint, std::unordered_set<Rendering::Objects::Model*>> m_Models;
	};
}
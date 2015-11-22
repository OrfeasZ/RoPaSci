#pragma once

#include <Misc/StdAfx.h>

#include "IManager.h"
#include "Model/ModelData.h"

#include <unordered_map>

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

	protected:
		std::unordered_map<uint32_t, Model::ModelData*> m_CachedModels;
	};
}
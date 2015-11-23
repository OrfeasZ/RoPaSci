#include "CameraManager.h"

using namespace Managers;

CameraManager* CameraManager::m_Instance = nullptr;

CameraManager* CameraManager::GetInstance()
{
	if (!m_Instance)
		m_Instance = new CameraManager();

	return m_Instance;
}

void CameraManager::DestroyInstance()
{
	if (!m_Instance)
		return;

	delete m_Instance;
	m_Instance = nullptr;
}

//////////////////////////////////////////////////////////////////////////

CameraManager::CameraManager()
{
}

CameraManager::~CameraManager()
{
}

bool CameraManager::Init()
{
	m_Dirty = true;
	m_EyePosition = glm::vec3(0, 3, 0.00001);
	m_LookAtPosition = glm::vec3(0, 0, 0);

	return true;
}
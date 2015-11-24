#pragma once

#include <Misc/StdAfx.h>
#include "IManager.h"

namespace Managers
{
	class CameraManager : public IManager
	{
	public:
		static CameraManager* GetInstance();
		static void DestroyInstance();

	private:
		static CameraManager* m_Instance;

	protected:
		CameraManager();
		~CameraManager();

	public:
		virtual bool Init() override;

	public:
		inline void EyePosition(glm::vec3 p_Position) { m_Dirty = m_EyePosition != p_Position; m_EyePosition = p_Position; }
		inline glm::vec3 EyePosition() const { return m_EyePosition; }

		inline void LookAtPosition(glm::vec3 p_Position) { m_Dirty = m_LookAtPosition != p_Position; m_LookAtPosition = p_Position; }
		inline glm::vec3 LookAtPosition() const { return m_LookAtPosition; }

		inline void FOV(float p_FOV) { m_Dirty = p_FOV != m_FOV; m_FOV = p_FOV; }
		inline float FOV() const { return m_FOV; }

		inline void Dirty(bool p_Dirty) { m_Dirty = p_Dirty; }
		inline bool Dirty() const { return m_Dirty; }

	protected:
		bool m_Dirty;
		float m_FOV;
		glm::vec3 m_EyePosition;
		glm::vec3 m_LookAtPosition;
	};
}
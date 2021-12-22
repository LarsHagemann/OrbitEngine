#include "interfaces/rendering/Camera.hpp"
#include "implementation/Common.hpp"
#include "implementation/misc/Logger.hpp"
#include "implementation/backends/impl/EngineImpl.hpp"

namespace orbit
{

	ICamera::ICamera() :
		_transform(std::make_shared<Transform>())
	{
	}

	void ICamera::RecalculateProjectionMatrix()
	{
		ORBIT_INFO_LEVEL(ORBIT_LEVEL_MISC, "Recalculating projection matrix.");

		auto size = Engine::Get()->Window()->GetDimensions();
		_projection = (m_projectionHook)
			? m_projectionHook()
			: Math<float>::Perspective(Math<float>::PIDIV4, static_cast<float>(size.x()) / size.y(), 0.1f, 10.f);
	}

	Matrix4f ICamera::GetViewProjectionMatrix() const
	{
		return GetViewMatrix() * _projection;
	}

	void ICamera::SetProjectionHook(std::function<Matrix4f()>&& hook) 
	{ 
		m_projectionHook = std::move(hook);  
		RecalculateProjectionMatrix();
	}


}
#include "Engine/Rendering/Cameras/BaseCamera.hpp"
#include "Engine/Misc/Math.hpp"
#include "Engine/Misc/Logger.hpp"

namespace orbit
{

	BaseCamera::BaseCamera() :
		_transform(std::make_shared<Transform>())
	{
	}

	void BaseCamera::RecalculateProjectionMatrix(float vFOV, float aspectRatio, float nearZ, float farZ)
	{
		ORBIT_INFO_LEVEL("Recalculating projection matrix.", 8);
		_projection = Math<float>::PerspectiveFovLH(
			vFOV,
			aspectRatio,
			nearZ,
			farZ
		).transpose();
	}

	Matrix4f BaseCamera::GetViewProjectionMatrix() const
	{
		return GetViewMatrix() * _projection;
	}

}
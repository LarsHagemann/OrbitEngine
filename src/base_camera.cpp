#include "base_camera.hpp"
#include "exception.hpp"

namespace orbit
{

	BaseCamera::BaseCamera() :
		_transform(std::make_shared<Transform>())
	{
	}

	void BaseCamera::RecalculateProjectionMatrix(float vFOV, float aspectRatio, float nearZ, float farZ)
	{
		ORBIT_INFO_LEVEL("Recalculating projection matrix.", 5);
		_projection = PerspectiveFovLH(
			vFOV,
			aspectRatio,
			nearZ,
			farZ
		);
	}

	Matrix4f BaseCamera::GetViewProjectionMatrix() const
	{
		return GetViewMatrix() * _projection;
	}

}
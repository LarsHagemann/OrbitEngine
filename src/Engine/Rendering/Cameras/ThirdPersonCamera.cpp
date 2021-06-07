#include "Engine/Rendering/Cameras/ThirdPersonCamera.hpp"

#include <DirectXMath.h>

namespace orbit
{

	Matrix4f ThirdPersonCamera::GetViewMatrix() const
	{
		if (!_target)
			return Matrix4f::Identity();

		auto target = _target->GetCombinedTranslation();

		auto r = Quaternionf(AngleAxisf(_cachedTilt, _target->LocalLeft()));
		auto zoom = r._transformVector(-_target->LocalForward() * _distanceToTarget);

		auto eye = target + zoom;
		auto up = _target->LocalUp();

		_transform->SetTranslation(eye);

		return Math<float>::LookAt(eye, target, up);
	}

	ThirdPersonCamera::ThirdPersonCamera() :
		_distanceToTarget(.05f),
		_distanceMinMax(Vector2f{ .01f, 4.f }),
		_cachedTilt(0.f)
	{
	}

	std::shared_ptr<ThirdPersonCamera> ThirdPersonCamera::Create()
	{
		return std::make_shared<ThirdPersonCamera>();
	}

	void ThirdPersonCamera::SetDistanceMinMax(const Vector2f& dMinMax)
	{
		_distanceMinMax = dMinMax;
		SetDistance(_distanceToTarget);
	}

	void ThirdPersonCamera::SetDistance(float distance)
	{
		_distanceToTarget = std::clamp(
			distance,
			_distanceMinMax.x(),
			_distanceMinMax.y()
		);
	}

	void ThirdPersonCamera::Tilt(float tilt)
	{
		_cachedTilt = std::clamp(
			_cachedTilt + tilt,
			-_tiltMinMax,
			_tiltMinMax
		);
	}

}
#include "Engine/Rendering/Cameras/ThirdPersonCamera.hpp"

namespace orbit
{

	Matrix4f ThirdPersonCamera::GetViewMatrix() const
	{
		auto left = _target->LocalLeft();
		auto forward = _target->LocalForward();
		auto up = _target->LocalUp();

		auto rot = Quaternionf(AngleAxisf(_cachedTilt, left));
		Vector3f zoom = rot._transformVector(-forward * _distanceToTarget);

		auto target = _target->GetCombinedTranslation();
		
		Vector3f offset =
			_targetOffset.x() * forward +
			_targetOffset.y() * up +
			_targetOffset.z() * left;

		auto pos = target + zoom + offset;

		_transform->SetTranslation(pos);
		return Math<float>::LookAt(pos, target + offset, up);
	}

	ThirdPersonCamera::ThirdPersonCamera() :
		_distanceToTarget(4.f),
		_distanceMinMax(Vector2f{ 2.f, 40.f }),
		_cachedTilt(0.f),
		_targetOffset(Vector3f::Zero())
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

	void ThirdPersonCamera::SetTargetOffet(const Vector3f& offset)
	{
		_targetOffset = offset;
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
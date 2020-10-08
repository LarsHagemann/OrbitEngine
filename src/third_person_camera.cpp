#include "third_person_camera.hpp"

namespace orbit
{

	Matrix4f ThirdPersonCamera::GetViewMatrix() const
	{
		auto rot = Quaternionf(AngleAxisf(_cachedTilt, _target->LocalLeft()));
		Vector3f zoom = rot._transformVector(-_target->LocalForward() * _distanceToTarget);

		auto target = _target->GetCombinedTranslation();
		
		Vector3f offset =
			_targetOffset.x() * _target->LocalForward() +
			_targetOffset.y() * _target->LocalUp() +
			_targetOffset.z() * _target->LocalLeft();

		auto pos = target + zoom + offset;
		auto up = _target->LocalUp();

		_transform->SetTranslation(pos);
		return LookAt(pos, target + offset, up);
	}

	ThirdPersonCamera::ThirdPersonCamera() :
		_distanceToTarget(4.f),
		_distanceMinMax(Vector2f{ 2.f, 40.f }),
		_cachedTilt(0.f)
	{
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
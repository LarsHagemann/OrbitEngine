#include "Engine/Rendering/Cameras/ThirdPersonCamera.hpp"

namespace orbit
{

	Matrix4f ThirdPersonCamera::GetViewMatrix() const
	{
		//auto left = _target->LocalLeft();
		//auto forward = _target->LocalForward();
		//auto up = _target->LocalUp();
//
		//auto rot = Quaternionf(AngleAxisf(_cachedTilt, left));
		//Vector3f zoom = rot._transformVector(-forward * _distanceToTarget);
//
		//auto target = _target->GetCombinedTranslation();
		//
		//Vector3f offset =
		//	_targetOffset.x() * forward +
		//	_targetOffset.y() * up +
		//	_targetOffset.z() * left;
//
		//auto pos = target + zoom + offset;
//
		//_transform->SetTranslation(pos);
		//return Math<float>::LookAt(pos, target + offset, up).transpose();

		auto target = _target->GetCombinedTranslation();
		auto forward = _target->LocalForward();
		auto up = _target->LocalUp();
		auto left = _target->LocalLeft();

		auto rot = Quaternionf(AngleAxisf(_cachedTilt, left));
		Vector3f zoom = rot._transformVector(-forward * _distanceToTarget);

		auto eye = target + zoom;

		Matrix4f matrix = Matrix4f::Zero();
		const auto f = (eye - target).normalized();
        const auto r = up.cross(f);
        auto u = f.cross(r);
        const auto s = f.cross(u).normalized();
        u = s.cross(f);
        
        matrix.block<1, 3>(0, 0) = s;
        matrix.block<1, 3>(1, 0) = u;
        matrix.block<1, 3>(2, 0) = -f;
        matrix(0, 3) = -s.dot(eye);
        matrix(1, 3) = -u.dot(eye);
        matrix(2, 3) = f.dot(eye);
        matrix.row(3) << 0, 0, 0, 1;

		return matrix;

		//return Math<float>::LookAt(eye, target, up).transpose();
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
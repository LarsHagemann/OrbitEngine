#pragma once
#include "implementation/Common.hpp"
#include "implementation/misc/Transform.hpp"
#include "interfaces/rendering/Camera.hpp"

namespace orbit
{

	class ThirdPersonCamera : public ICamera
	{
	protected:
		// @member: the object (its location) the camera is looking over
		TransformPtr _target;
		// @member: the distance to the target transform
		float _distanceToTarget;
		// @member: minimum and maximum distance to the target
		Vector2f _distanceMinMax;
		// @member: tilt of the camera
		float _cachedTilt;
		// @member: tilt freedom
		static constexpr float _tiltMinMax = Math<float>::PIDIV4 * 1.1f;
	public:
		ThirdPersonCamera();
		ThirdPersonCamera(const ThirdPersonCamera& other) = default;
		ThirdPersonCamera(ThirdPersonCamera&& other) = default;
		ThirdPersonCamera& operator=(const ThirdPersonCamera& other) = default;
		ThirdPersonCamera& operator=(ThirdPersonCamera&& other) = default;
		// @brief: creates a new Third Person Camera
		static std::shared_ptr<ThirdPersonCamera> Create();
		// @method: sets the target to follow
		void SetTarget(TransformPtr target) { _target = target; }
		// @method: returns the target to follow
		TransformPtr GetTarget() const { return _target; }
		// @method: sets the minimum and maximum distance to the target
		// @param dMinMax: the first entry is the minimum distance and the second entry is the maximum
		//	distance to the target
		void SetDistanceMinMax(const Vector2f& dMinMax);
		// @method: sets the distance to the target (clamped to the min/max distance)
		// @param distance: the new distance to the target
		void SetDistance(float distance);
		// @method: tilts the camera
		// @param tilt: tilt amount in radians
		void Tilt(float tilt);
		// @method: implements the view matrix calculation
		//	for third person cameras
		virtual Matrix4f GetViewMatrix() const override;
	};

}
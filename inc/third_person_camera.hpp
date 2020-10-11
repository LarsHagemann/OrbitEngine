#pragma once
#include "base_camera.hpp"

namespace orbit
{

	class ThirdPersonCamera : public BaseCamera
	{
	protected:
		// @member: the object (its location) the camera is looking over
		TransformPtr _target;
		// @member: the distance to the target transform
		float _distanceToTarget;
		// @member: minimum and maximum distance to the target
		Vector2f _distanceMinMax;
		// @member: offset from the target position
		Vector3f _targetOffset;
		// @member: tilt of the camera
		float _cachedTilt;
		// @member: tilt freedom
		static constexpr float _tiltMinMax = XM_PIDIV4 * 1.1f;
	protected:
		// @method: implements the view matrix calculation
		//	for third person cameras
		virtual Matrix4f GetViewMatrix() const override;
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
		// @method: sets the offset from the target position
		// @param offset: the offset from the target
		void SetTargetOffet(const Vector3f& offset);
		// @method: tilts the camera
		// @param tilt: tilt amount in radians
		void Tilt(float tilt);
	};

}

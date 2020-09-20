#pragma once
#include "transform.hpp"

namespace orbit
{

	class BaseCamera
	{
	protected:
		// @member: transform of the camera (its position and rotation)
		TransformPtr _transform;
		// @member: projection matrix initialized by calling RecalculateProjectionMatrix()
		// @see: BaseCamera::RecalculateProjectionMatrix()
		Matrix4f _projection;
	public:
		BaseCamera();
		BaseCamera(const BaseCamera& other) = default;
		BaseCamera(BaseCamera&& other) = default;
		BaseCamera& operator=(const BaseCamera& other) = default;
		BaseCamera& operator=(BaseCamera&& other) = default;

		// @method: depending on the type of camera you have
		//	(orbit/third person/first person/...) the implementation of this
		//	function differs
		// @return: the camera's view matrix
		virtual Matrix4f GetViewMatrix() const = 0;

		// @method: calculates a projection matrix
		// @param vFOV: the vertical field of view in radians
		// @param aspectRatio: the screens aspect ratio (screenwidth/screenheight)
		// @param nearZ: the distance of the near plane
		// @param farZ: the distance of the far plane
		// @note: for best results (farZ - nearZ) should be kept as low as possible
		void RecalculateProjectionMatrix(float vFOV, float aspectRatio, float nearZ, float farZ);
		// @method: returns the combined view-projection matrix
		Matrix4f GetViewProjectionMatrix() const;
		// @method: returns the projection matrix 
		Matrix4f GetProjectionMatrix() const { return _projection; }
		// @method: returns the camera's transform
		TransformPtr GetTransform() const { return _transform; }
	};

	using CameraPtr = std::shared_ptr<BaseCamera>;

}


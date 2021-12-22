#pragma once
#include "implementation/misc/Transform.hpp"

namespace orbit
{

	class ICamera
	{
	protected:
		// @member: transform of the camera (its position and rotation)
		TransformPtr _transform;
		// @member: projection matrix initialized by calling RecalculateProjectionMatrix()
		// @see: ICamera::RecalculateProjectionMatrix()
		Matrix4f _projection;
		// @member: if you want to use something else, than the default projection matrix, than
		// 	you can set this member via SetProjectionHook();
		std::function<Matrix4f()> m_projectionHook;
	public:
		ICamera();
		ICamera(const ICamera& other) = default;
		ICamera(ICamera&& other) = default;
		ICamera& operator=(const ICamera& other) = default;
		ICamera& operator=(ICamera&& other) = default;

		// @method: depending on the type of camera you have
		//	(orbit/third person/first person/...) the implementation of this
		//	function differs
		// @return: the camera's view matrix
		virtual Matrix4f GetViewMatrix() const = 0;

		// @see: m_projectionHook
		void SetProjectionHook(std::function<Matrix4f()>&& hook);

		// @method: calculates a projection matrix
		// @param vFOV: the vertical field of view in radians
		// @param aspectRatio: the screens aspect ratio (screenwidth/screenheight)
		// @param nearZ: the distance of the near plane
		// @param farZ: the distance of the far plane
		// @note: for best results (farZ - nearZ) should be kept as low as possible
		void RecalculateProjectionMatrix();
		// @method: returns the combined view-projection matrix
		Matrix4f GetViewProjectionMatrix() const;
		// @method: returns the projection matrix 
		Matrix4f GetProjectionMatrix() const { return _projection; }
		// @method: returns the camera's transform
		TransformPtr GetTransform() const { return _transform; }
	};

	using CameraPtr = std::shared_ptr<ICamera>;

}
#pragma once
#include "implementation/Common.hpp"
#include "interfaces/rendering/Camera.hpp"

namespace orbit
{

    class ArcBallCamera : public ICamera
    {
    private:
        // @member: The focus position
        TransformPtr m_target;
        // @member: The distance to the focus position
        float m_distance = 1.f;
        // @member: The minimum distance to the focus position
        float m_distanceMax = 10.f;
        // @member: The maximum distance from the focus position
        float m_distanceMin = 0.1f;
        // @member: Up-Vector
        Vector3f m_upVector = Eigen::Vector3f::UnitZ();
        // @member: Tilt of the camera
        float m_tilt = 0.f;
        // @member: Upper angle limit for the camera tilt
        float m_tiltMax = Math<float>::PIDIV2 * 0.95f;
        // @member: Lower angle limit for the camera tilt
        float m_tiltMin = -Math<float>::PIDIV2 * 0.95f;
        // @member: Rotation around the up-vector
        float m_pan = 0.f;
    public:
        // @return: the camera's view matrix
		virtual Matrix4f GetViewMatrix() const;

        // @method: sets the camera's focus position
        // @param target: the transform of the target position
        void SetTarget(TransformPtr target) { m_target = target; }

        // @method: sets the camera's up-vector
        // @param up: the new up vector
        //  default: (0,0,1)
        void SetUpVector(Vector3f up) { m_upVector = up; }

        // @method: sets the camera's distance to the focus position
        // @param distance: the new distance
        // @see: ArcBallCamera::Zoom(float)
        void SetDistance(float distance);
        // @method: sets the minimum distance to the focus position
        // @param distance: the new minimum distance
        void SetMinimumDistance(float distance) { m_distanceMin = distance; }
        // @method: sets the maximum distance to the focus position
        // @param distance: the new maximum distance
        void SetMaximumDistance(float distance) { m_distanceMax = distance; }

        // @method: sets the camera's distance to the focus position
        // @param distance: the new distance
        // @see: ArcBallCamera::Zoom(float)
        void SetTilt(float tilt);
        // @method: sets the minimum distance to the focus position
        // @param distance: the new minimum distance
        void SetMinimumTilt(float tilt) { m_tiltMin = tilt; }
        // @method: sets the maximum distance to the focus position
        // @param distance: the new maximum distance
        void SetMaximumTilt(float tilt) { m_tiltMax = tilt; }

        // @method: Zooms in or out 
        // @param factor: Values greater than 1 will zoom away from the
        //  focus position and values smaller than 1 will zoom towards
        //  the focus position
        void Zoom(float factor);
        // @method: Pans the camera
        // @param pan: angle in radians for the camera to be panned
        //  around the up-vector.
        void Pan(float pan);
        // @method: Tilts the camera
        // @param tilt: Values greater than 0 will tilt the camera upwards,
        //  values less than 0 will tilt it downwards
        void Tilt(float tilt);
    };

}
#include "implementation/rendering/ArcBallCamera.hpp"

namespace orbit
{

    Matrix4f ArcBallCamera::GetViewMatrix() const
    {
        if (m_target == nullptr)
            return Matrix4f::Identity();

        auto target = m_target->GetCombinedTranslation();
        auto r0 = Quaternionf(AngleAxisf(m_tilt, -m_transform->LocalRight()));
        auto r1 = Quaternionf(AngleAxisf(m_pan, m_upVector));
		auto dir = r0 * r1 * Vector3f::UnitY();
		auto zoom = -dir * m_distance;

		auto eye = target + zoom;

		m_transform->SetTranslation(eye);
        m_transform->SetRotation(r1);

		return Math<float>::LookAt(eye, target, m_upVector);
    }

    void ArcBallCamera::SetDistance(float distance)
    {
        m_distance = std::clamp(
            distance,
            m_distanceMin,
            m_distanceMax
        );
    }

    void ArcBallCamera::SetTilt(float tilt)
    {
        m_tilt = std::clamp(
            tilt,
            m_tiltMin,
            m_tiltMax
        );
    }

    void ArcBallCamera::Zoom(float factor)
    {
        SetDistance(m_distance * factor);
    }

    void ArcBallCamera::Pan(float pan)
    {
        m_pan += pan;
    }
    
    void ArcBallCamera::Tilt(float tilt)
    {
        SetTilt(m_tilt + tilt);
    }

}
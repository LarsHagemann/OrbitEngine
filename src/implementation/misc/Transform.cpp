#include "implementation/misc/Transform.hpp"

namespace orbit
{

	Transform::Transform() :
		m_location(Vector3f::Zero()),
		m_pivot(Vector3f::Zero()),
		m_rotation(Quaternionf::Identity()),
		m_scaling(Vector3f::Ones()),
		m_isDirty(true),
		m_isRotationDirty(true),
		m_cachedRotation(std::make_shared<Matrix3f>())
	{
	}

	void Transform::RecalculateMatrix() const
	{
		m_isDirty = false;
		auto S = Translation3f(m_pivot);
		auto S_ = Translation3f(-m_pivot);

		auto aS = S * Scaling(m_scaling) * S_;
		auto aT = Translation3f(m_location);
		auto aR = S * m_rotation * S_;

		auto a = aT * aR * aS;
		auto m = a.matrix();
		if (m_parent)
			m = m_parent->LocalToWorldMatrix() * m;

		_matrix = m.transpose();
	}

	const Matrix3f& Transform::GetRotation() const
	{
		if (m_isRotationDirty)
		{
			m_isRotationDirty = false;
			*m_cachedRotation = m_rotation.toRotationMatrix();
		}

		return *m_cachedRotation;
	}

	Matrix3f& Transform::GetRotation()
	{
		if (m_isRotationDirty)
		{
			m_isRotationDirty = false;
			*m_cachedRotation = m_rotation.toRotationMatrix();
		}

		return *m_cachedRotation;
	}

	const Matrix4f& Transform::LocalToWorldMatrix() const
	{
		if (m_isDirty) RecalculateMatrix();
		return _matrix;
	}

	Matrix4f Transform::WorldToLocalMatrix() const
	{
		return LocalToWorldMatrix().inverse();
	}

	Matrix3f Transform::GetCombinedRotation() const
	{
		if (m_parent)
			return m_parent->GetCombinedRotation() * GetRotation();
		return GetRotation();
	}

	Vector3f Transform::GetCombinedTranslation() const
	{
		if (m_parent)
			return m_parent->GetCombinedTranslation() + m_location;
		return m_location;
	}

	Vector3f Transform::GetCombinedScaling() const
	{
		if (m_parent)
			return m_scaling.cwiseProduct(m_parent->GetCombinedScaling());
		return m_scaling;
	}

	Vector3f Transform::TransformVector(const Vector3f& vector) const
	{
		return GetCombinedRotation() * vector;
	}

	Vector3f Transform::TransformPoint(const Vector3f& point) const
	{
		auto p4 = Vector4f{ point.x(), point.y(), point.z(), 1.f };
		return (LocalToWorldMatrix() * p4).head<3>();
	}

	Vector3f Transform::InverseTransformVector(const Vector3f& vector) const
	{
		return GetCombinedRotation().inverse() * vector;
	}

	Vector3f Transform::InverseTransformPoint(const Vector3f& point) const
	{
		auto p4 = Vector4f{ point.x(), point.y(), point.z(), 1.f };
		return (WorldToLocalMatrix() * p4).head<3>();
	}

	Vector3f Transform::LocalUp() const
	{
		return GetRotation() * Vector3f{ 0.f, 0.f, 1.f };
	}

	Vector3f Transform::LocalForward() const
	{
		return GetRotation() * Vector3f{ 0.f, 1.f, 0.f };
	}

	Vector3f Transform::LocalRight() const
	{
		return GetRotation() * Vector3f{ 1.f, 0.f, 0.f };
	}

	Vector3f Transform::CombinedUp() const
	{
		return TransformVector(Vector3f{ 0.f, 0.f, 1.f });
	}

	Vector3f Transform::CombinedForward() const
	{
		return TransformVector(Vector3f{ 0.f, 1.f, 0.f });
	}

	Vector3f Transform::CombinedRight() const
	{
		return TransformVector(Vector3f{ 1.f, 0.f, 0.f });
	}

	void Transform::Translate(const Vector3f& translation)
	{
		m_location += translation;
		m_isDirty = true;
	}

	void Transform::Rotate(const Quaternionf& rotation)
	{
		m_rotation = rotation * m_rotation;
		if (fabsf(1.f - m_rotation.squaredNorm()) < 0.01f)
			m_rotation.normalize(); // counter floating point arithmetic errors
		m_isDirty = true;
		m_isRotationDirty = true;
	}

	void Transform::Rotate(const Vector3f& euler)
	{
		Rotate(
			AngleAxisf(euler.x(), Vector3f::UnitZ()) * 
			AngleAxisf(euler.y(), Vector3f::UnitY()) *
			AngleAxisf(euler.z(), Vector3f::UnitZ())
		);
	}

	void Transform::Rotate(float angle, const Vector3f& axis)
	{
		Rotate(Quaternionf(AngleAxisf(angle, axis)));
	}

	void Transform::Scale(const Vector3f& scaling)
	{
		m_scaling = m_scaling.cwiseProduct(scaling);
		m_isDirty = true;
	}

	void Transform::Scale(float uscale)
	{
		Scale(Vector3f{ uscale, uscale, uscale });
	}

	void Transform::SetTranslation(const Vector3f& translation)
	{
		m_location = translation;
		m_isDirty = true;
	}

	void Transform::SetRotation(const Quaternionf& rotation)
	{
		m_rotation = rotation;
		m_isDirty = true;
		m_isRotationDirty = true;
	}

	void Transform::SetRotation(float angle, const Vector3f& axis)
	{
		m_rotation = AngleAxisf(angle, axis);
		m_isDirty = true;
		m_isRotationDirty = true;
	}

	void Transform::SetRotation(const Vector3f& euler)
	{
		m_rotation =
			AngleAxisf(euler.x(), Vector3f::UnitZ()) *
			AngleAxisf(euler.y(), Vector3f::UnitY()) *
			AngleAxisf(euler.z(), Vector3f::UnitX());
		m_isDirty = true;
		m_isRotationDirty = true;
	}

	void Transform::SetScaling(const Vector3f& scaling)
	{
		m_scaling = scaling;
		m_isDirty = true;
	}

	void Transform::SetScaling(float uscaling)
	{
		m_scaling = Vector3f{ uscaling, uscaling, uscaling };
		m_isDirty = true;
	}

	void Transform::SetParent(std::shared_ptr<Transform> parent)
	{
		m_parent = parent;
		m_isDirty = true;
		m_isRotationDirty = true;
	}

	void Transform::SetOrigin(const Vector3f& origin)
	{
		m_pivot = origin;
		m_isDirty = true;
	}

}
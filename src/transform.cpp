#include "transform.hpp"

namespace orbit
{

	void Transform::RecalculateMatrix() const
	{
		_isDirty = false;
		auto aS = Translation3f(_pivot) * Scaling(_scaling) * Translation3f(-_pivot);
		auto aT = Translation3f(_location);
		auto aR = Translation3f(_pivot) * _rotation * Translation3f(-_pivot);

		auto a = aT * aR * aS;
		auto m = a.matrix();
		if (_parent)
			m = _parent->LocalToWorldMatrix() * m;

		_matrix = m.transpose();
	}

	const Matrix4f& Transform::LocalToWorldMatrix() const
	{
		if (_isDirty) RecalculateMatrix();
		return _matrix;
	}

	Matrix4f Transform::WorldToLocalMatrix() const
	{
		return LocalToWorldMatrix().inverse();
	}

	Quaternionf Transform::GetCombinedRotation() const
	{
		if (_parent)
			return _parent->GetCombinedRotation() * _rotation;
		return _rotation;
	}

	Vector3f Transform::GetCombinedTranslation() const
	{
		if (_parent)
			return _parent->GetCombinedTranslation() + _location;
		return _location;
	}

	Vector3f Transform::GetCombinedScaling() const
	{
		if (_parent)
			return _scaling.cwiseProduct(_parent->GetCombinedScaling());
		return _scaling;
	}

	Vector3f Transform::TransformVector(const Vector3f& vector) const
	{
		return GetCombinedRotation()._transformVector(vector);
	}

	Vector3f Transform::TransformPoint(const Vector3f& point) const
	{
		auto p4 = Vector4f{ point.x(), point.y(), point.z(), 1.f };
		return (LocalToWorldMatrix() * p4).head<3>();
	}

	Vector3f Transform::InverseTransformVector(const Vector3f& vector) const
	{
		return GetCombinedRotation().inverse()._transformVector(vector);
	}

	Vector3f Transform::InverseTransformPoint(const Vector3f& point) const
	{
		auto p4 = Vector4f{ point.x(), point.y(), point.z(), 1.f };
		return (WorldToLocalMatrix() * p4).head<3>();
	}

	Vector3f Transform::LocalUp() const
	{
		return TransformVector(Vector3f{ 0.f, 1.f, 0.f });
	}

	Vector3f Transform::LocalForward() const
	{
		return TransformVector(Vector3f{ 1.f, 0.f, 0.f });
	}

	Vector3f Transform::LocalLeft() const
	{
		return TransformVector(Vector3f{ 0.f, 0.f, 1.f });
	}

	void Transform::Translate(const Vector3f& translation)
	{
		_location += translation;
		_isDirty = true;
	}

	void Transform::Rotate(const Quaternionf& rotation)
	{
		_rotation = rotation * _rotation;
		if (fabsf(1.f - _rotation.squaredNorm()) < 0.01f)
			_rotation.normalize(); // counter floating point arithmetic errors
		_isDirty = true;
	}

	void Transform::Rotate(const Vector3f& euler)
	{
		Rotate(
			AngleAxisf(euler.x(), Vector3f::UnitZ()) * 
			AngleAxisf(euler.y(), Vector3f::UnitX()) *
			AngleAxisf(euler.z(), Vector3f::UnitZ())
		);
	}

	void Transform::Rotate(float angle, const Vector3f& axis)
	{
		Rotate(Quaternionf(AngleAxisf(angle, axis)));
	}

	void Transform::Scale(const Vector3f& scaling)
	{
		_scaling = _scaling.cwiseProduct(scaling);
		_isDirty = true;
	}

	void Transform::Scale(float uscale)
	{
		Scale(Vector3f{ uscale, uscale, uscale });
	}

	void Transform::SetTranslation(const Vector3f& translation)
	{
		_location = translation;
		_isDirty = true;
	}

	void Transform::SetRotation(const Quaternionf& rotation)
	{
		_rotation = rotation;
		_isDirty = true;
	}

	void Transform::SetRotation(float angle, const Vector3f& axis)
	{
		_rotation = AngleAxisf(angle, axis);
		_isDirty = true;
	}

	void Transform::SetRotation(const Vector3f& euler)
	{
		_rotation =
			AngleAxisf(euler.x(), Vector3f::UnitZ()) *
			AngleAxisf(euler.y(), Vector3f::UnitX()) *
			AngleAxisf(euler.z(), Vector3f::UnitZ());
		_isDirty = true;
	}

	void Transform::SetScaling(const Vector3f& scaling)
	{
		_scaling = scaling;
		_isDirty = true;
	}

	void Transform::SetScaling(float uscaling)
	{
		_scaling = Vector3f{ uscaling, uscaling, uscaling };
		_isDirty = true;
	}

	void Transform::SetParent(std::shared_ptr<Transform> parent)
	{
		_parent = parent;
		_isDirty = true;
	}

	void Transform::SetOrigin(const Vector3f& origin)
	{
		_pivot = origin;
		_isDirty = true;
	}

}
#pragma once
#include "implementation/Common.hpp"

#include <memory>

namespace orbit
{

    using namespace Eigen;

    // @brief: describes a location, rotation and scaling in the world
	//	and offers useful helper functions
	class Transform
	{
	protected:
		// @member: the location part of the affine transformation
		Vector3f m_location = Vector3f::Zero();
		// @member: pivot point for the scaling and rotation
		Vector3f m_pivot = Vector3f::Zero();
		// @member: the rotation part of the affine transformation
		Quaternionf m_rotation = Quaternionf::Identity();
		// @member: the scaling part of the affine transformation
		Vector3f m_scaling = Vector3f::Ones();
		// @member: parent of the transform
		std::shared_ptr<Transform> m_parent;
		// @member: cached rotation matrix
		mutable std::shared_ptr<Matrix3f> m_cachedRotation;
	private:
		// @member: remembers whether the transformation matrix needs to be
		//	recalculated
		mutable bool m_isDirty;
		mutable bool m_isRotationDirty;
		// @member: cached transformation matrix
		//	contains the affine transformation of
		//	translation,
		//	rotation around the pivot
		//	and scaling around the pivot
		mutable Matrix4f _matrix;
	protected:
		// @method: recalculates the cached matrix
		void RecalculateMatrix() const;
	public:
		Transform();
		// @method: returns a transformation matrix
		// @return: returns the affine transformation from local space to world space
		const Matrix4f& LocalToWorldMatrix() const;
		// @method: returns a transformation matrix
		// @return: returns the affine transformation from world space to local space
		Matrix4f WorldToLocalMatrix() const;
		// @method: returns the parent transformation
		// @return: parent transformation, nullptr if not available
		std::shared_ptr<Transform> GetParent() const { return m_parent; }

		// @method: returns the rotation part of the affine transformation
		const Matrix3f& GetRotation() const;
		Matrix3f& GetRotation();
		// @method: returns the rotation of this transform and all of its parents
		Matrix3f GetCombinedRotation() const;
		// @method: returns the translation part of the affine transformation
		const Vector3f& GetTranslation() const { return m_location; }
		Vector3f& GetTranslation() { return m_location; }
		// @method: returns the translation of this transform and all of its parents
		Vector3f GetCombinedTranslation() const;
		// @method: returns the scaling part of the affine transformation
		const Vector3f& GetScaling() const { return m_scaling; }
		Vector3f& GetScaling() { return m_scaling; }
		// @method: returns the scaling of this transform and all of its parents
		Vector3f GetCombinedScaling() const;
		// @method: returns the scaling and rotation origin
		const Vector3f& GetOrigin() const { return m_pivot; }
		Vector3f& GetOrigin() { return m_pivot; }

		// @method: transforms a vector
		// @param vector: the vector to be transformed
		// @return: the transformed vector
		Vector3f TransformVector(const Vector3f& vector) const;
		// @method: transforms a point
		// @param point: the point to be transformed
		// @return: the transformed point
		Vector3f TransformPoint(const Vector3f& point) const;
		// @method: inverse transformation of a vector
		// @param vector: the vector to be transformed
		// @return: the transformed vector
		Vector3f InverseTransformVector(const Vector3f& vector) const;
		// @method: inverse transformation of a point
		// @param point: the point to be transformed
		// @return: the transformed point
		Vector3f InverseTransformPoint(const Vector3f& point) const;

		// @method: returns the local (in this model's space) up direction
		Vector3f LocalUp() const;
		// @method: returns the local (in this model's space) forward direction
		Vector3f LocalForward() const;
		// @method: returns the local (in this model's space) left direction
		Vector3f LocalRight() const;

		// @method: returns the local up direction of this transform and
		// 	all of its parents (combined up)
		Vector3f CombinedUp() const;
		// @method: returns the local forward direction of this transform and
		// 	all of its parents (combined forward)
		Vector3f CombinedForward() const;
		// @method: returns the local left direction of this transform and
		// 	all of its parents (combined left)
		Vector3f CombinedRight() const;

		// @method: translates the transform
		// @param translation: amount to translate
		void Translate(const Vector3f& translation);
		// @method: rotates the transform
		// @param rotation: rotation quaternion to be applied
		void Rotate(const Quaternionf& rotation);
		// @method: rotates the transform by euler angles
		// @param euler: euler angles for the x-,y- and z-Axis
		void Rotate(const Vector3f& euler);
		// @method: rotates the transform by a certain angle around a certain axis
		// @param angleAxis: the angle and axis to rotate around
		void Rotate(float angle, const Vector3f& axis);
		// @method: scales the transform
		// @param scaling: scaling to be applied
		void Scale(const Vector3f& scaling);
		// @method: scales the transform by a uniform value
		// @param uscale: uniform scaling value to be applied to all 3 axis
		void Scale(float uscale);

		// @method: sets (overwrites) the translation
		// @param translation: the new translation of the transform
		void SetTranslation(const Vector3f& translation);
		// @method: sets (overwrites) the rotation
		// @param rotation: the new rotation of the transform
		void SetRotation(const Quaternionf& rotation);
		// @method: sets (overwrites) the rotation
		// @param angle: angle of the new rotation
		// @param axis: axis around which to rotate
		void SetRotation(float angle, const Vector3f& axis);
		// @method: sets (overwrites) the rotation
		// @param euler: the new rotation angles
		void SetRotation(const Vector3f& euler);
		// @method: sets (overwrites) the scaling
		// @param scaling: the new scaling of the transform
		void SetScaling(const Vector3f& scaling);
		// @method: sets (overwrites) the scaling to a uniform value
		// @param uscaling: the new uniform scaling value
		void SetScaling(float uscaling);
		// @method: sets (overwrites) the transform's parent
		// @param parent: the new parent transform
		void SetParent(std::shared_ptr<Transform> parent);
		// @method: sets (overwrites) the scaling and rotation origin
		// @param origin: the new rotation and scaling origin
		void SetOrigin(const Vector3f& origin);
	};

	using TransformPtr = std::shared_ptr<Transform>;

}
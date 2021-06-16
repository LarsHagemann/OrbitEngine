#pragma once 
#include "Eigen/Dense"

#include <type_traits>

#include <foundation/PxVec2.h>
#include <foundation/PxVec3.h>
#include <foundation/PxVec4.h>
#include <foundation/PxQuat.h>
#include <characterkinematic/PxExtended.h>

#if defined (ORBIT_DX11) || defined (ORBIT_DX12)
#include <DirectXMath.h>
#endif

namespace orbit
{

    using namespace physx;
    using namespace Eigen;

    template<typename T = float>
    struct Math
    {
        static constexpr T PI = static_cast<T>(3.141'592'653'589'793'238'462L);
        static constexpr T PIDIV4 = PI / 4;
        static constexpr T PIDIV2 = PI / 2;
        static constexpr T _2PI = PI * 2;
        static constexpr T AlignUp(const T& in, const T& alignment)
        {
            static_assert(std::is_integral<T>::value && "Math<T>::AlignUp() T must be integral type.");
            return (in + alignment - 1) & ~(alignment - 1);
        }
        static constexpr T AlignDown(const T& in, const T& alignment)
        {
            static_assert(std::is_integral<T>::value && "Math<T>::AlignDown() T must be integral type.");
            return in & ~(alignment - 1);
        }
        static constexpr Matrix<T, 4, 4> PerspectiveFovLH(
            T vFOV,
            T aspectRatio,
            T nearZ,
            T farZ);
        static constexpr Matrix<T, 4, 4> OrthographicProjection(
            T viewWidth,
            T viewHeight,
            T nearZ,
            T farZ
        );
        static constexpr Matrix<T, 4, 4> LookAt(
            const Matrix<T, 3, 1>& eye,
            const Matrix<T, 3, 1>& target,
            const Matrix<T, 3, 1>& up);

        static Vector3f PxToEigen(PxVec3 vec)
        {
            return Vector3f{
                vec.x,
                vec.y,
                vec.z
            };
        } 
        static Vector4f PxToEigen(PxVec4 vec)
        {
            return Vector4f{
                vec.x,
                vec.y,
                vec.z,
                vec.w
            };
        }
        static Vector3f PxToEigen(PxExtendedVec3 vec)
        {
            return Vector3f{
                static_cast<float>(vec.x),
                static_cast<float>(vec.y),
                static_cast<float>(vec.z)
            };
        } 
        static Quaternionf PxToEigen(PxQuat quat)
        {
            return Quaternionf(AngleAxisf(quat.getAngle(), PxToEigen(quat.getBasisVector0())));
        }
        static PxVec2 EigenToPx2(Vector<T, 2> vec)
        {
            return PxVec2{
                static_cast<float>(vec.x()),
                static_cast<float>(vec.y())
            };
        }
        static PxVec3 EigenToPx3(Vector<T, 3> vec)
        {
            return PxVec3{
                static_cast<float>(vec.x()),
                static_cast<float>(vec.y()),
                static_cast<float>(vec.z())
            };
        }
        static PxVec4 EigenToPx4(Vector<T, 4> vec)
        {
            return PxVec4{
                static_cast<float>(vec.x()),
                static_cast<float>(vec.y()),
                static_cast<float>(vec.z()),
                static_cast<float>(vec.w())
            };
        }
        static PxQuat EigenToPx(Quaternionf quat)
        {
            return PxQuat{
                quat.x(),
                quat.y(),
                quat.z(),
                quat.w()
            };
        }

        static Vector<T, 3> Lerp(const Vector<T, 3>& in0, const Vector<T, 3>& in1, float delta)
        {
            return Vector<T, 3>{
                in0.x() * (1 - delta) + delta * in1.x(),
                in0.y() * (1 - delta) + delta * in1.y(),
                in0.z() * (1 - delta) + delta * in1.z()
            };
        }

#if defined (ORBIT_DX11) || defined (ORBIT_DX12)
        static Matrix<T, 4, 4> XMToEigen(DirectX::XMMATRIX matrix)
        {
            // Faster way to initialize Eigen matrix from
            // XMMATRIX?
            Matrix<T, 4, 4> result;
            result << 
                matrix.r[0].m128_f32[0], matrix.r[0].m128_f32[1], matrix.r[0].m128_f32[2],matrix.r[0].m128_f32[3],
                matrix.r[1].m128_f32[0], matrix.r[1].m128_f32[1], matrix.r[1].m128_f32[2],matrix.r[1].m128_f32[3],
                matrix.r[2].m128_f32[0], matrix.r[2].m128_f32[1], matrix.r[2].m128_f32[2],matrix.r[2].m128_f32[3],
                matrix.r[3].m128_f32[0], matrix.r[3].m128_f32[1], matrix.r[3].m128_f32[2],matrix.r[3].m128_f32[3];
            return result;
        }

        static DirectX::XMVECTOR EigenToXM3(Vector<T, 3> vector)
        {
            DirectX::XMFLOAT3 intermediate = { vector.x(), vector.y(), vector.z() };
            return DirectX::XMLoadFloat3(&intermediate);
        }
#endif
    };
} // namespace orbit

#include "Math.inl"

#include "Math.hpp"

namespace orbit
{

    template<typename T>
    constexpr Matrix<T, 4, 4> Math<T>::LookAt(
                const Matrix<T, 3, 1>& eye,
                const Matrix<T, 3, 1>& target,
                const Matrix<T, 3, 1>& up)
    {
#if defined (ORBIT_DX11) || defined (ORBIT_DX12)
        auto result = DirectX::XMMatrixLookAtLH(EigenToXM3(eye), EigenToXM3(target), EigenToXM3(up));
        return Math<T>::XMToEigen(result);
#else
        auto zaxis = (target - eye).normalized();
        auto xaxis = up.cross(zaxis).normalized();
        auto yaxis = zaxis.cross(xaxis);
        
        Matrix<T,4,4> matrix = Matrix<T, 4, 4>::Zero();
        matrix.block<3, 1>(0, 0) = xaxis;
        matrix.block<3, 1>(0, 1) = yaxis;
        matrix.block<3, 1>(0, 2) = zaxis;
        matrix(3, 0) = -xaxis.dot(eye);
        matrix(3, 1) = -yaxis.dot(eye);
        matrix(3, 2) = -zaxis.dot(eye);
        matrix(3, 3) = 1;

        return matrix;
#endif
    }

    template<typename T>
    constexpr Matrix<T, 4, 4> Math<T>::PerspectiveFovLH(
        T vFOV,
        T aspectRatio,
        T nearZ,
        T farZ)
    {
#if defined (ORBIT_DX11) || defined (ORBIT_DX12)
        auto result = DirectX::XMMatrixPerspectiveFovLH(vFOV, aspectRatio, nearZ, farZ);
        return Math<T>::XMToEigen(result);
#else
        Matrix<T, 4, 4> matrix = Matrix<T, 4, 4>::Identity();
        float theta = vFOV * 0.5f;
        float range = farZ - nearZ;
        float invtan = 1.f / tan(theta);

        matrix(0, 0) = invtan / aspectRatio;
        matrix(1, 1) = invtan;
        matrix(2, 2) = -(nearZ + farZ) / range;
        matrix(3, 2) = -1;
        matrix(2, 3) = -2 * nearZ * farZ / range;
        matrix(3, 3) = 0;

        return matrix;
#endif
    }

    template<typename T>
    constexpr Matrix<T, 4, 4> Math<T>::OrthographicProjection(
        T viewWidth,
        T viewHeight,
        T nearZ,
        T farZ
    )
    {
#if defined (ORBIT_DX11) || defined (ORBIT_DX12)
        auto result = DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ);
        return Math<T>::XMToEigen(result);
#else
        Matrix<T, 4, 4> matrix = Matrix<T, 4, 4>::Zero();

        return matrix;
#endif
    }

}
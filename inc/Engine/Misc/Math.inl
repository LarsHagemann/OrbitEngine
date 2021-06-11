#include "Math.hpp"

namespace orbit
{

    template<typename T>
    constexpr Matrix<T, 4, 4> Math<T>::LookAt(
                const Matrix<T, 3, 1>& eye,
                const Matrix<T, 3, 1>& target,
                const Matrix<T, 3, 1>& up)
    {
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
    }

    template<typename T>
    constexpr Matrix<T, 4, 4> Math<T>::PerspectiveFovLH(
        T vFOV,
        T aspectRatio,
        T nearZ,
        T farZ)
    {
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
    }

    template<typename T>
    constexpr Matrix<T, 4, 4> Math<T>::OrthogonalProjection(
        T vFOV,
        T aspectRatio,
        T nearZ,
        T farZ
    )
    {
        Matrix<T, 4, 4> matrix = Matrix<T, 4, 4>::Zero();

        return matrix;
    }

}
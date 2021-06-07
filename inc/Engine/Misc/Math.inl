#include "Math.hpp"

namespace orbit
{

    template<typename T>
    constexpr Matrix<T, 4, 4> Math<T>::LookAt(
                const Matrix<T, 3, 1>& eye,
                const Matrix<T, 3, 1>& target,
                const Matrix<T, 3, 1>& up)
    {
        //const auto f = (eye - target).normalized();
        //const auto r = up.cross(f);
        //auto u = f.cross(r);
        //const auto s = f.cross(u).normalized();
        //u = s.cross(f);
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
        
        //matrix.block<1, 3>(0, 0) = s;
        //matrix.block<1, 3>(1, 0) = u;
        //matrix.block<1, 3>(2, 0) = -f;
        //matrix(0, 3) = -s.dot(eye);
        //matrix(1, 3) = -u.dot(eye);
        //matrix(2, 3) = f.dot(eye);
        //matrix.row(3) << 0, 0, 0, 1;

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

        matrix(0, 0) = 2 / (right - left);
        matrix(1, 1) = 2 / (top - bottom);
        matrix(2, 2) = -2 / (far - near);
        matrix(3, 2) = (far + near) / (far - near);
        matrix(3, 0) = (right + left) / (right - left);
        matrix(3, 1) = (top + bottom) / (top - bottom);
        matrix(3, 3) = 1;

        return matrix;
    }

}
#include "Math.hpp"

namespace orbit
{

    template<typename T>
    constexpr Matrix<T, 4, 4> Math<T>::LookAt(
                const Matrix<T, 3, 1>& eye,
                const Matrix<T, 3, 1>& target,
                const Matrix<T, 3, 1>& up)
    {
        const auto f = (eye - target).normalized();
        const auto r = up.cross(f);
        auto u = f.cross(r);
        const auto s = f.cross(u).normalized();
        u = s.cross(f);
        
        Matrix<T,4,4> matrix = Matrix<T, 4, 4>::Zero();
        
        matrix.block<1, 3>(0, 0) = s;
        matrix.block<1, 3>(1, 0) = u;
        matrix.block<1, 3>(2, 0) = -f;
        matrix(0, 3) = -s.dot(eye);
        matrix(1, 3) = -u.dot(eye);
        matrix(2, 3) = f.dot(eye);
        matrix.row(3) << 0, 0, 0, 1;

        return matrix;
    }

    template<typename T>
    constexpr Matrix<T, 4, 4> Math<T>::PerspectiveFovLH(
        T vFOV,
        T aspectRatio,
        T nearZ,
        T farZ)
    {
        Matrix<T,4,4> matrix = Matrix<T, 4, 4>::Zero();

        const auto theta = vFOV * static_cast<T>(0.5);
        const auto range = farZ - nearZ;
        const auto invtan = static_cast<T>(1.) / tan(theta);
        const auto tmp = (farZ) / range;
        
        matrix(0, 0) = invtan / aspectRatio;
        matrix(1, 1) = invtan;
        matrix(2, 2) = 2 * tmp;
        matrix(2, 3) = 1;
        matrix(3, 2) = -nearZ * tmp;

        return matrix;
    }

}
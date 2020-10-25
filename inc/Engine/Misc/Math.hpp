#pragma once 
#include <type_traits>
#include "Eigen/Dense"

namespace orbit
{

    using namespace Eigen;

    template<typename T>
    struct Math
    {
        static constexpr T PI = static_cast<T>(3.141'592'653'589'793'238'462l);
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
        static constexpr Matrix<T, 4, 4> LookAt(
            const Matrix<T, 3, 1>& eye,
            const Matrix<T, 3, 1>& target,
            const Matrix<T, 3, 1>& up);     
    };

}

#include "Math.inl"

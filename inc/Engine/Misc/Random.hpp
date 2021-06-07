#pragma once
#include <random>
#include "Eigen/Dense"

namespace orbit
{

    using namespace Eigen;

    // Helper class for random numbers
    // Provides useful functions
    class Random
    {
    private:
        static std::random_device m_device;
        static std::mt19937_64 m_engine;
        static std::uniform_real_distribution<double> udist01;
        static std::normal_distribution<double> ndist01;
        static std::uniform_real_distribution<double> udist11;
        static std::normal_distribution<double> ndist11;
    public:
        template<typename T, class Distribution>
        static T Number(Distribution dist)
        {
            return static_cast<T>(dist(m_engine));
        }
        template<typename T>
        static T UniformNumber(T min, T max)
        {
            std::uniform_real_distribution<double> dist(
                static_cast<double>(min), 
                static_cast<double>(max)
            );
            return static_cast<T>(dist(m_engine));
        }
        template<typename T>
        static T UniformNumber01()
        {
            return static_cast<T>(udist01(m_engine));
        }
        template<typename T>
        static T NormalNumber01()
        {
            return static_cast<T>(ndist01(m_engine));
        }
        template<typename T>
        static T UniformNumberNegative11()
        {
            return static_cast<T>(udist11(m_engine));
        }
        template<typename T>
        static T NormalNumberNegative11()
        {
            return static_cast<T>(ndist11(m_engine));
        }
        template<typename T>
        static Vector<T, 2> OnUnitCircle()
        {
            return Vector<T, 2>{
                static_cast<T>(UniformNumberNegative11()),
                static_cast<T>(UniformNumberNegative11())
            }.normalize();
        }
        template<typename T>
        static Vector<T, 3> OnUnitSphere()
        {
            return Vector<T, 3>{
                static_cast<T>(UniformNumberNegative11()),
                static_cast<T>(UniformNumberNegative11()),
                static_cast<T>(UniformNumberNegative11())
            }.normalize();
        }
    };

}

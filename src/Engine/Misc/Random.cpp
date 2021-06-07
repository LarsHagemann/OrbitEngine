#include "Engine/Misc/Random.hpp"

namespace orbit
{

    std::random_device Random::m_device;
    std::mt19937_64 Random::m_engine = std::mt19937_64(Random::m_device());
    std::uniform_real_distribution<double> Random::udist01 = std::uniform_real_distribution<double>(0., 1.);
    std::normal_distribution<double> Random::ndist01 = std::normal_distribution<double>(0., 1.);
    std::uniform_real_distribution<double> Random::udist11 = std::uniform_real_distribution<double>(-1., 1.);
    std::normal_distribution<double> Random::ndist11 = std::normal_distribution<double>(-1., 1.);

}
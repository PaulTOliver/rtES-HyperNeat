#ifndef __HYPERNEAT_RANDOM_HPP__
#define __HYPERNEAT_RANDOM_HPP__

#include <random>

namespace hyperneat
{
    using RandGen  = std::mt19937_64;
    using IntDist  = std::uniform_int_distribution<size_t>;
    using RealDist = std::uniform_real_distribution<double>;
    using BellDist = std::normal_distribution<double>;
}

#endif

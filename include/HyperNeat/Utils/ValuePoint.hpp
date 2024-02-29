#ifndef __HYPERNEAT_VALUEPOINT_HPP__
#define __HYPERNEAT_VALUEPOINT_HPP__

#include <HyperNeat/Utils/Point.hpp>

namespace hyperneat
{
    class ValuePoint : public Point
    {
    public:
        ValuePoint() = default;
        ValuePoint(double x, double y, double value, double segment);

        double _value   = 0.0;
        double _segment = 0.0;
    };
}

#endif

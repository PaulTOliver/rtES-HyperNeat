#include <HyperNeat/Utils/ValuePoint.hpp>

using namespace hyperneat;

ValuePoint::ValuePoint(double x, double y, double value, double segment)
    : Point(x, y), _value(value), _segment(segment)
{}

#include <cmath>
#include <HyperNeat/Utils/Point.hpp>

using namespace std;
using namespace hyperneat;

Point::Point(double x, double y)
    : _x(x), _y(y)
{}

double
Point::distance(const Point& other) const
{
    double x = _x - other._x;
    double y = _y - other._y;

    return sqrt(x * x + y * y);
}

bool
Point::operator==(const Point& other) const{
    return (_x == other._x) && (_y == other._y);
}

bool
Point::operator<(const Point& other) const
{
    if (_x == other._x) {
        return _y < other._y;
    } else {
        return _x < other._x;
    }
}

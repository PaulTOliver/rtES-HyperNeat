#ifndef __HYPERNEAT_POINT_HPP__
#define __HYPERNEAT_POINT_HPP__

namespace hyperneat
{
    class Point
    {
    public:
        Point() = default;
        Point(double x, double y);

        double distance(const Point& other) const;
        bool operator== (const Point& other) const;
        bool operator< (const Point& other) const;

        double _x = 0.0;
        double _y = 0.0;
    };
}

#endif

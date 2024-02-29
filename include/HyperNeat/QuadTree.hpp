#ifndef __HYPERNEAT_QUADTREE_HPP__
#define __HYPERNEAT_QUADTREE_HPP__

#include <HyperNeat/Utils/Vector.hpp>
#include <HyperNeat/Utils/Function.hpp>

namespace hyperneat
{
    class QuadTree
    {
    public:
        QuadTree() = default;
        QuadTree(double segment, double x, double y);

        double getSegment() const;
        double getX() const;
        double getY() const;

        void subdivide(Function<bool(QuadTree*)> subdivider);
        void traverse(Function<void(const QuadTree*)> traverser) const;

    private:
        Vector<QuadTree> _children;
        double           _segment = 0.0;
        double           _x       = 0.0;
        double           _y       = 0.0;
    };
}

#endif

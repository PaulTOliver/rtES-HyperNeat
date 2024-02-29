#include <HyperNeat/QuadTree.hpp>

using namespace std;
using namespace hyperneat;

QuadTree::QuadTree(double segment, double x, double y)
    : _segment(segment), _x(x), _y(y)
{}

double
QuadTree::getSegment() const
{
    return _segment;
}

double
QuadTree::getX() const
{
    return _x;
}

double
QuadTree::getY() const
{
    return _y;
}

void
QuadTree::subdivide(Function<bool(QuadTree*)> subdivider)
{
    if (subdivider(this)) {
        double newSeg = _segment / 2.0;
        _children.resize(4);
        _children[0] = {newSeg, _x - newSeg, _y - newSeg};
        _children[1] = {newSeg, _x + newSeg, _y - newSeg};
        _children[2] = {newSeg, _x - newSeg, _y + newSeg};
        _children[3] = {newSeg, _x + newSeg, _y + newSeg};

        for (auto &i : _children) {
            i.subdivide(subdivider);
        }
    }
}

void
QuadTree::traverse(Function<void(const QuadTree*)> traverser) const
{
    if (!_children.empty()) {
        for (auto &i : _children) {
            i.traverse(traverser);
        }
    } else {
        traverser(this);
    }
}

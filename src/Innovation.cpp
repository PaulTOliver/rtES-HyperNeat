#include <Hyperneat/Innovation.hpp>

using namespace hyperneat;

Innovation::Innovation(size_t number, size_t source, size_t target, double depth, NodeType nodeType)
    : _number(number), _source(source), _target(target), _depth(depth), _nodeType(nodeType)
{}

bool
Innovation::operator==(const Innovation& other) const
{
    return (_source   == other._source  ) &&
           (_target   == other._target  ) &&
           (_depth    == other._depth   ) &&
           (_nodeType == other._nodeType);
}

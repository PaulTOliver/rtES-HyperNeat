#ifndef __HYPERNEAT_INNOVATION_HPP__
#define __HYPERNEAT_INNOVATION_HPP__

#include <Hyperneat/Utils/Size.hpp>
#include <Hyperneat/Utils/NodeTypes.hpp>

namespace hyperneat
{
    class Innovation
    {
    public:
        Innovation() = default;
        Innovation(size_t number, size_t source, size_t target, double depth, NodeType nodeType);

        bool operator== (const Innovation& other) const;

        size_t   _number   = 0;
        size_t   _source   = 0;
        size_t   _target   = 0;
        double   _depth    = 0.0;
        NodeType _nodeType = NodeType::NULL_TYPE;
    };
}

#endif

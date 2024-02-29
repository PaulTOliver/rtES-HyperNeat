#ifndef __HYPERNEAT_NODETYPES_HPP__
#define __HYPERNEAT_NODETYPES_HPP__

#include <HyperNeat/Utils/String.hpp>

namespace hyperneat
{
    enum class NodeType {
        NULL_TYPE = -1,
        SIGMOID   =  0,
        GAUSSIAN  =  1,
        SINE      =  2,
        ABSOLUTE  =  3,
    };

    String nodeToString(NodeType type);
    NodeType stringToNode(const String& str);

    const size_t NODE_TYPES_COUNT = 4;
}

#endif

#include <HyperNeat/Utils/NodeTypes.hpp>

namespace hyperneat
{
    String
    nodeToString(NodeType type)
    {
        switch (type) {
            case NodeType::SIGMOID:
                return "\"sigmoid\"";

            case NodeType::GAUSSIAN:
                return "\"gaussian\"";

            case NodeType::SINE:
                return "\"sine\"";

            case NodeType::ABSOLUTE:
                return "\"absolute\"";

            default:
                return "\"nullType\"";
        }
    }

    NodeType
    stringToNode(const String& str)
    {
        if (str == "\"sigmoid\"") {
            return NodeType::SIGMOID;
        } else if (str == "\"gaussian\"") {
            return NodeType::GAUSSIAN;
        } else if (str == "\"sine\"") {
            return NodeType::SINE;
        } else if (str == "\"absolute\"") {
            return NodeType::ABSOLUTE;
        } else {
            return NodeType::NULL_TYPE;
        }
    }
}

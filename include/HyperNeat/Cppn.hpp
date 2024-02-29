#ifndef __HYPERNEAT_CPPN_HPP__
#define __HYPERNEAT_CPPN_HPP__

#include <HyperNeat/Utils/Size.hpp>
#include <HyperNeat/Utils/ValueMap.hpp>
#include <HyperNeat/Utils/Vector2D.hpp>
#include <HyperNeat/Utils/NodeTypes.hpp>

namespace hyperneat
{
    class Genome;
    class NodeSearchPrms;

    class Cppn
    {
    public:
        Cppn() = default;

        void create(const Genome& genome);
        void clear();

        size_t getInputsCount() const;
        size_t getOutputsCount() const;
        size_t getNodesCount() const;

        double& inputAt(size_t i);
        double outputAt(size_t i) const;

        void cycle();
        void findNodesIn2DSection(ValueMap& valueMap, const NodeSearchPrms& qpPrms, const Point& source = Point());

    private:
        class Node
        {
        public:
            void appendInput();
            void flushOutput();

            class Link
            {
            public:
                double* _input  = nullptr;
                double  _weight = 0.0;
            };

            Vector<Link> _links;
            NodeType     _nodeType    = NodeType::NULL_TYPE;
            double       _storedInput = 0.0;
            double       _output      = 0.0;
        };

        Vector<double>  _inputs;
        Vector<double*> _outputs;
        Vector2D<Node>  _nodeLayers;
    };
}

#endif

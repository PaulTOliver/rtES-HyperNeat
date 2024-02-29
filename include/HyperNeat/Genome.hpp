#ifndef __HYPERNEAT_GENOME_HPP__
#define __HYPERNEAT_GENOME_HPP__

#include <HyperNeat/Utils/Map.hpp>
#include <HyperNeat/Utils/Size.hpp>
#include <HyperNeat/Utils/NodeTypes.hpp>

namespace hyperneat
{
    class Genome
    {
    public:
        Genome() = default;
        explicit Genome(size_t inputs);

        class NodeGene
        {
        public:
            NodeGene() = default;
            NodeGene(double depth, NodeType nodeType);

            class LinkGene
            {
            public:
                LinkGene() = default;
                LinkGene(double weight, bool isEnabled = true);

                double _weight    = 0.0;
                bool   _isEnabled = true;
            };

            double                _depth    = 0.0;
            NodeType              _nodeType = NodeType::NULL_TYPE;
            Map<size_t, LinkGene> _linkGenes;
        };

        size_t                _inputs = 0;
        Map<size_t, NodeGene> _nodeGenes;
    };
}

#endif

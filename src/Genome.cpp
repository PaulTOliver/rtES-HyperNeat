#include <HyperNeat/Genome.hpp>

using namespace hyperneat;

Genome::Genome(size_t inputs)
    : _inputs(inputs)
{}

Genome::NodeGene::NodeGene(double depth, NodeType nodeType)
    : _depth(depth), _nodeType(nodeType)
{}

Genome::NodeGene::LinkGene::LinkGene(double weight, bool isEnabled)
    : _weight(weight), _isEnabled(isEnabled)
{}

#include <HyperNeat/NeuralNetPrms.hpp>
#include <HyperNeat/NodeSearchPrms.hpp>

using namespace hyperneat;

NodeSearchPrms::NodeSearchPrms(size_t o, size_t x, size_t y)
    : _o(o), _x(x), _y(y), _useDistance(false)
{}

NodeSearchPrms::NodeSearchPrms(size_t o, size_t x, size_t y, size_t d)
    : _o(o), _x(x), _y(y), _d(d)
{}

void
NodeSearchPrms::importFrom(const NeuralNetPrms& nnPrms)
{
    _testGridLevel        = nnPrms._testGridLevel;
    _maxQuadTreeLevel     = nnPrms._maxQuadTreeLevel;
    _minQuadTreeLevel     = nnPrms._minQuadTreeLevel;
    _bandPruningThreshold = nnPrms._bandPruningThreshold;
    _varianceThreshold    = nnPrms._varianceThreshold;
    _divisionThreshold    = nnPrms._divisionThreshold;
}

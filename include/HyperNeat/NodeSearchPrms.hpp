#ifndef __HYPERNEAT_QUERYPLANEPRMS_HPP__
#define __HYPERNEAT_QUERYPLANEPRMS_HPP__

#include <HyperNeat/Utils/Size.hpp>

namespace hyperneat
{
    class NeuralNetPrms;

    class NodeSearchPrms
    {
    public:
        NodeSearchPrms() = default;
        NodeSearchPrms(size_t o, size_t x, size_t y);
        NodeSearchPrms(size_t o, size_t x, size_t y, size_t d);

        void importFrom(const NeuralNetPrms& nnPrms);

        size_t _o                    = 0;
        size_t _x                    = 0;
        size_t _y                    = 1;
        bool   _useDistance          = true;
        size_t _d                    = 4;
        size_t _testGridLevel        = 3;
        size_t _maxQuadTreeLevel     = -1;
        size_t _minQuadTreeLevel     = 0;
        double _bandPruningThreshold = 0.3;
        double _varianceThreshold    = 0.03;
        double _divisionThreshold    = 0.03;
    };
}

#endif

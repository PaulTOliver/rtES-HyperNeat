#ifndef __HYPERNEAT_NEURALNETPRMS_HPP__
#define __HYPERNEAT_NEURALNETPRMS_HPP__

#include <HyperNeat/Utils/Size.hpp>
#include <HyperNeat/Utils/Point.hpp>
#include <HyperNeat/Utils/Vector.hpp>

namespace hyperneat
{
    class NeuralNetPrms
    {
    public:
        Vector<Point> _inputMap;
        Vector<Point> _outputMap;
        size_t        _testGridLevel        = 3;
        size_t        _maxQuadTreeLevel     = SIZE_MAX;
        size_t        _minQuadTreeLevel     = 0;
        double        _bandPruningThreshold = 0.3;
        double        _varianceThreshold    = 0.03;
        double        _divisionThreshold    = 0.03;
        size_t        _iterations           = SIZE_MAX;
    };
}

#endif

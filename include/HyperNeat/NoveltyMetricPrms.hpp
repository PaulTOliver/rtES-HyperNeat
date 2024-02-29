#ifndef __HYPERNEAT_NOVELTY_METRIC_PRMS_HPP__
#define __HYPERNEAT_NOVELTY_METRIC_PRMS_HPP__

#include <Hyperneat/Utils/Size.hpp>

namespace hyperneat
{
    class Population;

    class NoveltyMetricPrms
    {
    public:
        double _noveltyThreshold         = 10.0;
        size_t _referenceOrganisms       = 15;
        size_t _characterizationSize     = 3;
        bool   _criteriaReachedByDefault = true;
    };
}

#endif

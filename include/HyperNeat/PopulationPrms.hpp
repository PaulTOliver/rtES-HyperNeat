#ifndef __HYPERNEAT_POPULATIONPRMS_HPP__
#define __HYPERNEAT_POPULATIONPRMS_HPP__

// #include <cereal/cereal.hpp>
#include <HyperNeat/Utils/Size.hpp>

namespace hyperneat
{
    class PopulationPrms
    {
    public:
        size_t _popSize                  = 200;
        size_t _cppnInputs               = 6;
        size_t _cppnOutputs              = 2;
        size_t _seed                     = 0;
        double _weightRange              = 1.0;
        double _c1Disjoint               = 1.0;
        double _c3WeightDifference       = 0.4;
        double _initialDistanceThreshold = 4.0;
        double _distanceThresholdShift   = 0.3;
        double _sexualReproductionRate   = 0.5;
        double _weightMutationRate       = 0.94;
        double _weightDeviation          = 0.25;
        double _interspeciesMatingRate   = 0.001;
        double _geneDisablingRatio       = 0.75;
        double _linkMutationRate         = 0.03;
        double _nodeMutationRate         = 0.01;
        size_t _targetSpeciesCount       = 8;
        double _eligibilityRatio         = 0.5;
        size_t _minimumLifetime          = 120;
        size_t _replBeforeReorganization = 5;

    // private:
    //     friend class cereal::access;

    //     template <class Archive>
    //     void serialize(Archive &ar)
    //     {
    //         ar(CEREAL_NVP(_popSize));
    //         ar(CEREAL_NVP(_cppnInputs));
    //         ar(CEREAL_NVP(_cppnOutputs));
    //         ar(CEREAL_NVP(_seed));
    //         ar(CEREAL_NVP(_weightRange));
    //         ar(CEREAL_NVP(_c1Disjoint));
    //         ar(CEREAL_NVP(_c3WeightDifference));
    //         ar(CEREAL_NVP(_initialDistanceThreshold));
    //         ar(CEREAL_NVP(_distanceThresholdShift));
    //         ar(CEREAL_NVP(_sexualReproductionRate));
    //         ar(CEREAL_NVP(_weightMutationRate));
    //         ar(CEREAL_NVP(_weightDeviation));
    //         ar(CEREAL_NVP(_interspeciesMatingRate));
    //         ar(CEREAL_NVP(_geneDisablingRatio));
    //         ar(CEREAL_NVP(_linkMutationRate));
    //         ar(CEREAL_NVP(_nodeMutationRate));
    //         ar(CEREAL_NVP(_targetSpeciesCount));
    //         ar(CEREAL_NVP(_eligibilityRatio));
    //         ar(CEREAL_NVP(_minimumLifetime));
    //         ar(CEREAL_NVP(_replBeforeReorganization));
    //     }
    };
}

#endif

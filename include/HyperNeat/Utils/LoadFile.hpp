#ifndef __HYPERNEAT_LOADFILE_HPP__
#define __HYPERNEAT_LOADFILE_HPP__

#include <iostream>
#include <HyperNeat/Utils/String.hpp>
#include <HyperNeat/Utils/Vector.hpp>

namespace hyperneat
{
    using Istream = std::istream;

    class Genome;
    class Organism;
    class Population;
    class NoveltyMetric;
    class PopulationPrms;
    class NoveltyMetricPrms;

    class LoadFile
    {
    public:
        LoadFile(Istream& stream);

        void loadPopulation(Population& population);
        void loadPopulationPrms(PopulationPrms& prms);
        void loadNeuralNetPrms(NeuralNetPrms& prms);
        void loadNoveltyMetric(NoveltyMetric& noveltyMetric);
        void loadNoveltyMetricPrms(NoveltyMetricPrms& prms);
        void loadOrganism(Organism& organism);
        void loadGenome(Genome& genome);

    private:
        Istream& nextPrm(bool arrayVal = false);
        Istream& nextArrayValue();

        Istream& _stream;
    };
}

#endif

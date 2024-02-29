#ifndef __HYPERNEAT_SAVEFILE_HPP__
#define __HYPERNEAT_SAVEFILE_HPP__

#include <iostream>
#include <HyperNeat/Utils/String.hpp>
#include <HyperNeat/Utils/Vector.hpp>

namespace hyperneat
{
    using Ostream = std::ostream;

    class Genome;
    class Organism;
    class Population;
    class NoveltyMetric;
    class NeuralNetPrms;
    class PopulationPrms;
    class NoveltyMetricPrms;

    class SaveFile
    {
    public:
        SaveFile(Ostream& stream);

        void savePopulation(Population& population, bool shuttedDown = false, size_t tabs = 0,
            const String& prefix = "");
        void savePopulationPrms(const PopulationPrms& prms, size_t tabs = 0, const String& prefix = "");
        void saveNeuralNetPrms(const NeuralNetPrms& prms, size_t tabs = 0, const String& prefix = "");
        void saveOrganism(const Organism& organism, bool shuttedDown = false, size_t tabs = 0,
            const String& prefix = "");
        void saveGenome(const Genome& genome, size_t tabs = 0, const String& prefix = "");
        void saveNoveltyMetric(const NoveltyMetric& noveltyMetric, bool shuttedDown = false, size_t tabs = 0,
            const String& prefix = "");
        void saveNoveltyMetricPrms(const NoveltyMetricPrms& noveltyMetricPrms, size_t tabs = 0,
            const String& prefix = "");

    private:
        Ostream& print(size_t tabs = 0);
        String   newl(size_t lines = 1);

        Ostream& _stream;
    };
}

#endif

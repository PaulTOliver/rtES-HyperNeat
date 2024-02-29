#ifndef __HYPERNEAT_POPULATION_HPP__
#define __HYPERNEAT_POPULATION_HPP__

#include <HyperNeat/Organism.hpp>
#include <HyperNeat/Innovation.hpp>
#include <HyperNeat/Utils/Random.hpp>
#include <HyperNeat/Utils/Vector.hpp>
#include <HyperNeat/Utils/Pointer.hpp>
#include <HyperNeat/NeuralNetPrms.hpp>
#include <HyperNeat/NoveltyMetric.hpp>
#include <HyperNeat/Utils/Vector2D.hpp>
#include <HyperNeat/Utils/Function.hpp>
#include <HyperNeat/PopulationPrms.hpp>

namespace hyperneat
{
    class NeuralNet;
    class NoveltyMetricPrms;

    class Population
    {
    public:
        void create(const PopulationPrms& popPrms);
        void create(const PopulationPrms& popPrms, const NeuralNetPrms& nnPrms);
        void create(const PopulationPrms& popPrms, const NeuralNetPrms& nnPrms, const NoveltyMetricPrms& nmPrms);
        void shutdown(bool resetOrganisms = false, bool archiveOrganisms = false);
        ~Population();

        void setMinimumLifetime(size_t lifetime);

        const PopulationPrms& getPopulationPrms() const;
        const NeuralNetPrms& getNeuralNetPrms() const;
        bool hasNeuralNets() const;
        const Vector<Organism>& getAllOrganisms() const;
        const Vector2D<Organism*>& getSpecies() const;

        Organism& getOrganism(size_t i);
        Organism& getChampion();
        const Vector<Organism*>& getSpecie(size_t i) const;

        void lock();
        void unlock();
        bool isLocked() const;

        void lockOrganism(size_t i);
        void unlockOrganism(size_t i);
        bool isOrganismLocked(size_t i) const;
        bool isAnyOrganismLocked() const;
        size_t getLockedOrganisms() const;

        void freezeOrganism(size_t i);
        void unfreezeOrganism(size_t i);
        bool isOrganismFrozen(size_t i) const;
        bool isAnyOrganismFrozen() const;
        size_t getFrozenOrganisms() const;

        bool isOrganismBeingGenerated(size_t i) const;
        bool isAnyOrganismBeingGenerated() const;
        size_t getOrganismsBeingGenerated() const;

        size_t getReadyOrganisms() const;

        const Vector<Innovation>& getInnovations() const;
        size_t getInnovationsCount() const;
        size_t getBasicInnovationsCount() const;
        Organism* getLastReplacement();
        Organism* getLastMother();
        Organism* getLastFather();
        size_t getReplacements() const;
        bool recentReplacement() const;
        double getDistanceThreshold() const;
        size_t getOldOrganisms() const;
        size_t getMinimumOldOrganisms() const;
        double getAverageFitness() const;
        double getAverageOldFitness() const;

        void setNoveltyMetric(const NoveltyMetricPrms& prms);
        void clearNoveltyMetric();
        bool isNoveltyMetricSet() const;
        const NoveltyMetric& getNoveltyMetric() const;

        size_t getUpdates() const;

        double& fitnessOf(size_t i);
        bool update(Function<void(void)> beforeReplacement = []() {}, Function<void(void)> afterReplacement = []() {});

    private:
        void generateAllNeuralNets();

        void replaceOrganism();
        Organism* killPoorOrganism();
        Vector<Organism*>* chooseParentSpecie();

        void breedAsexually(Genome& child, const Genome& mother);
        void breedSexually(Genome& child, const Genome& mother, const Genome& father);
        bool mutateNodesAndLinks(Genome& child);

        void assignToSpecie(Organism& org);
        void organizeSpecies();
        double computeDistance(const Genome& g1, const Genome& g2) const;

        size_t getRandSeed() const;
        size_t getRandSize(size_t low, size_t hi);
        double getRandReal(double low, double hi);
        double getRandWeight();
        double getWeightDeviation();
        NodeType getRandNodeType();
        bool getChance(double ratio);

        PopulationPrms         _prms;
        Pointer<NeuralNetPrms> _nnPrms;
        Vector2D<Organism*>    _species;
        Vector<Organism>       _allOrganisms;

        bool           _populationLock          = false;
        size_t         _lockedOrganisms         = 0;
        size_t         _frozenOrganisms         = 0;
        Atomic<size_t> _organismsBeingGenerated = {0};

        Vector<Innovation> _innovations;
        size_t             _basicInnovs = 0;

        Organism* _lastReplacement   = nullptr;
        Organism* _lastMother        = nullptr;
        Organism* _lastFather        = nullptr;
        size_t    _replacements      = 0;
        bool      _recentReplacement = false;
        double    _distanceThreshold = 0.0;
        size_t    _oldOrganisms      = 0;
        size_t    _minOldOrganisms   = 0;

        Pointer<NoveltyMetric> _noveltyMetric;

        size_t _updates = 0;

        RandGen  _randGen;
        IntDist  _nodeTypeSelector = IntDist(0, NODE_TYPES_COUNT - 1);
        RealDist _weightSelector   = RealDist(0.0, 1.0);
        BellDist _weightDeviator;
        RealDist _chanceSelector;

        friend class LoadFile;
        friend class Organism;
        friend class NoveltyMetric;
    };
}

#endif

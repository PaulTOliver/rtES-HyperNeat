#ifndef __HYPERNEAT_ORGANISM_HPP__
#define __HYPERNEAT_ORGANISM_HPP__

#include <HyperNeat/Genome.hpp>
#include <HyperNeat/NeuralNet.hpp>
#include <HyperNeat/Utils/Atomic.hpp>
#include <HyperNeat/Utils/Pointer.hpp>

namespace hyperneat
{
    class Behavior;
    class NeuralNet;
    class Population;
    class NeuralNetPrms;

    class Organism
    {
    public:
        Organism(const Organism& other);
        Organism& operator=(const Organism& other);

        size_t getIndex() const;

        void lock();
        void unlock();
        bool isLocked() const;

        void freeze();
        void unfreeze();
        bool isFrozen() const;

        bool isBeingGenerated() const;
        size_t getSpecie() const;
        bool isOld() const;
        size_t getLifetime() const;

        Behavior& getBehavior();
        const Genome& getGenome() const;

        bool isChampion() const;
        Population& getPopulation() const;

        void createNeuralNet();

        Pointer<NeuralNet> _neuralNet;
        double             _fitness = 0.0;

    private:
        Organism(Population* population);
        Organism(size_t inputs, Population* population);

        void reset(bool archive = false);

        size_t       _index            = 0;
        bool         _isLocked         = false;
        bool         _isFrozen         = false;
        Atomic<bool> _isBeingGenerated = {false};
        size_t       _specie           = 0;
        size_t       _lifetime         = 0;
        Behavior*    _behavior         = nullptr;
        Genome       _genome;

        Population* _population = nullptr;

        friend class LoadFile;
        friend class Population;
        friend class NoveltyMetric;
    };
}

#endif

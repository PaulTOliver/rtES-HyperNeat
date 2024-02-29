#include <HyperNeat/Cppn.hpp>
#include <HyperNeat/Behavior.hpp>
#include <HyperNeat/NeuralNet.hpp>
#include <HyperNeat/Population.hpp>
#include <HyperNeat/Utils/Thread.hpp>
#include <HyperNeat/NeuralNetPrms.hpp>

using namespace std;
using namespace hyperneat;

Organism::Organism(const Organism& other)
{
    *this = other;
}

Organism&
Organism::operator=(const Organism& other)
{
    _fitness    = other._fitness;
    _index      = other._index;
    _isLocked   = other._isLocked;
    _isFrozen   = other._isFrozen;
    _specie     = other._specie;
    _lifetime   = other._lifetime;
    _genome     = other._genome;
    _population = other._population;

    return *this;
}

size_t
Organism::getIndex() const
{
    return _index;
}

void
Organism::lock()
{
    if (!_isLocked) {
        _isLocked = true;
        ++_population->_lockedOrganisms;
    }
}

void
Organism::unlock()
{
    if (_isLocked) {
        _isLocked = false;
        --_population->_lockedOrganisms;
    }
}

bool
Organism::isLocked() const
{
    return _isLocked;
}

void
Organism::freeze()
{
    if (!_isFrozen) {
        _isFrozen = true;
        ++_population->_frozenOrganisms;
    }
}

void
Organism::unfreeze()
{
    if (_isFrozen) {
        _isFrozen = false;
        --_population->_frozenOrganisms;
    }
}

bool
Organism::isFrozen() const
{
    return _isFrozen;
}

bool
Organism::isBeingGenerated() const
{
    return _isBeingGenerated;
}

size_t
Organism::getSpecie() const
{
    return _specie;
}

bool
Organism::isOld() const {
    return _lifetime >= _population->_prms._minimumLifetime;
}

size_t
Organism::getLifetime() const
{
    return _lifetime;
}

Behavior&
Organism::getBehavior()
{
    return *_behavior;
}

const Genome&
Organism::getGenome() const
{
    return _genome;
}

bool
Organism::isChampion() const
{
    return &_population->getChampion() == this;
}

Population&
Organism::getPopulation() const
{
    return *_population;
}

void
Organism::createNeuralNet()
{
    if (_isBeingGenerated) {
        return;
    }

    _isBeingGenerated = true;
    ++_population->_organismsBeingGenerated;

    Thread generator([&]() {
        Cppn cppn;
        cppn.create(_genome);

        _neuralNet->clear();
        _neuralNet->create(cppn, _population->getNeuralNetPrms());

        _isBeingGenerated = false;
        --_population->_organismsBeingGenerated;
    });

    generator.detach();
}

Organism::Organism(Population* population)
    : _population(population)
{}

Organism::Organism(size_t inputs, Population* population)
    : _genome(inputs), _population(population)
{}

void
Organism::reset(bool archive)
{
    unlock();
    unfreeze();

    if (isOld()) {
        --_population->_oldOrganisms;
    }

    _lifetime = 0;
    _fitness  = 0.0;

    if (_behavior) {
        _behavior->reset(archive);
    }
}

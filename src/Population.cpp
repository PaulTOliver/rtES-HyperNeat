#include <chrono>
#include <algorithm>
#include <HyperNeat/Cppn.hpp>
#include <HyperNeat/NeuralNet.hpp>
#include <HyperNeat/Population.hpp>
#include <HyperNeat/Utils/Thread.hpp>

using namespace std;
using namespace hyperneat;

void
Population::create(const PopulationPrms& popPrms)
{
    if (_prms._seed != 0) {
        _randGen.seed(_prms._seed);
    } else {
        _randGen.seed(getRandSeed());
    }

    _prms              = popPrms;
    _weightDeviator    = BellDist(0.0, _prms._weightDeviation);
    _weightSelector    = RealDist(-_prms._weightRange, _prms._weightRange);
    _distanceThreshold = _prms._initialDistanceThreshold;
    _minOldOrganisms   = _prms._popSize * _prms._eligibilityRatio;
    _basicInnovs       = _prms._cppnOutputs * NODE_TYPES_COUNT + _prms._cppnInputs;

    _allOrganisms.resize(_prms._popSize, Organism(_prms._cppnInputs, this));

    size_t orgIndex = 0;

    for (auto& i : _allOrganisms) {
        i._index        = orgIndex++;
        auto& nodeGenes = i._genome._nodeGenes;

        for (size_t j = 0; j < _prms._cppnOutputs; ++j) {
            size_t nodeType    = _nodeTypeSelector(_randGen);
            size_t geneIdx     = _prms._cppnInputs + (j * NODE_TYPES_COUNT) + nodeType;
            nodeGenes[geneIdx] = {1.0, static_cast<NodeType>(nodeType)};

            for (size_t k = 0; k < _prms._cppnInputs; ++k) {
                nodeGenes[geneIdx]._linkGenes[k] = {getRandWeight()};
            }
        }
    }

    organizeSpecies();
}

void
Population::create(const PopulationPrms& popPrms, const NeuralNetPrms& nnPrms)
{
    create(popPrms);
    _nnPrms = Pointer<NeuralNetPrms>(new NeuralNetPrms(nnPrms));
    generateAllNeuralNets();
}

void
Population::create(const PopulationPrms& popPrms, const NeuralNetPrms& nnPrms, const NoveltyMetricPrms& nmPrms)
{
    create(popPrms, nnPrms);
    setNoveltyMetric(nmPrms);
}

void
Population::shutdown(bool resetOrganisms, bool archiveOrganisms)
{
    while (isAnyOrganismBeingGenerated());

    if (resetOrganisms) {
        for (auto& i : _allOrganisms) {
            i.reset(archiveOrganisms);
        }
    }
}

Population::~Population()
{
    while (isAnyOrganismBeingGenerated());
}

void
Population::setMinimumLifetime(size_t lifetime)
{
    _oldOrganisms          = 0;
    _prms._minimumLifetime = lifetime;

    for (auto& i : _allOrganisms) {
        if (i._lifetime >= lifetime) {
            ++_oldOrganisms;
        }
    }
}

const PopulationPrms&
Population::getPopulationPrms() const
{
    return _prms;
}

const NeuralNetPrms&
Population::getNeuralNetPrms() const
{
    return *_nnPrms;
}

bool
Population::hasNeuralNets() const
{
    return (bool)_nnPrms;
}

const Vector<Organism>&
Population::getAllOrganisms() const
{
    return _allOrganisms;
}

const Vector2D<Organism*>&
Population::getSpecies() const
{
    return _species;
}

Organism&
Population::getOrganism(size_t i)
{
    return _allOrganisms[i];
}

const Vector<Organism*>&
Population::getSpecie(size_t i) const
{
    return _species[i];
}

Organism&
Population::getChampion()
{
    auto comFitness = [](Organism& a, Organism& b) {
        return a._fitness < b._fitness;
    };

    auto comNovelty = [](Organism& a, Organism& b) {
        return a.getBehavior().getNoveltyScore() < b.getBehavior().getNoveltyScore();
    };

    return *max_element(_allOrganisms.begin(), _allOrganisms.end(), isNoveltyMetricSet() ? comNovelty : comFitness);
}

void
Population::lock()
{
    _populationLock = true;
}

void
Population::unlock()
{
    _populationLock = false;
}

bool
Population::isLocked() const
{
    return _populationLock;
}

void
Population::lockOrganism(size_t i)
{
    _allOrganisms[i].lock();
}

void
Population::unlockOrganism(size_t i)
{
    _allOrganisms[i].unlock();
}

bool
Population::isOrganismLocked(size_t i) const
{
    return _allOrganisms[i]._isLocked;
}

bool
Population::isAnyOrganismLocked() const
{
    return _lockedOrganisms != 0 ? true : false;
}

size_t
Population::getLockedOrganisms() const
{
    return _lockedOrganisms;
}

void
Population::freezeOrganism(size_t i)
{
    _allOrganisms[i].freeze();
}

void
Population::unfreezeOrganism(size_t i)
{
    _allOrganisms[i].unfreeze();
}

bool
Population::isOrganismFrozen(size_t i) const
{
    return _allOrganisms[i]._isFrozen;
}

bool
Population::isAnyOrganismFrozen() const
{
    return _frozenOrganisms != 0 ? true : false;
}

size_t
Population::getFrozenOrganisms() const
{
    return _frozenOrganisms;
}

bool
Population::isOrganismBeingGenerated(size_t i) const
{
    return _allOrganisms[i].isBeingGenerated();
}

bool
Population::isAnyOrganismBeingGenerated() const
{
    return _organismsBeingGenerated != 0 ? true : false;
}

size_t
Population::getOrganismsBeingGenerated() const
{
    return _organismsBeingGenerated;
}

size_t
Population::getReadyOrganisms() const
{
    size_t result = 0;

    for (auto& i : _allOrganisms) {
        if (i.isOld() && !i.isLocked()) {
            ++result;
        }
    }

    return result;
}

const Vector<Innovation>&
Population::getInnovations() const
{
    return _innovations;
}

size_t
Population::getInnovationsCount() const
{
    return _innovations.size();
}

size_t
Population::getBasicInnovationsCount() const
{
    return _basicInnovs;
}

Organism*
Population::getLastReplacement()
{
    return _lastReplacement;
}

Organism*
Population::getLastMother()
{
    return _lastMother;
}

Organism*
Population::getLastFather()
{
    return _lastFather;
}

size_t
Population::getReplacements() const
{
    return _replacements;
}

bool
Population::recentReplacement() const
{
    return _recentReplacement;
}

double
Population::getDistanceThreshold() const
{
    return _distanceThreshold;
}

size_t
Population::getOldOrganisms() const
{
    return _oldOrganisms;
}

size_t
Population::getMinimumOldOrganisms() const
{
    return _minOldOrganisms;
}

double
Population::getAverageFitness() const
{
    double total = 0.0;

    for (auto& i : _allOrganisms) {
        total += i._fitness;
    }

    return total / static_cast<double>(_allOrganisms.size());
}

double
Population::getAverageOldFitness() const
{
    double total = 0.0;

    for (auto& i : _allOrganisms) {
        if (i.isOld()) {
            total += i._fitness;
        }
    }

    return total / static_cast<double>(getOldOrganisms());
}

void
Population::setNoveltyMetric(const NoveltyMetricPrms& prms) {
    _noveltyMetric = Pointer<NoveltyMetric>(new NoveltyMetric);
    _noveltyMetric->initialize(prms, this);
}

void
Population::clearNoveltyMetric()
{
    _noveltyMetric.reset();

    for (auto& i : _allOrganisms) {
        i._behavior = nullptr;
    }
}

bool
Population::isNoveltyMetricSet() const
{
    return _noveltyMetric.get();
}

const NoveltyMetric&
Population::getNoveltyMetric() const
{
    return *_noveltyMetric;
}

size_t
Population::getUpdates() const
{
    return _updates;
}

double&
Population::fitnessOf(size_t i)
{
    return _allOrganisms[i]._fitness;
}

bool
Population::update(Function<void(void)> beforeReplacement, Function<void(void)> afterReplacement)
{
    ++_updates;

    for (auto& i : _allOrganisms) {
        if (!i._isFrozen && !i.isBeingGenerated()) {
            ++i._lifetime;

            if (i._lifetime == _prms._minimumLifetime) {
                ++_oldOrganisms;
            }
        }
    }

    if (_populationLock) {
        return false;
    }

    if ((getReadyOrganisms() >= _minOldOrganisms) && !isAnyOrganismBeingGenerated()) {
        beforeReplacement();

        if (isNoveltyMetricSet()) {
            _noveltyMetric->setScores();
        }

        _recentReplacement = true;
        replaceOrganism();
        afterReplacement();
    } else {
        _recentReplacement = false;
    }

    return _recentReplacement;
}

void
Population::generateAllNeuralNets()
{
    auto generateChunk = [&](size_t i, size_t end) {
        for (; i < end; ++i) {
            Cppn cppn;
            cppn.create(_allOrganisms[i].getGenome());

            _allOrganisms[i]._neuralNet = Pointer<NeuralNet>(new NeuralNet);
            _allOrganisms[i]._neuralNet->create(cppn, *_nnPrms);
        }
    };

    size_t threadCount = max(1u, Thread::hardware_concurrency());
    size_t chunkSize   = _allOrganisms.size() / threadCount;

    Vector<Thread> chunks(threadCount);

    for (size_t i = 0; i < threadCount - 1; ++i) {
        chunks[i] = Thread(generateChunk, i * chunkSize, (i + 1) * chunkSize);
    }

    chunks.back() = Thread(generateChunk, (chunks.size() - 1) * chunkSize, _allOrganisms.size());

    for (auto& i : chunks) {
        i.join();
    }
}

void
Population::replaceOrganism()
{
    _lastReplacement  = killPoorOrganism();
    _lastMother       = nullptr;
    _lastFather       = nullptr;
    auto parentSpecie = chooseParentSpecie();
    Vector<Organism*> eligibleMothers;

    for (auto& i : *parentSpecie) {
        if (i->_lifetime >= _prms._minimumLifetime) {
            eligibleMothers.emplace_back(i);
        }
    }

    size_t motherIdx = getRandSize(0, eligibleMothers.size() - 1);
    _lastMother      = eligibleMothers[motherIdx];

    if (getChance(_prms._sexualReproductionRate)) {
        if (getChance(_prms._interspeciesMatingRate)) {
            Vector<Organism*> eligibleFathers;

            for (auto& i : _species) {
                if (&i == parentSpecie) {
                    continue;
                }

                for (auto& j : i) {
                    if (j->_lifetime >= _prms._minimumLifetime) {
                        eligibleFathers.emplace_back(j);
                    }
                }
            }

            if (eligibleFathers.size() > 0) {
                size_t fatherIdx = getRandSize(0, eligibleFathers.size() - 1);
                _lastFather      = eligibleFathers[fatherIdx];
            }
        } else {
            if (eligibleMothers.size() > 1) {
                size_t fatherIdx = getRandSize(0, eligibleMothers.size() - 2);

                if (fatherIdx >= motherIdx) {
                    ++fatherIdx;
                }

                _lastFather = eligibleMothers[fatherIdx];
            }
        }

        if (!_lastFather) {
            breedAsexually(_lastReplacement->_genome, _lastMother->_genome);
        } else {
            if (_lastFather->_fitness > _lastMother->_fitness) {
                swap(_lastFather, _lastMother);
            }

            breedSexually(_lastReplacement->_genome, _lastMother->_genome, _lastFather->_genome);
        }
    } else {
        breedAsexually(_lastReplacement->_genome, _lastMother->_genome);
    }

    assignToSpecie(*_lastReplacement);
    ++_replacements;
    --_oldOrganisms;

    if ((_replacements % _prms._replBeforeReorganization) == 0) {
        organizeSpecies();
    }
}

Organism*
Population::killPoorOrganism()
{
    Organism* poorOrganism  = nullptr;
    double    minAdjFitness = -1.0;

    for (auto& i : _allOrganisms) {
        if (i._lifetime >= _prms._minimumLifetime && !i._isLocked) {
            double orgAdjFitness = i._fitness / static_cast<double>(_species[i._specie].size());

            if (orgAdjFitness < minAdjFitness || minAdjFitness == -1.0) {
                minAdjFitness = orgAdjFitness;
                poorOrganism  = &i;
            }
        }
    }

    if (poorOrganism->_isFrozen) {
        poorOrganism->_isFrozen = false;
        --_frozenOrganisms;
    }

    poorOrganism->_fitness  = 0.0;
    poorOrganism->_lifetime = 0;
    poorOrganism->_genome._nodeGenes.clear();

    if (poorOrganism->_behavior) {
        poorOrganism->_behavior->reset();
    }

    auto& childSpec = _species[poorOrganism->_specie];
    auto  childIdx  = find(childSpec.begin(), childSpec.end(), poorOrganism);
    childSpec.erase(childIdx);

    return poorOrganism;
}

Vector<Organism*>*
Population::chooseParentSpecie()
{
    double totalAverageFitnesses = 0.0;
    Vector<double> averageFitnesses;
    averageFitnesses.reserve(_species.size());

    for (auto &i : _species) {
        double specieFitness = 0.0;
        size_t specieSize    = 0;

        for (auto &j : i) {
            if (j->_lifetime >= _prms._minimumLifetime) {
                specieFitness += j->_fitness;
                ++specieSize;
            }
        }

        if (specieSize == 0) {
            averageFitnesses.emplace_back(0.0);
        } else {
            specieFitness /= static_cast<double>(specieSize);
            averageFitnesses.emplace_back(specieFitness);
            totalAverageFitnesses += specieFitness;
        }
    }

    size_t specieIdx = 0;
    double selector  = getRandReal(0.0, totalAverageFitnesses) - averageFitnesses.front();

    while (selector > 0.0) {
        ++specieIdx;
        selector -= averageFitnesses[specieIdx];
    }

    return &_species[specieIdx];
}

void
Population::breedAsexually(Genome& child, const Genome& mother)
{
    child = mother;

    if (mutateNodesAndLinks(child)) {
        return;
    }

    for (auto& i : child._nodeGenes) {
        for (auto& j : i.second._linkGenes) {
            if (getChance(_prms._weightMutationRate)) {
                j.second._weight += getWeightDeviation();

                if (j.second._weight > _prms._weightRange) {
                    j.second._weight = _prms._weightRange;
                } else if (j.second._weight < -_prms._weightRange) {
                    j.second._weight = -_prms._weightRange;
                }
            }
        }
    }
}

void
Population::breedSexually(Genome& child, const Genome& mother, const Genome& father)
{
    child = mother;

    for (auto& i : child._nodeGenes) {
        if (father._nodeGenes.count(i.first)) {
            auto& fatherNode = father._nodeGenes.at(i.first);

            for (auto& j : i.second._linkGenes) {
                if (fatherNode._linkGenes.count(j.first)) {
                    auto& childLink  = j.second;
                    auto& fatherLink = fatherNode._linkGenes.at(j.first);

                    childLink._weight += fatherLink._weight;
                    childLink._weight /= 2.0;

                    if (!childLink._isEnabled || !fatherLink._isEnabled) {
                        if (getChance(_prms._geneDisablingRatio)) {
                            childLink._isEnabled = false;
                        } else {
                            childLink._isEnabled = true;
                        }
                    }
                }
            }
        }
    }
}

bool
Population::mutateNodesAndLinks(Genome& child)
{
    class LinkMutation {
    public:
        LinkMutation() = default;
        LinkMutation(size_t source, size_t target)
            : _source(source), _target(target)
        {}

        size_t _source = 0;
        size_t _target = 0;
    };

    class NodeMutation {
    public:
        NodeMutation() = default;
        NodeMutation(size_t source, size_t target, double weight)
            : _source(source), _target(target), _weight(weight)
        {}

        size_t _source = 0;
        size_t _target = 0;
        double _weight = 0.0;
    };

    if (getChance(_prms._linkMutationRate)) {
        Vector<LinkMutation> linkMutations;

        for (auto& i : child._nodeGenes) {
            auto& childNode  = i.first;
            auto& childLinks = i.second._linkGenes;
            auto& childDepth = i.second._depth;

            for (size_t j = 0; j < child._inputs; ++j) {
                if (!childLinks.count(j)) {
                    linkMutations.emplace_back(j, childNode);
                }
            }

            for (auto& j : child._nodeGenes) {
                auto& sourceNode  = j.first;
                auto& sourceDepth = j.second._depth;

                if (!childLinks.count(sourceNode) && childDepth > sourceDepth) {
                    linkMutations.emplace_back(sourceNode, childNode);
                }
            }
        }

        if (!linkMutations.empty()) {
            size_t mutationIdx = getRandSize(0, linkMutations.size() - 1);
            size_t target      = linkMutations[mutationIdx]._target;
            size_t source      = linkMutations[mutationIdx]._source;

            child._nodeGenes[target]._linkGenes[source] = {0.0};
        }

        return true;
    }

    if (getChance(_prms._nodeMutationRate)) {
        Vector<NodeMutation> nodeMutations;

        for (auto& i : child._nodeGenes) {
            for (auto& j : i.second._linkGenes) {
                auto& childLink = j.second;

                if (childLink._isEnabled) {
                    nodeMutations.emplace_back(j.first, i.first, childLink._weight);
                }
            }
        }

        size_t mutationIdx = getRandSize(0, nodeMutations.size() - 1);
        size_t target      = nodeMutations[mutationIdx]._target;
        size_t source      = nodeMutations[mutationIdx]._source;
        double targetDepth = child._nodeGenes[target]._depth;
        double sourceDepth = 0.0;

        if (source >= child._inputs) {
            sourceDepth = child._nodeGenes[source]._depth;
        }

        double   depth    = (targetDepth + sourceDepth) / 2.0;
        NodeType function = getRandNodeType();

        Innovation innov = {0, source, target, depth, function};
        auto       iter  = find(_innovations.begin(), _innovations.end(), innov);
        size_t     newID = 0;

        if (iter != _innovations.end()) {
            newID = iter->_number;
        } else {
            newID = _innovations.size() + _basicInnovs;
            _innovations.emplace_back(newID, source, target, depth, function);
        }

        child._nodeGenes[newID]                                = {depth, function};
        child._nodeGenes[newID]._linkGenes[source]             = {0.0};
        child._nodeGenes[target]._linkGenes[newID]             = {0.0};
        child._nodeGenes[target]._linkGenes[source]._isEnabled = false;

        return true;
    }

    return false;
}

void
Population::assignToSpecie(Organism& org)
{
    bool included = false;

    for (auto i = _species.begin(), end = _species.end(); i != end; ++i) {
        if (i->empty()) {
            continue;
        }

        double genDistance = computeDistance((*i)[0]->_genome, org._genome);

        if (genDistance < _distanceThreshold) {
            included    = true;
            org._specie = distance(_species.begin(), i);
            i->emplace_back(&org);
            break;
        }
    }

    if (!included) {
        org._specie = _species.size();
        _species.emplace_back(1, &org);
    }
}

void
Population::organizeSpecies()
{
    _species.clear();

    for (auto& i : _allOrganisms) {
        assignToSpecie(i);
    }

    if (_species.size() < _prms._targetSpeciesCount && _distanceThreshold > _prms._distanceThresholdShift) {
        _distanceThreshold -= _prms._distanceThresholdShift;
    } else if (_species.size() > _prms._targetSpeciesCount) {
        _distanceThreshold += _prms._distanceThresholdShift;
    }
}


double
Population::computeDistance(const Genome& g1, const Genome& g2) const
{
    class LinkPair {
    public:
        const Genome::NodeGene::LinkGene* _l1 = nullptr;
        const Genome::NodeGene::LinkGene* _l2 = nullptr;
    };

    using LinkPairs = Map<size_t, LinkPair>;

    class GenePair {
    public:
        const Genome::NodeGene* _g1 = nullptr;
        const Genome::NodeGene* _g2 = nullptr;
        LinkPairs _links;
    };

    using GenePairs = Map<size_t, GenePair>;

    auto alignGenes = [](GenePairs& gPairs, const Genome& g1, const Genome& g2) {
        for (auto& i : g1._nodeGenes) {
            gPairs[i.first]._g1 = &i.second;

            for (auto& j : i.second._linkGenes) {
                gPairs[i.first]._links[j.first]._l1 = &j.second;
            }
        }

        for (auto& i : g2._nodeGenes) {
            gPairs[i.first]._g2 = &i.second;

            for (auto& j : i.second._linkGenes) {
                gPairs[i.first]._links[j.first]._l2 = &j.second;
            }
        }
    };

    GenePairs gPairs;
    alignGenes(gPairs, g1, g2);

    double g1Size           = 0.0;
    double g2Size           = 0.0;
    double disjointGenes    = 0.0;
    double linkGenePairs    = 0.0;
    double weightDifference = 0.0;

    for (auto& i : gPairs) {
        if (i.second._g1 && i.second._g2) {
            ++g1Size;
            ++g2Size;

            for (auto& j : i.second._links) {
                if (j.second._l1 && j.second._l2) {
                    ++g1Size;
                    ++g2Size;

                    ++linkGenePairs;
                    weightDifference += fabs(j.second._l1->_weight - j.second._l2->_weight);
                } else if (j.second._l1) {
                    ++g1Size;
                    ++disjointGenes;
                } else {
                    ++g2Size;
                    ++disjointGenes;
                }
            }
        } else if (i.second._g1) {
            double geneSize = static_cast<double>(1 + i.second._links.size());
            g1Size         += geneSize;
            disjointGenes  += geneSize;
        } else {
            double geneSize = static_cast<double>(1 + i.second._links.size());
            g2Size         += geneSize;
            disjointGenes  += geneSize;
        }
    }

    double normalize      = g1Size > g2Size ? g1Size : g2Size;
    double disjointFactor = (_prms._c1Disjoint * disjointGenes) / normalize;
    double weightFactor   = 0.0;

    if (linkGenePairs != 0.0) {
        weightFactor  = (_prms._c3WeightDifference * weightDifference) / linkGenePairs;
    }

    return disjointFactor + weightFactor;
}

size_t
Population::getRandSeed() const
{
    return chrono::system_clock::now().time_since_epoch().count();
}

size_t
Population::getRandSize(size_t low, size_t hi)
{
    return IntDist(low, hi)(_randGen);
}

double
Population::getRandReal(double low, double hi)
{
    return RealDist(low, hi)(_randGen);
}

double
Population::getRandWeight()
{
    return _weightSelector(_randGen);
}

double
Population::getWeightDeviation()
{
    return _weightDeviator(_randGen);
}

NodeType
Population::getRandNodeType()
{
    return static_cast<NodeType>(_nodeTypeSelector(_randGen));
}

bool
Population::getChance(double ratio)
{
    return _chanceSelector(_randGen) < ratio;
}

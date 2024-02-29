#include <HyperNeat/Population.hpp>
#include <HyperNeat/NoveltyMetric.hpp>
#include <HyperNeat/NeuralNetPrms.hpp>
#include <HyperNeat/Utils/LoadFile.hpp>

using namespace std;
using namespace hyperneat;

LoadFile::LoadFile(Istream& stream)
    : _stream(stream)
{
    _stream.setf(ios::boolalpha);
}

void
LoadFile::loadPopulation(Population& population)
{
    size_t  speciesCnt       = 0;
    size_t  innovationsCount = 0;
    ssize_t lastReplacement  = 0;
    ssize_t lastMother       = 0;
    ssize_t lastFather       = 0;
    bool    hasNoveltyMetric = false;
    bool    hasNeuralNets    = false;
    auto&   prms             = population._prms;

    nextPrm() >> population._populationLock;
    nextPrm() >> population._lockedOrganisms;
    nextPrm() >> population._frozenOrganisms;
    nextPrm() >> innovationsCount;
    nextPrm() >> population._basicInnovs;
    nextPrm() >> lastReplacement;
    nextPrm() >> lastMother;
    nextPrm() >> lastFather;
    nextPrm() >> population._replacements;
    nextPrm() >> population._distanceThreshold;
    nextPrm() >> population._oldOrganisms;
    nextPrm() >> population._minOldOrganisms;
    nextPrm() >> hasNoveltyMetric;
    nextPrm() >> population._updates;
    nextPrm() >> speciesCnt;
    nextPrm() >> hasNeuralNets;

    loadPopulationPrms(prms);

    if (hasNeuralNets) {
        population._nnPrms = Pointer<NeuralNetPrms>(new NeuralNetPrms);
        loadNeuralNetPrms(*population._nnPrms);
    }

    population._innovations.resize(innovationsCount);

    for (auto& i : population._innovations) {
        String nodeStr;

        nextPrm() >> i._number;
        nextPrm() >> i._source;
        nextPrm() >> i._target;
        nextPrm() >> i._depth;
        nextPrm() >> nodeStr;

        i._nodeType = stringToNode(nodeStr);
    }

    population._allOrganisms.resize(prms._popSize, Organism(&population));
    population._species.resize(speciesCnt);

    for (auto& i : population._allOrganisms) {
        loadOrganism(i);
    }

    population._lastReplacement = (lastReplacement == -1 ? nullptr: &population._allOrganisms[lastReplacement]);
    population._lastMother      = (lastMother      == -1 ? nullptr: &population._allOrganisms[lastMother]);
    population._lastFather      = (lastFather      == -1 ? nullptr: &population._allOrganisms[lastFather]);

    for (auto& i : population._species) {
        size_t specieSize = 0;
        nextPrm() >> specieSize;

        while (specieSize--) {
            size_t organismIdx = 0;
            nextArrayValue() >> organismIdx;

            i.emplace_back(&population._allOrganisms[organismIdx]);
        }
    }

    if (hasNoveltyMetric) {
        NoveltyMetricPrms nmPrms;
        loadNoveltyMetricPrms(nmPrms);

        population.setNoveltyMetric(nmPrms);
        loadNoveltyMetric(*population._noveltyMetric);
    }

    if (hasNeuralNets) {
        population.generateAllNeuralNets();
    }

    if (prms._seed != 0) {
        population._randGen.seed(prms._seed);
    } else {
        population._randGen.seed(population.getRandSeed());
    }

    population._weightDeviator = BellDist(0.0, prms._weightDeviation);
    population._weightSelector = RealDist(-prms._weightRange, prms._weightRange);

    population._organismsBeingGenerated = 0;
}

void
LoadFile::loadPopulationPrms(PopulationPrms& prms)
{
    nextPrm() >> prms._popSize;
    nextPrm() >> prms._cppnInputs;
    nextPrm() >> prms._cppnOutputs;
    nextPrm() >> prms._seed;
    nextPrm() >> prms._weightRange;
    nextPrm() >> prms._c1Disjoint;
    nextPrm() >> prms._c3WeightDifference;
    nextPrm() >> prms._initialDistanceThreshold;
    nextPrm() >> prms._distanceThresholdShift;
    nextPrm() >> prms._sexualReproductionRate;
    nextPrm() >> prms._weightMutationRate;
    nextPrm() >> prms._weightDeviation;
    nextPrm() >> prms._interspeciesMatingRate;
    nextPrm() >> prms._geneDisablingRatio;
    nextPrm() >> prms._linkMutationRate;
    nextPrm() >> prms._nodeMutationRate;
    nextPrm() >> prms._targetSpeciesCount;
    nextPrm() >> prms._eligibilityRatio;
    nextPrm() >> prms._minimumLifetime;
    nextPrm() >> prms._replBeforeReorganization;
}

void
LoadFile::loadNeuralNetPrms(NeuralNetPrms& prms)
{
    size_t inputs  = 0;
    size_t outputs = 0;

    nextPrm() >> prms._testGridLevel;
    nextPrm() >> prms._maxQuadTreeLevel;
    nextPrm() >> prms._minQuadTreeLevel;
    nextPrm() >> prms._bandPruningThreshold;
    nextPrm() >> prms._varianceThreshold;
    nextPrm() >> prms._divisionThreshold;
    nextPrm() >> prms._iterations;
    nextPrm() >> inputs;
    nextPrm() >> outputs;

    prms._inputMap.resize(inputs);
    prms._outputMap.resize(outputs);

    for (auto& i : prms._inputMap) {
        nextArrayValue() >> i._x;
        nextArrayValue() >> i._y;
    }

    for (auto& i : prms._outputMap) {
        nextArrayValue() >> i._x;
        nextArrayValue() >> i._y;
    }
}

void
LoadFile::loadNoveltyMetric(NoveltyMetric& noveltyMetric)
{
    for (auto& i : noveltyMetric._behaviors) {
        nextPrm() >> i._criteriaReached;
        nextPrm() >> i._noveltyScore;
        nextPrm() >> i._toBeArchived;

        for (auto& j : i._characterization) {
            nextArrayValue() >> j;
        }
    }

    size_t archiveSize = 0;
    nextPrm() >> archiveSize;
    noveltyMetric._archive.resize(archiveSize);

    for (auto& i : noveltyMetric._archive) {
        i.resize(noveltyMetric._prms._characterizationSize, 0.0);

        for (auto& j : i) {
            nextArrayValue() >> j;
        }
    }
}

void
LoadFile::loadNoveltyMetricPrms(NoveltyMetricPrms& noveltyMetricPrms)
{
    nextPrm() >> noveltyMetricPrms._noveltyThreshold;
    nextPrm() >> noveltyMetricPrms._referenceOrganisms;
    nextPrm() >> noveltyMetricPrms._characterizationSize;
    nextPrm() >> noveltyMetricPrms._criteriaReachedByDefault;
}

void
LoadFile::loadOrganism(Organism& organism)
{
    nextPrm() >> organism._index;
    nextPrm() >> organism._fitness;
    nextPrm() >> organism._isLocked;
    nextPrm() >> organism._isFrozen;
    nextPrm() >> organism._specie;
    nextPrm() >> organism._lifetime;

    loadGenome(organism._genome);
}

void
LoadFile::loadGenome(Genome& genome)
{
    size_t nodes = 0;

    nextPrm() >> genome._inputs;
    nextPrm() >> nodes;

    while (nodes--) {
        String nodeType;
        size_t links = 0;
        size_t innov = 0;

        nextPrm() >> innov;
        auto& nodeGene = genome._nodeGenes[innov];

        nextPrm() >> nodeGene._depth;
        nextPrm() >> nodeType;
        nodeGene._nodeType = stringToNode(nodeType);

        nextPrm() >> links;

        while (links--) {
            size_t source = 0;

            nextPrm() >> source;
            auto& linkGene = nodeGene._linkGenes[source];

            nextPrm() >> linkGene._weight;
            nextPrm() >> linkGene._isEnabled;
        }
    }
}

Istream&
LoadFile::nextPrm(bool arrayVal)
{
    for (;;) {
        char ch = static_cast<char>(_stream.peek());

        if (ch == '#') {
            _stream.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            if (!arrayVal) {
                _stream.ignore();

                if (ch == '=') {
                    return _stream;
                }
            } else {
                if (string("-0123456789").find(ch) != string::npos) {
                    return _stream;
                }

                _stream.ignore();
            }
        }
    }
}


Istream&
LoadFile::nextArrayValue()
{
    return nextPrm(true);
}

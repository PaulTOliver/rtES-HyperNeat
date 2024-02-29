#include <HyperNeat/Population.hpp>
#include <HyperNeat/NoveltyMetric.hpp>
#include <HyperNeat/NeuralNetPrms.hpp>
#include <HyperNeat/Utils/SaveFile.hpp>

using namespace std;
using namespace hyperneat;

SaveFile::SaveFile(Ostream& stream)
    : _stream(stream)
{
    _stream.setf(ios::boolalpha);
    _stream.precision(numeric_limits<double>::digits10);
}

void
SaveFile::savePopulation(Population& population, bool shuttedDown, size_t tabs, const String& prefix)
{
    print(tabs) << "# hyperneat::Population data:"                                                           << newl();
    print(tabs) << "# -------------------------------------------------------------------------------------" << newl();
    print(tabs) << "# DO NOT erase or change the order of any entry! Data is read back ENTRY by ENTRY."      << newl(2);

    ssize_t lastReplacement = (population.getLastReplacement() ? population.getLastReplacement()->getIndex() : -1);
    ssize_t lastMother      = (population.getLastMother() ? population.getLastMother()->getIndex() : -1);
    ssize_t lastFather      = (population.getLastFather() ? population.getLastFather()->getIndex() : -1);

    print(tabs) << "[" << prefix << "population]"                                                    << newl();
    print(tabs + 1) << "populationLock    = " << (shuttedDown ? false : population.isLocked())       << newl();
    print(tabs + 1) << "lockedOrganisms   = " << (shuttedDown ? 0 : population.getLockedOrganisms()) << newl();
    print(tabs + 1) << "frozenOrganisms   = " << (shuttedDown ? 0 : population.getFrozenOrganisms()) << newl();
    print(tabs + 1) << "innovationsCount  = " << population.getInnovationsCount()                    << newl();
    print(tabs + 1) << "basicInnovations  = " << population.getBasicInnovationsCount()               << newl();
    print(tabs + 1) << "lastReplacement   = " << lastReplacement                                     << newl();
    print(tabs + 1) << "lastMother        = " << lastMother                                          << newl();
    print(tabs + 1) << "lastFather        = " << lastFather                                          << newl();
    print(tabs + 1) << "replacements      = " << population.getReplacements()                        << newl();
    print(tabs + 1) << "distanceThreshold = " << population.getDistanceThreshold()                   << newl();
    print(tabs + 1) << "oldOrganisms      = " << (shuttedDown ? 0 : population.getOldOrganisms())    << newl();
    print(tabs + 1) << "minOldOrganisms   = " << population.getMinimumOldOrganisms()                 << newl();
    print(tabs + 1) << "noveltyMetric     = " << population.isNoveltyMetricSet()                     << newl();
    print(tabs + 1) << "updates           = " << population.getUpdates()                             << newl();
    print(tabs + 1) << "species           = " << population.getSpecies().size()                      << newl();
    print(tabs + 1) << "withNeuralNets    = " << population.hasNeuralNets()                          << newl(2);

    savePopulationPrms(population.getPopulationPrms(), tabs + 1, prefix + "population.");

    if (population.hasNeuralNets()) {
        print() << newl();
        saveNeuralNetPrms(population.getNeuralNetPrms(), tabs + 1, prefix + "population.");
    }

    for (auto& i : population.getInnovations()) {
        print()                                                       << newl();
        print(tabs + 1) << "[[" + prefix + "population.innovation]]"  << newl();
        print(tabs + 2) << "number   = " << i._number                 << newl();
        print(tabs + 2) << "source   = " << i._source                 << newl();
        print(tabs + 2) << "target   = " << i._target                 << newl();
        print(tabs + 2) << "depth    = " << i._depth                  << newl();
        print(tabs + 2) << "function = " << nodeToString(i._nodeType) << newl();
    }

    for (auto& i : population.getAllOrganisms()) {
        print() << newl();
        saveOrganism(i, shuttedDown, tabs + 1, prefix + "population.");
    }

    for (auto& i : population.getSpecies()) {
        print() << newl();
        print(tabs + 1) << "[[" + prefix + "population.specie]]" << newl();
        print(tabs + 2) << "size    = " << i.size()              << newl();
        print(tabs + 2) << "members = ["                         << newl();

        for (auto& j : i) {
            print(tabs + 3) << j->getIndex() << "," << newl();
        }

        print(tabs + 2) << "]" << newl();
    }

    if (population.isNoveltyMetricSet()) {
        print() << newl();
        saveNoveltyMetric(population.getNoveltyMetric(), shuttedDown, tabs + 1, prefix + "population.");
    }
}

void
SaveFile::savePopulationPrms(const PopulationPrms& prms, size_t tabs, const String& prefix)
{
    print(tabs) << "[" << prefix << "parameters]"                                      << newl();
    print(tabs + 1) << "popSize                  = " << prms._popSize                  << newl();
    print(tabs + 1) << "cppnInputs               = " << prms._cppnInputs               << newl();
    print(tabs + 1) << "cppnOutputs              = " << prms._cppnOutputs              << newl();
    print(tabs + 1) << "seed                     = " << prms._seed                     << newl();
    print(tabs + 1) << "weightRange              = " << prms._weightRange              << newl();
    print(tabs + 1) << "disjointCoeff            = " << prms._c1Disjoint               << newl();
    print(tabs + 1) << "weightDifferenceCoeff    = " << prms._c3WeightDifference       << newl();
    print(tabs + 1) << "initialDistanceThreshold = " << prms._initialDistanceThreshold << newl();
    print(tabs + 1) << "distanceThresholdShift   = " << prms._distanceThresholdShift   << newl();
    print(tabs + 1) << "sexualReproductionRate   = " << prms._sexualReproductionRate   << newl();
    print(tabs + 1) << "weightMutationRate       = " << prms._weightMutationRate       << newl();
    print(tabs + 1) << "weightDeviation          = " << prms._weightDeviation          << newl();
    print(tabs + 1) << "interspeciesMatingRate   = " << prms._interspeciesMatingRate   << newl();
    print(tabs + 1) << "geneDisablingRatio       = " << prms._geneDisablingRatio       << newl();
    print(tabs + 1) << "linkMutationRate         = " << prms._linkMutationRate         << newl();
    print(tabs + 1) << "nodeMutationRate         = " << prms._nodeMutationRate         << newl();
    print(tabs + 1) << "targetSpeciesCount       = " << prms._targetSpeciesCount       << newl();
    print(tabs + 1) << "eligibilityRatio         = " << prms._eligibilityRatio         << newl();
    print(tabs + 1) << "minimumLifetime          = " << prms._minimumLifetime          << newl();
    print(tabs + 1) << "replBeforeReorganization = " << prms._replBeforeReorganization << newl();
}

void
SaveFile::saveNeuralNetPrms(const NeuralNetPrms& prms, size_t tabs, const String& prefix)
{
    print(tabs) << "[" << prefix << "neuralNetParameters]"                     << newl();
    print(tabs + 1) << "testGridLevel        = " << prms._testGridLevel        << newl();
    print(tabs + 1) << "maxQuadTreeLevel     = " << prms._maxQuadTreeLevel     << newl();
    print(tabs + 1) << "minQuadTreeLevel     = " << prms._minQuadTreeLevel     << newl();
    print(tabs + 1) << "bandPruningThreshold = " << prms._bandPruningThreshold << newl();
    print(tabs + 1) << "varianceThreshold    = " << prms._varianceThreshold    << newl();
    print(tabs + 1) << "divisionThreshold    = " << prms._divisionThreshold    << newl();
    print(tabs + 1) << "iterations           = " << prms._iterations           << newl();
    print(tabs + 1) << "inputs               = " << prms._inputMap.size()      << newl();
    print(tabs + 1) << "outputs              = " << prms._outputMap.size()     << newl(2);
    print(tabs + 1) << "inputMap = ["                                          << newl();

    for (auto& i : prms._inputMap) {
        print(tabs + 2) << "[" << i._x << ", " << i._y << "]," << newl();
    }

    print(tabs + 1) << "]"             << newl(2);
    print(tabs + 1) << "outputMap = [" << newl();

    for (auto& i : prms._outputMap) {
        print(tabs + 2) << "[" << i._x << ", " << i._y << "]," << newl();
    }

    print(tabs + 1) << "]" << newl();
}

void
SaveFile::saveOrganism(const Organism& organism, bool shuttedDown, size_t tabs, const String& prefix)
{
    print(tabs) << "[[" << prefix << "organism]]" << newl();

    if (organism.isChampion()) {
        print(tabs + 1) << "# Current champion." << newl(2);
    }

    print(tabs + 1) << "index    = " << organism.getIndex()                         << newl();
    print(tabs + 1) << "fitness  = " << (shuttedDown ? 0 : organism._fitness)       << newl();
    print(tabs + 1) << "isLocked = " << (shuttedDown ? false : organism.isLocked()) << newl();
    print(tabs + 1) << "isFrozen = " << (shuttedDown ? false : organism.isFrozen()) << newl();
    print(tabs + 1) << "specie   = " << organism.getSpecie()                        << newl();
    print(tabs + 1) << "lifetime = " << (shuttedDown ? 0 : organism.getLifetime())  << newl(2);

    saveGenome(organism.getGenome(), tabs + 1, prefix + "organism.");

    // If shutted down, save Neural Net
}

void
SaveFile::saveGenome(const Genome& genome, size_t tabs, const String& prefix)
{
    print(tabs) << "[" << prefix << "genome]"                     << newl();
    print(tabs + 1) << "inputs = "    << genome._inputs           << newl();
    print(tabs + 1) << "nodes  = "    << genome._nodeGenes.size() << newl();

    for (auto& i : genome._nodeGenes) {
        print()                                                                << newl();
        print(tabs + 1) << "[[" << prefix << "genome.nodeGene]]"               << newl();
        print(tabs + 2) << "innovation = " << i.first                          << newl();
        print(tabs + 2) << "depth      = " << i.second._depth                  << newl();
        print(tabs + 2) << "function   = " << nodeToString(i.second._nodeType) << newl();
        print(tabs + 2) << "links      = " << i.second._linkGenes.size()       << newl();

        for (auto& j : i.second._linkGenes) {
            print()                                                           << newl();
            print(tabs + 2) << "[[" << prefix << "genome.nodeGene.linkGene]]" << newl();
            print(tabs + 3) << "source    = " << j.first                      << newl();
            print(tabs + 3) << "weight    = " << j.second._weight             << newl();
            print(tabs + 3) << "isEnabled = " << j.second._isEnabled          << newl();
        }
    }
}

void
SaveFile::saveNoveltyMetric(const NoveltyMetric& noveltyMetric, bool shuttedDown, size_t tabs, const String& prefix)
{
    print(tabs) << "[" << prefix << "noveltyMetric]" << newl();
    saveNoveltyMetricPrms(noveltyMetric.getPrms(), tabs + 1, prefix + "noveltyMetric.");

    bool critDef = noveltyMetric.getPrms()._criteriaReachedByDefault;

    for (auto& i : noveltyMetric.getBehaviors()) {
        print()                                                                                   << newl();
        print(tabs + 1) << "[[" << prefix << "noveltyMetric.behavior]]"                           << newl();
        print(tabs + 2) << "criteriaReached  = " << (shuttedDown ? critDef : i._criteriaReached)  << newl();
        print(tabs + 2) << "noveltyScore     = " << (shuttedDown ? 0 : i.getNoveltyScore())       << newl();
        print(tabs + 2) << "toBeArchived     = " << (shuttedDown ? false : i.isToBeArchived())    << newl();
        print(tabs + 2) << "characterization = ["                                                 << newl();

        for (auto& j : i.getCharacterization()) {
            print(tabs + 3) << j << "," << newl();
        }

        print(tabs + 2) << "]" << newl();
    }

    print()                                                                        << newl();
    print(tabs + 1) << "[" << prefix << "noveltyMetric.archive]"                   << newl();
    print(tabs + 2) << "size              = " << noveltyMetric.getArchive().size() << newl();
    print(tabs + 2) << "characterizations = ["                                     << newl();

    for (auto& i : noveltyMetric.getArchive()) {
        print(tabs + 3) << "[" << newl();

        for (auto& j : i) {
            print(tabs + 4) << j << "," << newl();
        }

        print(tabs + 3) << "]," << newl();
    }

    print(tabs + 2) << "]" << newl();
}

void
SaveFile::saveNoveltyMetricPrms(const NoveltyMetricPrms& noveltyMetricPrms, size_t tabs, const String& prefix)
{
    print(tabs) << "[" << prefix << "parameters]" << newl();
    print(tabs + 1) << "noveltyThreshold         = " << noveltyMetricPrms._noveltyThreshold         << newl();
    print(tabs + 1) << "referenceOrganisms       = " << noveltyMetricPrms._referenceOrganisms       << newl();
    print(tabs + 1) << "characterizationSize     = " << noveltyMetricPrms._characterizationSize     << newl();
    print(tabs + 1) << "criteriaReachedByDefault = " << noveltyMetricPrms._criteriaReachedByDefault << newl();
}

Ostream&
SaveFile::print(size_t tabs)
{
    _stream << String(tabs * 4, ' ');
    return _stream;
}

String
SaveFile::newl(size_t lines)
{
    return String(lines, '\n');
}

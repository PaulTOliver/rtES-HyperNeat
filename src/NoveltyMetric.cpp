#include <algorithm>
#include <Hyperneat/Population.hpp>
#include <Hyperneat/NoveltyMetric.hpp>

using namespace std;
using namespace hyperneat;

const NoveltyMetricPrms&
NoveltyMetric::getPrms() const
{
    return _prms;
}

const Vector<Behavior>&
NoveltyMetric::getBehaviors() const
{
    return _behaviors;
}

const Vector2D<double>&
NoveltyMetric::getArchive() const
{
    return _archive;
}

Behavior&
NoveltyMetric::getBehaviorOf(size_t i)
{
    return _behaviors[i];
}

void
NoveltyMetric::initialize(const NoveltyMetricPrms& prms, Population* population)
{
    _prms = prms;
    _behaviors.resize(population->getAllOrganisms().size());

    for (size_t i = 0; i < _behaviors.size(); ++i) {
        population->getOrganism(i)._behavior = &_behaviors[i];
        _behaviors[i]._organism              = &population->getOrganism(i);
        _behaviors[i]._noveltyMetric         = this;
        _behaviors[i]._criteriaReached       = prms._criteriaReachedByDefault;
        _behaviors[i]._characterization.resize(prms._characterizationSize, 0.0);
    }
}

void
NoveltyMetric::setScores()
{
    size_t readyOrganisms = 0;

    for (auto& i : _behaviors) {
        if ((i._isReady = (i._organism->isOld() && i._criteriaReached))) {
            ++readyOrganisms;
        }
    }

    for (auto& i : _behaviors) {
        if (!i._isReady) {
            i._toBeArchived       = false;
            i._organism->_fitness = i._noveltyScore = 0.0;
            continue;
        }

        Vector<double> distances;
        distances.reserve(readyOrganisms + _archive.size() - 1);

        for (auto& j : _behaviors) {
            if (j._isReady && (&i != &j)) {
                distances.emplace_back(getDistance(i._characterization, j._characterization));
            }
        }

        for (auto& j : _archive) {
            distances.emplace_back(getDistance(i._characterization, j));
        }

        auto dEnd = distances.end();
        auto dBeg = distances.begin();
        auto dRef = dBeg + _prms._referenceOrganisms;

        partial_sort(dBeg, dRef, dEnd);

        double totalDistance  = accumulate(dBeg, dRef, 0.0);
        i._organism->_fitness = i._noveltyScore = (totalDistance / static_cast<double>(_prms._referenceOrganisms));
        i._toBeArchived       = (i._noveltyScore > _prms._noveltyThreshold);
    }
}

double
NoveltyMetric::getDistance(const Vector<double>& v1, const Vector<double>& v2) const
{
    double result = 0.0;

    for (size_t i = 0; i < v1.size(); ++i) {
        double diff = v1[i] - v2[i];
        result     += diff * diff;
    }

    return sqrt(result);
}

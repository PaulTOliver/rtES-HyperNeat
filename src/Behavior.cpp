#include <HyperNeat/Behavior.hpp>
#include <HyperNeat/NoveltyMetric.hpp>

using namespace hyperneat;

void
Behavior::clear()
{
    _characterization.assign(_characterization.size(), 0.0);
}

void
Behavior::reset(bool archive)
{
    if (_toBeArchived && archive) {
        _noveltyMetric->_archive.emplace_back(_characterization);
    }

    _noveltyScore    = 0.0;
    _toBeArchived    = false;
    _criteriaReached = _noveltyMetric->_prms._criteriaReachedByDefault;
    _characterization.assign(_characterization.size(), 0.0);
}

double&
Behavior::at(size_t i)
{
    return _characterization[i];
}

double
Behavior::getNoveltyScore() const
{
    return _noveltyScore;
}

bool
Behavior::isToBeArchived() const
{
    return _toBeArchived;
}

Organism&
Behavior::getOrganism()
{
    return *_organism;
}

const NoveltyMetric&
Behavior::getNoveltyMetric() const
{
    return *_noveltyMetric;
}

const Vector<double>&
Behavior::getCharacterization() const
{
    return _characterization;
}

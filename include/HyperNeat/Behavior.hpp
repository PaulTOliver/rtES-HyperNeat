#ifndef __HYPERNEAT_BEHAVIOR_HPP__
#define __HYPERNEAT_BEHAVIOR_HPP__

#include <HyperNeat/Utils/Size.hpp>
#include <HyperNeat/Utils/Vector.hpp>

namespace hyperneat
{
    class Organism;
    class NoveltyMetric;

    class Behavior
    {
    public:
        void clear();
        void reset(bool archive = true);

        double& at(size_t i);
        double getNoveltyScore() const;
        bool isToBeArchived() const;
        Organism& getOrganism();
        const NoveltyMetric& getNoveltyMetric() const;
        const Vector<double>& getCharacterization() const;

        bool _criteriaReached = true;

    private:
        double         _noveltyScore  = 0.0;
        bool           _isReady       = false;
        bool           _toBeArchived  = false;
        Organism*      _organism      = nullptr;
        NoveltyMetric* _noveltyMetric = nullptr;
        Vector<double> _characterization;

        friend class LoadFile;
        friend class NoveltyMetric;
    };
}

#endif

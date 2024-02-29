#ifndef __HYPERNEAT_NOVELTY_METRIC_HPP__
#define __HYPERNEAT_NOVELTY_METRIC_HPP__

#include <Hyperneat/Behavior.hpp>
#include <Hyperneat/Utils/Vector2D.hpp>
#include <Hyperneat/NoveltyMetricPrms.hpp>

namespace hyperneat
{
    class Population;

    class NoveltyMetric
    {
    public:
        const NoveltyMetricPrms& getPrms() const;
        const Vector<Behavior>& getBehaviors() const;
        const Vector2D<double>& getArchive() const;

        Behavior& getBehaviorOf(size_t i);

    private:
        void initialize(const NoveltyMetricPrms& prms, Population* population = nullptr);
        void setScores();

        double getDistance(const Vector<double>& v1, const Vector<double>& v2) const;

        NoveltyMetricPrms _prms;
        Vector<Behavior>  _behaviors;
        Vector2D<double>  _archive;

        friend class Behavior;
        friend class LoadFile;
        friend class Population;
    };
}

#endif

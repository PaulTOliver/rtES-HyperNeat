#ifndef __HYPERNEAT_ATOM_HPP__
#define __HYPERNEAT_ATOM_HPP__

#include <atomic>

namespace hyperneat
{
    template <class T>
    using Atomic = std::atomic<T>;
}

#endif

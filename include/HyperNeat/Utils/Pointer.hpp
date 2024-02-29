#ifndef __HYPERNEAT_POINTER_HPP__
#define __HYPERNEAT_POINTER_HPP__

#include <memory>

namespace hyperneat
{
    template <class T>
    using Pointer = std::unique_ptr<T>;
}

#endif

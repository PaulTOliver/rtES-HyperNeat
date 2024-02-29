#ifndef __HYPERNEAT_FUNCTION_HPP__
#define __HYPERNEAT_FUNCTION_HPP__

#include <functional>

namespace hyperneat
{
    template <class F>
    using Function = std::function<F>;
}

#endif

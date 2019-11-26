#pragma once

#if defined(UBOOST_USE_BOOST)
#include <boost/context/stack_context.hpp>
#endif

#include <cstdint>

namespace uboost
{
namespace context
{
struct stack_context
{
    std::size_t size = 0;
    void *sp = nullptr;

#if defined(UBOOST_USE_BOOST)
    operator boost::context::stack_context() noexcept
    {
        return boost::context::stack_context{size, sp};
    }
#endif
};
} // namespace context
} // namespace uboost

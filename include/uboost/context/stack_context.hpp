#pragma once

#if defined(UBOOST_USE_BOOST)
#include <boost/context/stack_context.hpp>

namespace uboost
{
namespace context
{
using stack_context = boost::context::stack_context;
}
} // namespace uboost
#else
#include <cstdint>

namespace uboost
{
namespace context
{
struct stack_context
{
    std::size_t size = 0;
    void *sp = nullptr;
};
} // namespace context
} // namespace uboost
#endif

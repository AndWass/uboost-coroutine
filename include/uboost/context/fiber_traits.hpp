#pragma once

#include <type_traits>
#include <uboost/context/stop_token.hpp>

namespace uboost
{
namespace context
{
class fiber;

template <class Fn>
constexpr bool is_fiber_invocable =
    std::is_invocable_r_v<fiber, std::decay_t<Fn>, fiber &&, stop_token>;
} // namespace context
} // namespace uboost

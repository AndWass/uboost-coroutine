#pragma once

#include <uboost/context/fiber.hpp>
#include <uboost/coroutine/detail/pull_coroutine.hpp>
#include <uboost/coroutine/detail/push_coroutine.hpp>

namespace uboost
{
namespace coroutine
{
namespace detail
{
template <class T>
struct push_coroutine<T>::push_control_block
{
    uboost::context::fiber fiber_;
    bool stopped_ = false;
    typename pull_coroutine<T>::pull_control_block *other_;

    push_control_block(uboost::context::fiber &&fib,
                       typename pull_coroutine<T>::pull_control_block *other) noexcept;

    template <class Fn>
    push_control_block(uboost::context::stack_context stack, Fn &&fn) noexcept;

    ~push_control_block() noexcept;
};
} // namespace detail
} // namespace coroutine
} // namespace uboost

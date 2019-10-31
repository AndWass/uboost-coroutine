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
struct pull_coroutine<T>::pull_control_block
{
    uboost::context::fiber fiber_;
    typename push_coroutine<T>::push_control_block *other_;
    alignas(T) std::uint8_t value_storage_[sizeof(T)];
    bool valid_ = false;

    pull_control_block(uboost::context::fiber &&fib,
                       typename push_coroutine<T>::push_control_block *other) noexcept
        : fiber_(std::move(fib)), other_(other) {
    }

    template <class Fn>
    pull_control_block(uboost::context::stack_context stack, Fn &&fn) noexcept;

    ~pull_control_block() noexcept;

    void resume() {
        fiber_ = std::move(fiber_).resume();
    }

    T *value_ptr() noexcept {
        return std::launder(static_cast<T *>(static_cast<void *>(value_storage_)));
    }
};
} // namespace detail
} // namespace coroutine
} // namespace uboost

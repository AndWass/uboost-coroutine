#pragma once

#include <uboost/context/stack_context.hpp>

namespace uboost
{
namespace coroutine
{
namespace detail
{
template <class T>
class push_coroutine
{
    template <class U>
    friend class pull_coroutine;

    struct push_control_block;

    push_control_block *cb_;

    struct preallocated
    {
    };

    push_coroutine(preallocated, push_control_block *cb) noexcept : cb_(cb) {
    }

public:
    template <typename Fn>
    push_coroutine(uboost::context::stack_context, Fn &&) noexcept;

    ~push_coroutine() noexcept;

    push_coroutine(push_coroutine const &) = delete;
    push_coroutine &operator=(push_coroutine const &) = delete;

    // push_coroutine(push_coroutine &&) noexcept;
    push_coroutine &operator()(T const &) noexcept;
    push_coroutine &operator()(T &&) noexcept;

    explicit operator bool() const noexcept;

    // bool operator!() const noexcept;

    void stop() noexcept;
};
} // namespace detail
} // namespace coroutine
} // namespace uboost

#include <uboost/coroutine/detail/push_coroutine_impl.hpp>

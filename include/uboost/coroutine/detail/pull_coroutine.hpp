#pragma once

#include <uboost/context/fiber.hpp>
#include <uboost/context/stack_context.hpp>

namespace uboost
{
namespace coroutine
{
namespace detail
{
template <class T>
class pull_coroutine
{
    template <class U>
    friend class push_coroutine;

    struct pull_control_block;

    pull_control_block *cb_;

    struct preallocated
    {
    };

    pull_coroutine(preallocated, pull_control_block *cb) noexcept : cb_(cb) {
    }

public:
    template <class Fn>
    explicit pull_coroutine(uboost::context::stack_context stack, Fn &&fn) noexcept;

    ~pull_coroutine() noexcept;

    pull_coroutine(pull_coroutine const &) = delete;
    pull_coroutine &operator=(pull_coroutine const &) = delete;

    template<class Fn>
    pull_coroutine& map_and_pull(Fn &&fn) noexcept;
    // pull_coroutine(pull_coroutine &&) noexcept;
    pull_coroutine &operator()() noexcept;
    explicit operator bool() const noexcept;
    // bool operator!() const noexcept;
    T get() noexcept;

    void stop() noexcept;
};
} // namespace detail
} // namespace coroutine
} // namespace uboost

#include <uboost/coroutine/detail/pull_coroutine_impl.hpp>

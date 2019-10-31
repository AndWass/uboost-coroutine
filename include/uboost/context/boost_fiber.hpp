#pragma once

#include <boost/context/fiber.hpp>
#include <uboost/context/fiber_traits.hpp>
#include <uboost/context/stack_context.hpp>
#include <uboost/context/stop_token.hpp>

namespace uboost
{
namespace context
{

namespace detail
{
template <class Fn>
struct wrap_uboost_fn
{
    static auto wrap(Fn &&fn, stop_token stopper) {
        return [f = std::forward<Fn>(fn),
                stopper](boost::context::fiber &&ctx) mutable -> boost::context::fiber {
            return f(std::move(ctx), stopper).fib_;
        };
    }
};
} // namespace detail

class fiber
{
    template <class Fn>
    friend struct detail::wrap_uboost_fn;

    uboost::context::detail::stop_state stop_state_;
    boost::context::fiber fib_;

    fiber(boost::context::fiber &&fib) : fib_(std::move(fib)) {
    }

public:
    template <class Fn>
    fiber(stack_context, Fn &&fn) noexcept
        : fib_(detail::wrap_uboost_fn<std::decay_t<Fn>>::wrap(std::forward<Fn>(fn),
                                                              {& stop_state_})) {
        static_assert(is_fiber_invocable<Fn>, "Fn is not a fiber invocable");
    }

    ~fiber() {
        if (fib_) {
            try {
                std::move(fib_).resume_with(
                    [](boost::context::fiber &&fib) -> boost::context::fiber {
                        throw boost::context::detail::forced_unwind();
                    });
            }
            catch (boost::context::detail::forced_unwind &unwind) {
                unwind.caught = true;
            }
        }
    }

    fiber(fiber &&) = default;
    fiber &operator=(fiber &&) = default;

    fiber resume() && noexcept {
        try {
            return std::move(fib_).resume();
        }
        catch (boost::context::detail::forced_unwind &unwind) {
            unwind.caught = true;
            return boost::context::fiber();
        }
    }

    template <class Fn>
    fiber resume_with(Fn &&fn) && noexcept {
        try {
            return std::move(fib_).resume_with(
                detail::wrap_uboost_fn<std::decay_t<Fn>>::wrap(std::forward<Fn>(fn)));
        }
        catch (boost::context::detail::forced_unwind &unwind) {
            unwind.caught = true;
            return boost::context::fiber();
        }
    }

    void request_stop() noexcept {
        stop_state_.stopped_ = true;
    }

    explicit operator bool() const noexcept {
        return bool(fib_) && !stop_state_.stopped_;
    }

    bool operator!() const noexcept {
        return fib_.operator!();
    }

    void swap(fiber &other) noexcept {
        fib_.swap(other.fib_);
    }
};

void swap(fiber &lhs, fiber &rhs) {
    lhs.swap(rhs);
}

using fiber_context = fiber;
} // namespace context
} // namespace uboost

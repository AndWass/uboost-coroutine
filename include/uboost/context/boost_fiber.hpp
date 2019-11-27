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
    static auto wrap(Fn &&fn, stop_state* stopper) {
        return [f = std::forward<Fn>(fn),
                stopper](boost::context::fiber &&ctx) mutable -> boost::context::fiber {
            return f({stopper, std::move(ctx)}, stop_token(stopper)).fib_;
        };
    }
};

uboost::context::detail::stop_state* make_stop_state(stack_context sc) {
    std::uintptr_t ptr_val = reinterpret_cast<std::uintptr_t>(sc.sp) + sc.size;
    ptr_val -= sizeof(stop_state);
    ptr_val &= ~static_cast<std::uintptr_t>((alignof(stop_state)-1));
    return ::new(reinterpret_cast<void*>(ptr_val)) stop_state;
}
void handle_unwind_exception(boost::context::detail::forced_unwind &unwind)
{
    #ifndef BOOST_ASSERT_IS_VOID
    unwind.caught = true;
    #endif
}
} // namespace detail

class fiber
{
    template <class Fn>
    friend struct detail::wrap_uboost_fn;

    uboost::context::detail::stop_state* stop_state_ = nullptr;
    boost::context::fiber fib_;

    fiber(uboost::context::detail::stop_state* ss,
        boost::context::fiber &&fib) : stop_state_(ss), fib_(std::move(fib)) {
    }

public:
    template <class Fn, std::enable_if_t<is_fiber_invocable<Fn>>* = nullptr>
    fiber(stack_context stack, Fn &&fn) noexcept
        : stop_state_(detail::make_stop_state(stack)),
        fib_(detail::wrap_uboost_fn<std::decay_t<Fn>>::wrap(std::forward<Fn>(fn),
                                                              {stop_state_})) {
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
                detail::handle_unwind_exception(unwind);
            }
        }
    }

    fiber(fiber &&) = default;
    fiber &operator=(fiber &&) = default;

    fiber resume() && noexcept {
        try {
            return {stop_state_, std::move(fib_).resume()};
        }
        catch (boost::context::detail::forced_unwind &unwind) {
            detail::handle_unwind_exception(unwind);
            return {stop_state_, boost::context::fiber()};
        }
    }

    template <class Fn>
    fiber resume_with(Fn &&fn) && noexcept {
        try {
            return {stop_state_, std::move(fib_).resume_with(
                detail::wrap_uboost_fn<std::decay_t<Fn>>::wrap(std::forward<Fn>(fn), {stop_state_}))};
        }
        catch (boost::context::detail::forced_unwind &unwind) {
            detail::handle_unwind_exception(unwind);
            return {stop_state_, boost::context::fiber()};
        }
    }

    void request_stop() noexcept {
        stop_state_->stopped_ = true;
    }

    explicit operator bool() const noexcept {
        return bool(fib_) && !stop_state_->stopped_;
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

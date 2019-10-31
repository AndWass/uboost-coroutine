#pragma once

#include <uboost/context/fcontext.hpp>
#include <uboost/context/fiber_traits.hpp>
#include <uboost/context/stack_context.hpp>

#include <functional>
#include <type_traits>

namespace uboost
{
namespace context
{
class fiber;

namespace detail
{
extern "C" uboost::context::fcontext_t make_fcontext(void *, void (*)(transfer_t));
extern "C" transfer_t jump_fcontext(uboost::context::fcontext_t, void *);
extern "C" transfer_t ontop_fcontext(uboost::context::fcontext_t, void *,
                                     transfer_t (*)(transfer_t));

template <class Record>
transfer_t fiber_exit(transfer_t t) {
    Record *rec = static_cast<Record *>(t.data);
    rec->~Record();
    return {nullptr, nullptr};
}

template <class Record>
void fiber_entry(transfer_t trans) {
    Record *rec = static_cast<Record *>(trans.data);
    trans = detail::jump_fcontext(trans.fctx, nullptr);
    trans.fctx = rec->run(trans.fctx);
    detail::ontop_fcontext(trans.fctx, rec, &fiber_exit<Record>);
}

template <class Ctx, class Fn>
transfer_t fiber_ontop(transfer_t trans) {
    auto p = *static_cast<Fn *>(trans.data);
    trans.data = nullptr;
    Ctx ctx = std::invoke(p, Ctx{trans.fctx});
    return {std::exchange(trans.fctx, nullptr), nullptr};
}

template <class Ctx, class Fn>
class fiber_record
{
public:
    fiber_record(Fn &&fn) noexcept : fn_(std::forward<Fn>(fn)) {
    }

    fcontext_t run(fcontext_t invoker) noexcept {
        return std::invoke(fn_, Ctx{invoker}, stop_token{&stop_state_}).fctx_;
    }

    stop_token get_stop_token() {
        return stop_token{&stop_state_};
    }

private:
    std::decay_t<Fn> fn_;
    detail::stop_state stop_state_;
};

template <class Ctx, class Fn>
fcontext_t create_fiber1(stack_context stack, Fn &&fn, stop_token &stopper) {
    using fiber_context_t = fiber_record<Ctx, Fn>;
    auto fiber_context_start =
        (reinterpret_cast<std::uintptr_t>(stack.sp) + stack.size - sizeof(fiber_context_t));
    fiber_context_start &= ~(alignof(fiber_context_t) - 1);

    void *p = reinterpret_cast<void *>((fiber_context_start));

    fiber_context_t *fiber_ctx = ::new (p) fiber_context_t(std::forward<Fn>(fn));
    stopper = fiber_ctx->get_stop_token();
    fcontext_t coro_ctx = detail::make_fcontext(p, &fiber_entry<fiber_context_t>);
    return detail::jump_fcontext(coro_ctx, fiber_ctx).fctx;
}

} // namespace detail

class fiber
{
    stop_token stopper_;
    fcontext_t fctx_{nullptr};

    template <class Ctx, class Fn>
    friend class detail::fiber_record;

    fiber(fcontext_t fctx) : fctx_(fctx) {
    }

public:
    template <class Fn>
    fiber(stack_context stack, Fn &&fn) noexcept
        : fctx_(detail::create_fiber1<fiber, Fn>(stack, std::forward<Fn>(fn), stopper_)) {
        static_assert(is_fiber_invocable<std::decay_t<Fn>>,
                      "Function must have signature fiber(fiber&&, stop_token)");
    }

    fiber(const fiber &) noexcept = delete;
    fiber &operator=(const fiber &) noexcept = delete;

    fiber(fiber &&rhs) noexcept : fctx_(std::exchange(rhs.fctx_, nullptr)) {
    }
    fiber &operator=(fiber &&rhs) noexcept {
        if (&rhs != this) {
            swap(rhs);
        }

        return *this;
    }

    fiber resume() && noexcept {
        return {detail::jump_fcontext(std::exchange(fctx_, nullptr), nullptr).fctx};
    }

    template <class Fn>
    fiber resume_with(Fn &&fn) && noexcept {
        auto p = std::forward<Fn>(fn);
        return {detail::ontop_fcontext(std::exchange(fctx_, nullptr), std::addressof(p),
                                       &detail::fiber_ontop<fiber, std::decay_t<Fn>>)};
    }

    void request_stop() {
        stopper_.request_stop();
    }

    explicit operator bool() const noexcept {
        return fctx_ && !stopper_.is_stopped();
    }

    bool operator!() const noexcept {
        return !operator bool();
    }

    void swap(fiber &other) noexcept {
        std::exchange(fctx_, other.fctx_);
    }
};

void swap(fiber &lhs, fiber &rhs) {
    lhs.swap(rhs);
}

using fiber_context = fiber;

} // namespace context
} // namespace uboost

#pragma once

#include <uboost/coroutine/detail/pull_control_block_decl.hpp>

namespace uboost
{
namespace coroutine
{
namespace detail
{
template <class T>
template <class Fn>
pull_coroutine<T>::pull_control_block::pull_control_block(uboost::context::stack_context stack,
                                                          Fn &&fn) noexcept
    : fiber_(stack,
             [this, fn_ = std::forward<Fn>(fn)](uboost::context::fiber && fib,
                                                uboost::context::stop_token stop_token) mutable {
                 using pcb_t = typename push_coroutine<T>::push_control_block;
                 pcb_t pcb(std::move(fib), this);
                 this->other_ = &pcb;
                 push_coroutine<T> push(typename push_coroutine<T>::preallocated{}, &pcb);
                 auto fn = std::move(fn_);
                 fn_(push);
                 push.cb_ = nullptr;
                 return std::move(pcb.fiber_);
             }) {
}

template <class T>
pull_coroutine<T>::pull_control_block::~pull_control_block() noexcept {
    if (valid_) {
        value_ptr()->~T();
        if(other_) {
            // Need to reset the pointer to me
            other_->other_ = nullptr;
        }
    }
}

template <class Fn>
pull_coroutine<void>::pull_control_block::pull_control_block(uboost::context::stack_context stack,
                                                          Fn &&fn) noexcept
    : fiber_(stack,
             [this, fn_ = std::forward<Fn>(fn)](uboost::context::fiber && fib,
                                                uboost::context::stop_token stop_token) mutable {
                 using pcb_t = typename push_coroutine<void>::push_control_block;
                 pcb_t pcb(std::move(fib), this);
                 this->other_ = &pcb;
                 push_coroutine<void> push(typename push_coroutine<void>::preallocated{}, &pcb);
                 auto fn = std::move(fn_);
                 fn_(push);
                 push.cb_ = nullptr;
                 return std::move(pcb.fiber_);
             }) {
}

pull_coroutine<void>::pull_control_block::~pull_control_block() noexcept {
    if(other_) {
        // Need to reset the pointer to me
        other_->other_ = nullptr;
    }
}

}
}
}
#pragma once

#include <uboost/coroutine/detail/pull_coroutine.hpp>

#include <uboost/coroutine/detail/push_control_block_decl.hpp>
#include <uboost/coroutine/detail/pull_control_block_decl.hpp>

namespace uboost
{
namespace coroutine
{
namespace detail
{
template <class T>
push_coroutine<T>::push_control_block::push_control_block(
    uboost::context::fiber &&fib, typename pull_coroutine<T>::pull_control_block *other) noexcept
    : fiber_(std::move(fib)), other_(other) {
}

template <class T>
template <class Fn>
push_coroutine<T>::push_control_block::push_control_block(uboost::context::stack_context stack,
                                                          Fn &&fn) noexcept
    : fiber_(stack,
             [this, fn_ = std::forward<Fn>(fn)](uboost::context::fiber &&fib,
                                                uboost::context::stop_token stop_token) mutable {
                 using pcb_t = typename pull_coroutine<T>::pull_control_block;
                 pcb_t pcb(std::move(fib), this);
                 pull_coroutine<T> pull(typename pull_coroutine<T>::preallocated{}, &pcb);
                 auto fn = std::move(fn_);
                 this->other_ = &pcb;
                 pcb.fiber_ = std::move(pcb.fiber_).resume();
                 fn(pull);
                 pull.cb_ = nullptr;
                 return std::move(pcb.fiber_);
             }) {
    fiber_ = std::move(fiber_).resume();
}

template <class T>
push_coroutine<T>::push_control_block::~push_control_block() noexcept {
    if (other_) {
        // Reset the other pointer to me!
        other_->other_ = nullptr;
    }
}

// void specialization

push_coroutine<void>::push_control_block::push_control_block(
    uboost::context::fiber &&fib, typename pull_coroutine<void>::pull_control_block *other) noexcept
    : fiber_(std::move(fib)), other_(other) {
}

template <class Fn>
push_coroutine<void>::push_control_block::push_control_block(uboost::context::stack_context stack,
                                                          Fn &&fn) noexcept
    : fiber_(stack,
             [this, fn_ = std::forward<Fn>(fn)](uboost::context::fiber &&fib,
                                                uboost::context::stop_token stop_token) mutable {
                 using pcb_t = typename pull_coroutine<void>::pull_control_block;
                 pcb_t pcb(std::move(fib), this);
                 pull_coroutine<void> pull(typename pull_coroutine<void>::preallocated{}, &pcb);
                 auto fn = std::move(fn_);
                 this->other_ = &pcb;
                 pcb.fiber_ = std::move(pcb.fiber_).resume();
                 fn(pull);
                 pull.cb_ = nullptr;
                 return std::move(pcb.fiber_);
             }) {
    fiber_ = std::move(fiber_).resume();
}

push_coroutine<void>::push_control_block::~push_control_block() noexcept {
    if (other_) {
        // Reset the other pointer to me!
        other_->other_ = nullptr;
    }
}

}
}
}
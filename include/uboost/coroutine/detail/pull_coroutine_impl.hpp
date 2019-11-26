#pragma once

#include <uboost/coroutine/detail/pull_control_block_decl.hpp>
#include <uboost/coroutine/detail/pull_coroutine.hpp>
#include <uboost/coroutine/detail/push_control_block_decl.hpp>

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
                 fn(push);
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

template <class T>
template <class Fn>
pull_coroutine<T>::pull_coroutine(uboost::context::stack_context stack, Fn &&fn) noexcept {
    std::uintptr_t sp_ptr_val = reinterpret_cast<std::uintptr_t>(stack.sp);
    sp_ptr_val += stack.size - sizeof(pull_control_block);
    sp_ptr_val &= ~(alignof(pull_control_block) - 1);
    stack.size = sp_ptr_val - reinterpret_cast<std::uintptr_t>(stack.sp);
    auto *storage = reinterpret_cast<void *>(sp_ptr_val);
    cb_ = ::new (storage) pull_control_block(stack, std::move(fn));

    (*this)();
}

template <class T>
pull_coroutine<T>::~pull_coroutine() noexcept {
    if (cb_ != nullptr) {
        cb_->~pull_control_block();
    }
}

template <class T>
pull_coroutine<T> &pull_coroutine<T>::operator()() noexcept {
    cb_->fiber_ = std::move(cb_->fiber_).resume();
    return *this;
}

template <class T>
pull_coroutine<T>::operator bool() const noexcept {
    return bool(cb_->fiber_) && cb_->valid_ && cb_->other_;
}

template <class T>
T pull_coroutine<T>::get() noexcept {
    return *cb_->value_ptr();
}

template <class T>
void pull_coroutine<T>::stop() noexcept {
    if(cb_) {
        cb_->fiber_.request_stop();
        if(cb_->other_) {
            cb_->other_->stopped_ = true;
            cb_->fiber_ = std::move(cb_->fiber_).resume();
        }
    }
}
} // namespace detail
} // namespace coroutine
} // namespace uboost

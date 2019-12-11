#pragma once

#include <uboost/coroutine/detail/pull_control_block_decl.hpp>
#include <uboost/coroutine/detail/push_control_block_decl.hpp>
#include <uboost/coroutine/detail/push_coroutine.hpp>

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

template <class T>
template <class Fn>
push_coroutine<T>::push_coroutine(uboost::context::stack_context stack, Fn &&fn) noexcept {
    std::uintptr_t sp_ptr_val = reinterpret_cast<std::uintptr_t>(stack.sp);
    sp_ptr_val += stack.size - sizeof(push_control_block);
    sp_ptr_val &= ~(alignof(push_control_block) - 1);
    stack.size = sp_ptr_val - reinterpret_cast<std::uintptr_t>(stack.sp);
    auto *storage = reinterpret_cast<void *>(sp_ptr_val);
    cb_ = ::new (storage) push_control_block(stack, std::move(fn));
}

template <class T>
push_coroutine<T>::~push_coroutine() noexcept {
    if (cb_) {
        cb_->~push_control_block();
    }
}

template <class T>
bool push_coroutine<T>::is_valid() const noexcept {
    return bool(cb_->fiber_) && !cb_->stopped_;
}

template <class T>
push_coroutine<T>::operator bool() const noexcept {
    return is_valid();
}

template <class T>
bool push_coroutine<T>::operator!() const noexcept {
    return !is_valid();
}

template <class T>
push_coroutine<T> &push_coroutine<T>::operator()(const T &val) noexcept {
    if (cb_->other_->valid_) {
        cb_->other_->value_ptr()->~T();
    }
    ::new (cb_->other_->value_storage_) T(val);
    cb_->other_->valid_ = true;
    cb_->fiber_ = std::move(cb_->fiber_).resume();
    return *this;
}

template <class T>
push_coroutine<T> &push_coroutine<T>::operator()(T &&val) noexcept {
    if (cb_->other_->valid_) {
        cb_->other_->value_ptr()->~T();
    }
    ::new (cb_->other_->value_storage_) T(std::move(val));
    cb_->other_->valid_ = true;
    cb_->fiber_ = std::move(cb_->fiber_).resume();
    return *this;
}

template <class T>
void push_coroutine<T>::stop() noexcept {
    if (cb_) {
        cb_->fiber_.request_stop();
        if (cb_->other_->valid_) {
            cb_->other_->value_ptr()->~T();
            cb_->other_->valid_ = false;
        }
        cb_->fiber_ = std::move(cb_->fiber_).resume();
    }
}
} // namespace detail
} // namespace coroutine
} // namespace uboost

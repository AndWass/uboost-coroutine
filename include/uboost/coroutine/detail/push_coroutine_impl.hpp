#pragma once

#include <uboost/coroutine/detail/pull_control_block_decl.hpp>
#include <uboost/coroutine/detail/push_control_block_decl.hpp>
#include <uboost/coroutine/detail/push_coroutine.hpp>
#include <uboost/coroutine/detail/pull_coroutine.hpp>

#include <uboost/coroutine/detail/push_control_block_impl.hpp>

namespace uboost
{
namespace coroutine
{
namespace detail
{
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

// Void specialization

template <class Fn>
push_coroutine<void>::push_coroutine(uboost::context::stack_context stack, Fn &&fn) noexcept {
    std::uintptr_t sp_ptr_val = reinterpret_cast<std::uintptr_t>(stack.sp);
    sp_ptr_val += stack.size - sizeof(push_control_block);
    sp_ptr_val &= ~(alignof(push_control_block) - 1);
    stack.size = sp_ptr_val - reinterpret_cast<std::uintptr_t>(stack.sp);
    auto *storage = reinterpret_cast<void *>(sp_ptr_val);
    cb_ = ::new (storage) push_control_block(stack, std::move(fn));
}

push_coroutine<void>::~push_coroutine() noexcept {
    if (cb_) {
        cb_->~push_control_block();
    }
}

bool push_coroutine<void>::is_valid() const noexcept {
    return bool(cb_->fiber_) && !cb_->stopped_;
}

push_coroutine<void>::operator bool() const noexcept {
    return is_valid();
}

bool push_coroutine<void>::operator!() const noexcept {
    return !is_valid();
}

push_coroutine<void> &push_coroutine<void>::operator()() noexcept {
    cb_->fiber_ = std::move(cb_->fiber_).resume();
    return *this;
}

void push_coroutine<void>::stop() noexcept {
    if (cb_) {
        cb_->fiber_.request_stop();
        cb_->fiber_ = std::move(cb_->fiber_).resume();
    }
}

} // namespace detail
} // namespace coroutine
} // namespace uboost

#pragma once

#include <uboost/coroutine/detail/push_coroutine.hpp>

#include <uboost/coroutine/detail/pull_control_block_decl.hpp>
#include <uboost/coroutine/detail/push_control_block_decl.hpp>

#include <uboost/coroutine/detail/pull_control_block_impl.hpp>

namespace uboost
{
namespace coroutine
{
namespace detail
{
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

template<class T>
template<class Fn>
pull_coroutine<T> &pull_coroutine<T>::map_and_pull(Fn &&fn) noexcept {
    if(*this) {
        fn(get());
        (*this)();
    }
    return *this;
}

template <class T>
pull_coroutine<T> &pull_coroutine<T>::operator()() noexcept {
    cb_->fiber_ = std::move(cb_->fiber_).resume();
    return *this;
}

template<class T>
bool pull_coroutine<T>::is_valid() const noexcept {
    return bool(cb_->fiber_) && cb_->valid_ && cb_->other_;
}

template <class T>
pull_coroutine<T>::operator bool() const noexcept {
    return is_valid();
}

template<class T>
bool pull_coroutine<T>::operator!() const noexcept {
    return !is_valid();
}

template <class T>
T pull_coroutine<T>::get() const noexcept {
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

// void speciliazation

template <class Fn>
pull_coroutine<void>::pull_coroutine(uboost::context::stack_context stack, Fn &&fn) noexcept {
    std::uintptr_t sp_ptr_val = reinterpret_cast<std::uintptr_t>(stack.sp);
    sp_ptr_val += stack.size - sizeof(pull_control_block);
    sp_ptr_val &= ~(alignof(pull_control_block) - 1);
    stack.size = sp_ptr_val - reinterpret_cast<std::uintptr_t>(stack.sp);
    auto *storage = reinterpret_cast<void *>(sp_ptr_val);
    cb_ = ::new (storage) pull_control_block(stack, std::move(fn));

    (*this)();
}

pull_coroutine<void>::~pull_coroutine() noexcept {
    if (cb_ != nullptr) {
        cb_->~pull_control_block();
    }
}

pull_coroutine<void> &pull_coroutine<void>::operator()() noexcept {
    cb_->fiber_ = std::move(cb_->fiber_).resume();
    return *this;
}

bool pull_coroutine<void>::is_valid() const noexcept {
    return bool(cb_->fiber_) && cb_->other_;
}

pull_coroutine<void>::operator bool() const noexcept {
    return is_valid();
}

bool pull_coroutine<void>::operator!() const noexcept {
    return !is_valid();
}

void pull_coroutine<void>::stop() noexcept {
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

#pragma once

#include <iterator>
#include <type_traits>

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

    template <class Fn>
    pull_coroutine &map_and_pull(Fn &&fn) noexcept;
    // pull_coroutine(pull_coroutine &&) noexcept;
    pull_coroutine &operator()() noexcept;
    explicit operator bool() const noexcept;
    // bool operator!() const noexcept;
    T get() const noexcept;

    void stop() noexcept;

    class iterator;
    class const_iterator;

    class iterator
    {
        pull_coroutine *coro_ = nullptr;
        friend class const_iterator;

    public:
        using value_type = std::remove_reference_t<T>;
        using reference = value_type&;
        using pointer = value_type*;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;

        iterator() noexcept = default;
        explicit iterator(pull_coroutine &pc) noexcept : coro_(nullptr) {
            if (pc) {
                coro_ = &pc;
            }
        }

        reference operator*() const noexcept {
            return *coro_->cb_->value_ptr();
        }
        pointer operator->() const noexcept {
            return coro_->cb_->value_ptr();
        }

        iterator &operator++() noexcept {
            (*coro_)();
            if (!*coro_) {
                coro_ = nullptr;
            }
            return *this;
        }
        iterator operator++(int) noexcept {
            return ++(*this);
        }
        bool operator==(const iterator &rhs) const {
            return coro_ == rhs.coro_;
        }
        bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }
    };

    friend class iterator;

    iterator begin() noexcept {
        return iterator(*this);
    }

    iterator end() noexcept {
        return iterator();
    }
};
template <class T>
typename pull_coroutine<T>::iterator begin(pull_coroutine<T> &pc) noexcept {
    return pc.begin();
}

template <class T>
typename pull_coroutine<T>::iterator end(pull_coroutine<T> &pc) noexcept {
    return pc.end();
}
} // namespace detail
} // namespace coroutine
} // namespace uboost

#include <uboost/coroutine/detail/pull_coroutine_impl.hpp>

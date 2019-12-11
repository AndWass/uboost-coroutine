#pragma once

#include <uboost/context/stack_context.hpp>

namespace uboost
{
namespace coroutine
{
namespace detail
{
template <class T>
class push_coroutine
{
    template <class U>
    friend class pull_coroutine;

    struct push_control_block;

    push_control_block *cb_;

    struct preallocated
    {
    };

    push_coroutine(preallocated, push_control_block *cb) noexcept : cb_(cb) {
    }

public:
    template <typename Fn>
    push_coroutine(uboost::context::stack_context, Fn &&) noexcept;

    ~push_coroutine() noexcept;

    push_coroutine(push_coroutine const &) = delete;
    push_coroutine &operator=(push_coroutine const &) = delete;

    // push_coroutine(push_coroutine &&) noexcept;
    push_coroutine &operator()(T const &) noexcept;
    push_coroutine &operator()(T &&) noexcept;

    bool is_valid() const noexcept;
    explicit operator bool() const noexcept;
    bool operator!() const noexcept;

    void stop() noexcept;

    class iterator
    {
        push_coroutine *coro_ = nullptr;

    public:
        using value_type = void;
        using reference = void;
        using pointer = void;
        using difference_type = void;
        using iterator_category = std::output_iterator_tag;

        constexpr iterator() noexcept = default;
        explicit iterator(push_coroutine &pc) noexcept : coro_(&pc) {
        }

        iterator operator*() noexcept {
            return *this;
        }
        iterator &operator=(T const &v) noexcept {
            if (!(*coro_)(v)) {
                coro_ = nullptr;
            }
            return *this;
        }
        iterator &operator=(T &&v) noexcept {
            if (!(*coro_)(std::move(v))) {
                coro_ = nullptr;
            }
            return *this;
        }

        iterator &operator++() noexcept {
            return *this;
        }

        iterator operator++(int) noexcept {
            return *this;
        }

        bool operator==(const iterator &rhs) const noexcept {
            return coro_ == rhs.coro_;
        }
        bool operator!=(const iterator &rhs) const noexcept {
            return !(*this == rhs);
        }
    };
    iterator begin() noexcept {
        return iterator(*this);
    }

    iterator end() noexcept {
        return iterator();
    }
};
template <class T>
typename push_coroutine<T>::iterator begin(push_coroutine<T> &pc) noexcept {
    return pc.begin();
}

template <class T>
typename push_coroutine<T>::iterator end(push_coroutine<T> &pc) noexcept {
    return pc.end();
}
} // namespace detail
} // namespace coroutine
} // namespace uboost

#include <uboost/coroutine/detail/push_coroutine_impl.hpp>

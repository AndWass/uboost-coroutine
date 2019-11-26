#pragma once

#if defined(UBOOST_USE_BOOST)
#include <boost/context/stack_context.hpp>
#endif

#include <cstdint>
#include <array>

namespace uboost
{
namespace context
{
struct stack_context
{
    std::size_t size = 0;
    void *sp = nullptr;

    stack_context() noexcept = default;
    template<class T, std::size_t N>
    stack_context(T (&sp)[N]) noexcept: size(N*sizeof(T)), sp(sp) {}
    template<class T, std::size_t N>
    stack_context(std::array<T, N> &arr): size(N*sizeof(T)), sp(arr.data()) {}
    stack_context(std::size_t size, void *sp) noexcept : size(size), sp(sp) {}

#if defined(UBOOST_USE_BOOST)
    operator boost::context::stack_context() noexcept
    {
        return boost::context::stack_context{size, sp};
    }
#endif
};
} // namespace context
} // namespace uboost

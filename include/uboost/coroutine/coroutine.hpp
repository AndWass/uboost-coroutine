#include <uboost/coroutine/detail/pull_coroutine.hpp>
#include <uboost/coroutine/detail/push_coroutine.hpp>

namespace uboost
{
namespace coroutine
{
template <class T>
struct coroutine
{
    using push_type = detail::push_coroutine<T>;
    using pull_type = detail::pull_coroutine<T>;
};
} // namespace coroutine
} // namespace uboost

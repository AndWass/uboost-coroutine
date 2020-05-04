#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <uboost/context/fiber.hpp>

TEST_CASE("fiber_single_resume")
{
    std::uint32_t stack[128];
    bool called = false;
    uboost::context::fiber fib(
        stack, [&called](uboost::context::fiber &&fib, uboost::context::stop_token st) {
            called = true;
            return std::move(fib);
        });

    REQUIRE_FALSE(called);
    fib = std::move(fib).resume();
    REQUIRE(called);
    REQUIRE_FALSE(bool(fib));
}

TEST_CASE("fiber_stop_token_propogated")
{
    std::uint32_t stack[128];
    bool stop_set = false;
    uboost::context::fiber fib(stack, [&stop_set](uboost::context::fiber &&fib, uboost::context::stop_token st) {
        stop_set = st.is_stopped();
        return std::move(fib);
    });

    REQUIRE_FALSE(stop_set);
    fib.request_stop();
    fib = std::move(fib).resume();
    REQUIRE(stop_set);
    REQUIRE_FALSE(bool(fib));
}

TEST_CASE("fiber_reentry")
{
    std::uint32_t stack[128];
    int sum = 0;
    uboost::context::fiber fib(stack, [&sum](uboost::context::fiber &&fib, uboost::context::stop_token st) {
        sum++;
        fib = std::move(fib).resume();
        sum++;
        fib = std::move(fib).resume();
        sum++;
        fib = std::move(fib).resume();
        sum++;
        return std::move(fib);
    });

    int i=0;
    while(fib)
    {
        REQUIRE(sum == i);
        fib = std::move(fib).resume();
        i++;
    }
    REQUIRE(sum == 4);
}

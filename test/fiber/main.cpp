#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <uboost/context/fiber.hpp>

TEST_CASE("Single resume calls fiber function") {
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

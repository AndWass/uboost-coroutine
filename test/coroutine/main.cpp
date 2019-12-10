#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <uboost/coroutine/coroutine.hpp>

TEST_CASE("single resume") {
    std::uint32_t stack[128];
    uboost::coroutine::coroutine<int>::pull_type puller(stack, [](auto &pusher) { pusher(10); });
    REQUIRE(puller);
    REQUIRE_EQ(puller.get(), 10);
    puller();
    REQUIRE_FALSE(puller);
}

TEST_CASE("multiple resume") {
    std::uint32_t stack[128];
    uboost::coroutine::coroutine<int>::pull_type puller(stack, [](auto &pusher) {
        for (int i = 0; i < 10; i++) {
            pusher(i);
        }
    });
    int i = 0;
    for (i = 0; puller; i++) {
        REQUIRE(puller);
        REQUIRE_EQ(puller.get(), i);
        puller();
    }
    REQUIRE_EQ(i, 10);
    REQUIRE_FALSE(puller);
}

TEST_CASE("pull_coroutine: iteration with iterators") {
    std::uint32_t stack[128];
    uboost::coroutine::coroutine<int>::pull_type puller(stack, [](auto &pusher) {
        for (int i = 0; i < 10; i++) {
            pusher(i);
        }
    });
    REQUIRE(puller);
    SUBCASE("pull_coroutine: explicit iteration") {
        int i = 0;
        for (auto iter = puller.begin(); iter != puller.end(); iter++) {
            CHECK_EQ(*iter, i);
            i++;
        }
        CHECK_EQ(i, 10);
        CHECK_FALSE(puller);
    }
    SUBCASE("pull_coroutine: ADL") {
        using std::begin;
        using std::end;
        int i = 0;

        for (auto iter = begin(puller); iter != end(puller); iter++) {
            CHECK_EQ(*iter, i);
            i++;
        }
        CHECK_EQ(i, 10);
        CHECK_FALSE(puller);
    }
    SUBCASE("pull_coroutine: rangebased for-loop") {
        int i = 0;
        for (auto v : puller) {
            CHECK_EQ(v, i);
            i++;
        }
        CHECK_EQ(i, 10);
        CHECK_FALSE(puller);
    }
}

TEST_CASE("pull_coroutine: dereference iterator") {
    struct test
    {
        int a;
    };

    std::uint32_t stack[128];
    uboost::coroutine::coroutine<test>::pull_type puller(stack, [](auto &pusher) {
        for (int i = 0; i < 10; i++) {
            pusher(test{i});
        }
    });
    REQUIRE(puller);
    int i = 0;
    for (auto iter = puller.begin(); iter != puller.end(); iter++) {
        CHECK_EQ(iter->a, i);
        i++;
    }
    CHECK_EQ(i, 10);
    CHECK_FALSE(puller);
}

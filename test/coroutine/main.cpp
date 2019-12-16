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
    SUBCASE("pull_coroutine: ADL begin and end") {
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

TEST_CASE("push_coroutine: explicit output iterator") {
    std::uint32_t stack[128];
    uboost::coroutine::coroutine<int>::pull_type puller(stack, [](auto &pusher) {
        auto out = pusher.begin();
        for (int i = 0; i < 10; i++) {
            *out++ = i;
        }
    });

    int i = 0;
    for (auto v : puller) {
        CHECK_EQ(v, i);
        i++;
    }
    CHECK_EQ(i, 10);
    CHECK_FALSE(puller);
}

TEST_CASE("push_coroutine: copy algorithm") {
    std::uint32_t stack[128];
    uboost::coroutine::coroutine<char>::pull_type puller(stack, [](auto &pusher) {
        std::string str = "hello world";
        std::copy(str.begin(), str.end(), pusher.begin());
    });
    std::string str(puller.begin(), puller.end());
    REQUIRE_EQ(str, "hello world");
}

TEST_CASE("push_coroutine: ADL begin") {
    std::uint32_t stack[128];
    uboost::coroutine::coroutine<char>::pull_type puller(stack, [](auto &pusher) {
        std::string str = "hello world";
        std::copy(str.begin(), str.end(), begin(pusher));
    });
    std::string str(puller.begin(), puller.end());
    REQUIRE_EQ(str, "hello world");
}

TEST_CASE("pull_coroutine: is_valid")
{
    std::uint32_t stack[128];
    uboost::coroutine::coroutine<int>::pull_type puller(stack, [](auto &pusher) { pusher(10); });
    REQUIRE(puller);
    REQUIRE(puller.is_valid());
    REQUIRE(!!puller);
    puller();
    REQUIRE_FALSE(puller);
    REQUIRE_FALSE(puller.is_valid());
    REQUIRE_FALSE(!!puller);
    REQUIRE(!puller);
}

TEST_CASE("push_coroutine: is_valid")
{
    std::uint32_t stack[128];
    uboost::coroutine::coroutine<int>::push_type pusher(stack, [](auto &puller) { });
    REQUIRE(pusher);
    REQUIRE(pusher.is_valid());
    REQUIRE(!!pusher);
    pusher(0);
    REQUIRE_FALSE(pusher);
    REQUIRE_FALSE(pusher.is_valid());
    REQUIRE_FALSE(!!pusher);
    REQUIRE(!pusher);
}

TEST_CASE("coroutine: void values")
{
    std::uint32_t stack[128];
    bool reentry = false;
    uboost::coroutine::coroutine<void>::pull_type puller(stack, [&reentry](auto &pusher) { pusher(); reentry = true; });
    REQUIRE(puller);
    puller();
    REQUIRE_FALSE(puller);
    REQUIRE(reentry);
}

TEST_CASE("coroutine: stopping void coroutine")
{
    std::uint32_t stack[128];
    bool reentry = false;
    uboost::coroutine::coroutine<void>::pull_type puller(stack, [&reentry](auto &pusher) {
        pusher();
        if(!pusher) {
            return;
        }
        reentry = true;
    });
    REQUIRE(puller);
    puller.stop();
    REQUIRE_FALSE(puller);
    REQUIRE_FALSE(reentry);
}
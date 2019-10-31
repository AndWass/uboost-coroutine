//#include <functional>

#include <uboost/coroutine/coroutine.hpp>

//#include <iostream>
//#include <thread>

static std::uint32_t stack2[128];

using coro_t = uboost::coroutine::coroutine<int>;

struct test_type
{
    ~test_type() {
        // std::cout << "test_type destructor" << std::endl;
    }
};

int a = 0;
int b = 0;

static void coro(coro_t::push_type &test) {
    test_type destroyed_at_coro_end;
    for (int i = 0; i < 5; i++) {
        test(i);
    }
}

static void coro2(coro_t::pull_type &test) {
    while (test) {
        b += test.get();
        test();
    }
}

int main() {
    {
        coro_t::pull_type coro_fun({128 * sizeof(stack2[0]), stack2}, coro);
        while (coro_fun) {
            // std::cout << coro_fun.get() << std::endl;
            a += coro_fun.get();
            coro_fun();
        }
    }
    {
        coro_t::push_type coro_fun({128 * sizeof(stack2[0]), stack2}, coro2);
        for (int i = 10; i < 20; i++) {
            coro_fun(i);
        }
        coro_fun.stop();
    }
    while (true) {
    }
}

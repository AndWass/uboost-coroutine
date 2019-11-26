#include <iostream>

#include <uboost/coroutine/coroutine.hpp>

using coro_t = uboost::coroutine::coroutine<int>;

void fibonacci(coro_t::push_type &yielder)
{
    int n2 = 0;
    int n1 = 1;
    yielder(n2);
    yielder(n1);
    while(yielder)
    {
        yielder(n1+n2);
        n2 = std::exchange(n1, n1 + n2);
    }
    std::cout << "Stopping fibonacci\n";
}

int main() {
    std::uint32_t stack[128];
    coro_t::pull_type coro(stack, fibonacci);
    for(int i=0; i<10; i++)
    {
        coro.map_and_pull([](auto i) {
            std::cout << i << "\n";
        });
    }
    coro.stop();
}
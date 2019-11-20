#include <uboost/context/fiber.hpp>
#include <iostream>

uboost::context::fiber fib_fun(uboost::context::fiber &&fib, uboost::context::stop_token st)
{
    std::cout << "fib_fun 1\n";
    fib = std::move(fib).resume();
    if(st) {
        std::cout << "stop 1\n";
        return std::move(fib);
    }
    
    std::cout << "fib_fun 2\n";
    
    fib = std::move(fib).resume();
    if(st) {
        std::cout << "stop 2\n";
        return std::move(fib);
    }

    std::cout << "fib_fun 3\n";
    fib = std::move(fib).resume();
    std::cout << "exiting fib_fun\n";
    return std::move(fib);
}

uboost::context::fiber top_fun(uboost::context::fiber &&fib, uboost::context::stop_token st) {
    std::cout << "top_fun\n";
    fib = std::move(fib).resume();
    std::cout << "top_fun 2\n";
    return std::move(fib);
}

int main()
{
    std::uint32_t stack[128];
    uboost::context::fiber fib({128 * 4, stack}, fib_fun);
    int i=1;
    while(fib) {
        std::cout << "main " << i++ << "\n";
        fib = std::move(fib).resume();
    }
    
    fib = uboost::context::fiber({128 * 4, stack}, fib_fun);
    fib = std::move(fib).resume();
    fib = std::move(fib).resume_with(top_fun);
    fib = std::move(fib).resume();
    fib.request_stop();
    fib = std::move(fib).resume();
    std::cout << "fiber::operator bool() = " << bool(fib) << "\n";
}

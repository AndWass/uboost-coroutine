add_executable(test-coroutine main.cpp)
target_link_libraries(test-coroutine CONAN_PKG::doctest
    uboost::coroutine)

add_test(NAME coroutine COMMAND test-coroutine)
add_executable(sample-fibonacci fibonacci.cpp)
target_link_libraries(sample-fibonacci uboost::coroutine)
cmake_minimum_required(VERSION 3.5)

project(uboost-coro LANGUAGES ASM C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(uboost-coro
    sample/main.cpp

    include/uboost/context/stack_context.hpp
    include/uboost/context/fiber_traits.hpp
    include/uboost/context/fiber.hpp
    include/uboost/context/uboost_fiber.hpp
    include/uboost/context/boost_fiber.hpp
    include/uboost/context/fcontext.hpp
    include/uboost/context/stop_token.hpp

    include/uboost/coroutine/coroutine.hpp
    include/uboost/coroutine/detail/push_coroutine.hpp
    include/uboost/coroutine/detail/pull_coroutine.hpp

    include/uboost/coroutine/detail/push_coroutine_impl.hpp
    include/uboost/coroutine/detail/pull_coroutine_impl.hpp

    include/uboost/coroutine/detail/push_control_block_decl.hpp
    include/uboost/coroutine/detail/pull_control_block_decl.hpp
    )

message(${CMAKE_SYSTEM_NAME})

if(CMAKE_SYSTEM_NAME STREQUAL "Generic")
    add_library(platform STATIC
        src/make_arm_none_cortexm3_nofp.S
        src/jump_arm_none_cortexm3_nofp.S
        /opt/cross/targets/stellaris/lm3s811/startup.c
        /opt/cross/targets/stellaris/lm3s811/system_init.c
        )

    target_link_libraries(uboost-coro PUBLIC platform)

    set(LINKER_SCRIPT "/opt/cross/targets/atmel/atsam4sd32c/linker.ld")
    #set(LINKER_SCRIPT "/opt/cross/targets/stellaris/lm3s811/linker.ld")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${LINKER_SCRIPT}")
else()
    find_package(Boost REQUIRED COMPONENTS context)
    target_link_libraries(uboost-coro PUBLIC Boost::context)
    target_compile_definitions(uboost-coro PUBLIC UBOOST_USE_BOOST)
endif()

target_include_directories(uboost-coro PUBLIC include)

cmake_minimum_required(VERSION 3.8)

project(uboost_coroutine CXX ASM)

include(GNUInstallDirs)

set(UBOOST_CORO_USE_CONAN OFF)
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
    set(UBOOST_CORO_USE_CONAN ON)
endif()

set(UBOOST_CORO_IMPL "" CACHE STRING "Fiber implementation to use")
option(UBOOST_CORO_BUILD_SAMPLES "Build samples")
option(UBOOST_CORO_BUILD_TESTS "Build tests")

if(UBOOST_CORO_IMPL STREQUAL "boost")
    add_library(${PROJECT_NAME} INTERFACE)
    target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

    if(UBOOST_CORO_USE_CONAN)
        target_link_libraries(${PROJECT_NAME} INTERFACE CONAN_PKG::boost)
    else()
        find_package(Boost REQUIRED COMPONENTS context)
        target_link_libraries(${PROJECT_NAME} INTERFACE Boost::context)
    endif()
    target_compile_definitions(${PROJECT_NAME} INTERFACE UBOOST_USE_BOOST)
    target_include_directories(${PROJECT_NAME} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
elseif(NOT "${UBOOST_CORO_IMPL}" STREQUAL "")
    add_library(${PROJECT_NAME} STATIC
        src/make_${UBOOST_CORO_IMPL}.S
        src/jump_${UBOOST_CORO_IMPL}.S
    )
    target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
    set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
else()
    message(FATAL_ERROR "Either UBOOST_CORO_IMPL must be set. Valid values are 'boost' and 'aarch32_aapcs_nofp'")
endif()
add_library(uboost::coroutine ALIAS ${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}_targets
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT ${PROJECT_NAME}_targets
    DESTINATION ${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}
    NAMESPACE uboost::
    FILE ${PROJECT_NAME}-config.cmake
)

if(UBOOST_CORO_BUILD_SAMPLES)
    add_subdirectory(samples)
endif()

if(UBOOST_CORO_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
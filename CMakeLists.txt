cmake_minimum_required(VERSION 3.8)

project(uboost_coroutine CXX ASM)

include(GNUInstallDirs)

if(UBOOST_CORO_USE_BOOST OR UBOOST_CORO_IMPL STREQUAL "boost")
    add_library(${PROJECT_NAME} INTERFACE)
    target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

    find_package(Boost REQUIRED COMPONENTS context)
    target_link_libraries(${PROJECT_NAME} INTERFACE Boost::context)
    target_compile_definitions(${PROJECT_NAME} INTERFACE UBOOST_USE_BOOST)
    target_include_directories(${PROJECT_NAME} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
elseif(UBOOST_CORO_IMPL)
    add_library(${PROJECT_NAME} STATIC
        src/make_${UBOOST_CORO_IMPL}.S
        src/make_${UBOOST_CORO_IMPL}.S
    )
    target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
else()
    message(FATAL_ERROR "Either UBOOST_CORO_USE_BOOST or UBOOST_CORO_IMPL must be set")
endif()
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

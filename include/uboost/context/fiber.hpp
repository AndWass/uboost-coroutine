#pragma once

#ifdef UBOOST_CORO_USE_BOOST
#include <uboost/context/boost_fiber.hpp>
#else
#include <uboost/context/uboost_fiber.hpp>
#endif

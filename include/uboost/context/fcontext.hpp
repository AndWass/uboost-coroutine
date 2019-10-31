#pragma once

namespace uboost
{
namespace context
{
using fcontext_t = void *;
struct transfer_t
{
    fcontext_t fctx;
    void *data;
};
} // namespace context
} // namespace uboost

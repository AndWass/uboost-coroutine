#pragma once

namespace uboost
{
namespace context
{
namespace detail
{
struct stop_state
{
    bool stopped_ = false;
};
} // namespace detail

class stop_token
{
public:
    stop_token() noexcept = default;
    stop_token(detail::stop_state *ss_) noexcept : state_(ss_) {
    }

    bool is_stopped() const noexcept {
        return state_->stopped_;
    }

    void request_stop() noexcept {
        state_->stopped_ = true;
    }

    explicit operator bool() const noexcept {
        return is_stopped();
    }

private:
    detail::stop_state *state_ = nullptr;
};
} // namespace context
} // namespace uboost

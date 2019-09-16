#pragma once

namespace xutils
{

// Provides functionality for enqueuing multiple timers each with different
// timeout.
//
// The queue currently doesn't support canceling a particular timer
// because this:
// 1. Is not needed at the moment.
// 2. Would complicate the implementation.
//
// I deliberately chose to crash the application in case of timer failure
// because of two reasons:
// 1. It's very hard, in my experience and in almost all of the cases,
// for the application to continue to work correctly if some timer doesn't
// work.
// 2. I've never seen, in my practice so far (~10 years), a timer error when
// the timer is used correctly.
class timer_queue
{
public:
    using timer_impl    = baio::steady_timer;
    using clock_type    = timer_impl::clock_type;
    using duration      = timer_impl::duration;
    using time_point    = timer_impl::time_point;
    using executor_type = timer_impl::executor_type;

private:
    using expiry_point_t = time_point;
    using callback_t     = xlib::inplace_fn<32, void()>;

    bcont::flat_multimap<expiry_point_t, callback_t> entries_;
    timer_impl timer_;

public:
    explicit timer_queue(boost::asio::io_context& io_ctx) noexcept;
    ~timer_queue() noexcept;
    timer_queue(timer_queue&&) noexcept;
    timer_queue& operator=(timer_queue&&) noexcept;

    timer_queue(const timer_queue&) = delete;
    timer_queue& operator=(const timer_queue&) = delete;

    template <typename Callback>
    void enqueue_callback(duration expiry, Callback&& cb) noexcept;

    // Removes all currently enqueued timer callbacks without calling them.
    void remove_all() noexcept;

private:
    void set_timer() noexcept;
};

template <typename Callback>
void timer_queue::enqueue_callback(duration expiry, Callback&& cb) noexcept
{
    // The case when the expiry is <= 0 is implicitly handled. The timer will
    // just expiry immediately. This case should be very rare in practice, if
    // happens at all. Thus I decided to not handle it explicitly.
    const auto expiry_point = clock_type::now() + expiry;
    const bool set_tmr =
        (entries_.empty() || (expiry_point < entries_.begin()->first));
    entries_.emplace(expiry_point, std::forward<Callback>(cb));
    if (set_tmr)
        set_timer();
}

} // namespace xutils

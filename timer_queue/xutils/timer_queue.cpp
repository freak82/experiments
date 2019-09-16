#include "precompiled.h"
#include "xutils/timer_queue.h"

namespace xutils
{

timer_queue::timer_queue(boost::asio::io_context& io_ctx) noexcept
: timer_{io_ctx}
{
}

timer_queue::~timer_queue() noexcept = default;

timer_queue::timer_queue(timer_queue&& rhs) noexcept
: entries_{std::move(rhs.entries_)}, timer_{std::move(rhs.timer_)}
{
}

timer_queue& timer_queue::operator=(timer_queue&& rhs) noexcept
{
    if (X3ME_LIKELY(this != &rhs))
    {
        entries_ = std::move(rhs.entries_);
        timer_   = std::move(rhs.timer_);
    }
    return *this;
}

void timer_queue::remove_all() noexcept
{
    entries_.clear();
    // The call is noexcept. The below call may throw.
    // Let's die if this throw. I haven't seen such thing in practice.
    timer_.cancel();
}

template <typename TimePoint, typename Entries>
static void fire_expired(TimePoint now, Entries& ents) noexcept
{
    // Firing a callback may provoke adding new entry to the collection.
    // This we first move the expired callbacks to another collection and then
    // fire the callbacks.
    using callback_t = typename Entries::value_type::second_type;
    bcont::small_vector<callback_t, 8> cbs;
    // Note that the entries are sorted by the expiry point and thus we should
    // stop traversing the collection once we find that the entries are past
    // the 'now' point.
    for (auto it = ents.begin(); (it != ents.end()) && (it->first <= now);)
    {
        cbs.push_back(it->second);
        it = ents.erase(it);
    }
    for (auto& cb : cbs)
        cb();
}

void timer_queue::set_timer() noexcept
{
    X3ME_ASSERT(!entries_.empty());
    const auto expiry_point = entries_.begin()->first;
    // This call may throw and our function is noexcept.
    // I haven't seen timers throwing in practice and thus decided to not
    // convolute the code here. Let's crash, if it throws.
    timer_.expires_at(expiry_point);
    timer_.async_wait([this](const bsys::error_code& ec) {
        if (X3ME_LIKELY(!ec))
        {
            fire_expired(clock_type::now(), entries_);
            if (!entries_.empty())
                set_timer();
        }
        else if (ec == baio::error::operation_aborted)
        {
            // The timer has been reset or all entries have been removed
            if (!entries_.empty())
                set_timer();
        }
        else
        {
            // Haven't seen any other errors happen with timers in practice.
            // Thus instead of convolute the code and do desperate things here
            // I decided to kill the application.
            std::fprintf(stderr, "Timer failure. %s\n", ec.message().c_str());
            std::abort();
        }
    });
}

} // namespace xutils

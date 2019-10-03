#pragma once

namespace xutils
{

template <typename Exception, typename... Args>
inline void do_throw(const char* fmt_str, Args&&... args)
{
    if constexpr (sizeof...(args) > 0)
    {
        fmt::memory_buffer mb;
        fmt::format_to(mb, fmt_str, args...);
        mb.push_back(0); // So data() is NULL terminated
        throw Exception(mb.data());
    }
    else
        throw Exception(fmt_str);
}

} // namespace xutils

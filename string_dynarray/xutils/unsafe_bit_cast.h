#pragma once

namespace xutils
{

template <typename T>
static T unsafe_bit_cast(const char* p) noexcept
{
    static_assert(std::is_trivially_copyable_v<T>);
    T ret;
    ::memcpy(&ret, p, sizeof(ret));
    return ret;
}

} // namespace xutils

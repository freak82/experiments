#include "precompiled.h"
#include "xutils/string_dynarray.h"
#include "xutils/unsafe_bit_cast.h"

namespace xutils
{

string_dynarray::string_dynarray(xlib::span<const std::string> strs) noexcept
{
    if (!strs.empty())
    {
        // Calculate the full size of the block buffer.
        // We add one more index to put there the end offset of the last string.
        const uint32_t hdr_size = (strs.size() + 1) * sizeof(uint32_t);
        const uint32_t size     = std::accumulate(
            strs.begin(), strs.end(), hdr_size,
            [](size_t sum, const std::string& s) { return sum + s.size(); });
        auto data = boost::make_shared_noinit<char[]>(size);
        auto* d   = data.get();

        // Now copy the strings at their positions.
        uint32_t len = 0;
        for (uint32_t idx = 0; idx < strs.size(); ++idx)
        {
            const auto& str        = strs[idx];
            const uint32_t idx_pos = idx * sizeof(uint32_t);
            const uint32_t str_pos = hdr_size + len;
            ::memcpy(&d[idx_pos], &str_pos, sizeof(str_pos));
            ::memcpy(&d[str_pos], str.data(), str.size());
            len += str.size();
        }
        X3ME_ENFORCE((hdr_size + len) == size);
        const uint32_t end_idx_pos = strs.size() * sizeof(uint32_t);
        const uint32_t end_str_pos = hdr_size + len;
        ::memcpy(&d[end_idx_pos], &end_str_pos, sizeof(end_str_pos));

        // Finally fill the member variables
        data_ = std::move(data);
        cnt_  = strs.size();
    }
}

std::string_view string_dynarray::operator[](size_t idx) const noexcept
{
    X3ME_ASSERT(idx < cnt_);
    // The offsets to the strings are stored as uint32_t in the data_ member.
    // The number of the offsets is always cnt_ + 1.
    const auto pos0 = idx * sizeof(uint32_t);
    const auto pos1 = (idx + 1) * sizeof(uint32_t);
    const auto off0 = unsafe_bit_cast<uint32_t>(&data_[pos0]);
    const auto off1 = unsafe_bit_cast<uint32_t>(&data_[pos1]);
    const auto data = data_.get() + off0;
    const auto size = off1 - off0;
    return {data, size};
}

} // namespace xutils

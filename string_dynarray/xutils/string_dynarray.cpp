#include "precompiled.h"
#include "xutils/string_dynarray.h"

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

} // namespace xutils

#include "xutils/string_dynarray.h"

namespace xutils
{
// clang-format off
inline string_dynarray::
iterator::iterator(const string_dynarray* parent, size_t idx) noexcept
: parent_{parent}, idx_{idx}
{
}

inline string_dynarray::iterator::value_type
string_dynarray::iterator::operator*() const noexcept
{
    return (*parent_)[idx_];
}

inline
string_dynarray::iterator& string_dynarray::iterator::operator++() noexcept
{
    ++idx_;
    return *this;
}

inline
string_dynarray::iterator& string_dynarray::iterator::operator--() noexcept
{
    --idx_;
    return *this;
}

inline
string_dynarray::iterator string_dynarray::iterator::operator++(int) noexcept
{
    auto ret = *this;
    ++idx_;
    return ret;
}

inline
string_dynarray::iterator string_dynarray::iterator::operator--(int) noexcept
{
    auto ret = *this;
    --idx_;
    return ret;
}

inline string_dynarray::iterator& 
string_dynarray::iterator::operator+=(difference_type v) noexcept
{
    idx_ += v;
    return *this;
}

inline string_dynarray::iterator& 
string_dynarray::iterator::operator-=(difference_type v) noexcept
{
    idx_ -= v;
    return *this;
}

inline string_dynarray::iterator::difference_type 
string_dynarray::iterator::operator-(iterator rhs) noexcept
{
    return static_cast<difference_type>(idx_) - 
           static_cast<difference_type>(rhs.idx_);
}

inline bool string_dynarray::iterator::operator==(const iterator& rhs) noexcept
{
    return (parent_ == rhs.parent_) && (idx_ == rhs.idx_);
}

inline bool string_dynarray::iterator::operator!=(const iterator& rhs) noexcept
{
    return !(*this == rhs);
}

////////////////////////////////////////////////////////////////////////////////

inline string_dynarray::string_dynarray() noexcept = default;
inline string_dynarray::~string_dynarray() noexcept = default;
inline 
string_dynarray::string_dynarray(const string_dynarray&) noexcept = default;
inline string_dynarray& 
string_dynarray::operator=(const string_dynarray&) noexcept = default;

inline string_dynarray::string_dynarray(string_dynarray&& rhs) noexcept
: data_{std::move(rhs.data_)}, cnt_{std::exchange(rhs.cnt_, 0)}
{
}

inline
string_dynarray& string_dynarray::operator=(string_dynarray& rhs) noexcept
{
    if (X3ME_LIKELY(this != &rhs))
    {
        data_ = std::move(rhs.data_);
        cnt_  = std::exchange(rhs.cnt_, 0);
    }
    return *this;
}

inline void string_dynarray::swap(string_dynarray& rhs) noexcept
{
    data_.swap(rhs.data_);
    std::swap(cnt_, rhs.cnt_);
}

inline string_dynarray::iterator string_dynarray::begin() const noexcept
{
    return {this, 0U};
}

inline string_dynarray::iterator string_dynarray::end() const noexcept
{
    return {this, cnt_};
}

inline size_t string_dynarray::size() const noexcept
{
    return cnt_;
}

// clang-format on
} // namespace xutils

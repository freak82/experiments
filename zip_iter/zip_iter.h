#pragma once

#include <iterator>
#include <tuple>
#include <utility>

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/tuple.hpp>

template <typename It>
class checked_iter
{
    It it_;
    const It end_;

public:
    using value_type        = std::iterator_traits<It>::value_type;
    using difference_type   = std::iterator_traits<It>::difference_type;
    using reference         = std::iterator_traits<It>::reference;
    using pointer           = std::iterator_traits<It>::pointer;
    using iterator_category = std::iterator_traits<It>::iterator_category;

public:
    checked_iter(It it, It end) : it_{it}, end_{end} {}

    It end() const { return end_; }
    bool valid() const { return (it_ != end_); }

    reference operator*() { return *it_; }
    checked_iter& operator++()
    {
        ++it_;
        return *this;
    }
    checked_iter operator++(int)
    {
        auto ret = *this;
        it_++;
        return ret;
    }

    friend bool operator==(const checked_iter& lhs, const checked_iter& rhs)
    {
        return (lhs.it_ == rhs.it_);
    }
    friend bool operator!=(const checked_iter& lhs, const checked_iter& rhs)
    {
        return !(lhs == rhs);
    }
};

template <typename... It>
class zip_iter
{
public:
    using value_type = std::tuple<checked_iter<It>...>;
    using reference  = std::tuple<typename checked_iter<It>::value_type...>;

private:
    value_type iters_;

public:
    explicit zip_iter(checked_iter<It>... iters) : iters_{std::move(iters)...}
    {
    }

    reference operator*()
    {
        reference ret;

        constexpr auto cnt = sizeof...(It);
        using boost::mp11::mp_for_each;
        using boost::mp11::mp_iota_c;
        mp_for_each<mp_iota_c<cnt>>([this, &ret](auto idx) {
            if (auto& it = std::get<idx>(iters_); it.valid())
                std::get<idx>(ret) = *it;
        });

        return ret;
    }

    zip_iter& operator++()
    {
        boost::mp11::tuple_for_each(iters_, [](auto& it) {
            if (it.valid())
                ++it;
        });
        return *this;
    }

    zip_iter operator++(int)
    {
        auto ret = *this;
        boost::mp11::tuple_for_each(iters_, [](auto& it) {
            if (it.valid())
                it++;
        });
        return ret;
    }

    friend bool operator==(const zip_iter& lhs, const zip_iter& rhs)
    {
        return (lhs.iters_ == rhs.iters_);
    }
    friend bool operator!=(const zip_iter& lhs, const zip_iter& rhs)
    {
        return !(lhs == rhs);
    }
};

template <typename... It>
class zip_range
{
    zip_iter<It...> begs_;
    zip_iter<It...> ends_;

public:
    explicit zip_range(checked_iter<It>... iters)
    : begs_{iters...}, ends_{checked_iter<It>{iters.end(), iters.end()}...}
    {
    }

    zip_iter<It...>& begin() { return begs_; }
    const zip_iter<It...>& begin() const { return begs_; }

    zip_iter<It...>& end() { return ends_; }
    const zip_iter<It...>& end() const { return ends_; }
};

template <typename... It>
zip_range<It...> make_zip_range(checked_iter<It>... iters)
{
    return zip_range<It...>{iters...};
}

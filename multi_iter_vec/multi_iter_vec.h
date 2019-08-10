#pragma once

#include <algorithm>
#include <iterator>
#include <numeric>
#include <vector>

// Quick and dirty implementation of a vector like container which provides
// linear view for iterations, as well as, sorted view, for binary search, etc.
template <typename T>
class multi_iter_vec
{
    using data_t    = std::vector<T>;
    using indexes_t = std::vector<unsigned>;

    data_t data_;
    indexes_t indexes_;

public:
    class native_view
    {
        const data_t* data_;

    public:
        using const_iterator = typename data_t::const_iterator;

    private:
        friend class multi_iter_vec;
        native_view(const data_t* data) noexcept : data_{data} {}

    public:
        const_iterator begin() const noexcept { return data_->begin(); }
        const_iterator end() const noexcept { return data_->end(); }
    };

    class sorted_view
    {
        const data_t* data_;
        const indexes_t* indexes_;

    public:
        class const_iterator
        {
            const data_t* data_;
            const indexes_t* indexes_;
            indexes_t::size_type idx_;

            using it_t = typename data_t::const_iterator;

        public:
            using value_type = typename std::iterator_traits<it_t>::value_type;
            using pointer    = typename std::iterator_traits<it_t>::pointer;
            using reference  = typename std::iterator_traits<it_t>::reference;
            using difference_type =
                typename std::iterator_traits<it_t>::difference_type;
            using iterator_category = std::bidirectional_iterator_tag;

        private:
            friend class sorted_view;
            const_iterator(const data_t* data,
                           const indexes_t* indexes,
                           indexes_t::size_type idx) noexcept
            : data_{data}, indexes_{indexes}, idx_{idx}
            {
            }

        public:
            reference operator*() const noexcept
            {
                // X3ME_ASSERT(idx_ < indexes_.size());
                return (*data_)[(*indexes_)[idx_]];
            }
            const_iterator& operator++() noexcept
            {
                ++idx_;
                return *this;
            }
            const_iterator operator++(int) noexcept
            {
                const_iterator ret{*this};
                ++idx_;
                return ret;
            }
            friend bool operator==(const const_iterator& lhs,
                                   const const_iterator& rhs) noexcept
            {
                return (lhs.data_ == rhs.data_) &&
                       (lhs.indexes_ == rhs.indexes_) && (lhs.idx_ == rhs.idx_);
            }
            friend bool operator!=(const const_iterator& lhs,
                                   const const_iterator& rhs) noexcept
            {
                return !(lhs == rhs);
            }
        };

    private:
        friend class multi_iter_vec;
        sorted_view(const data_t* data, const indexes_t* indexes) noexcept
        : data_{data}, indexes_{indexes}
        {
        }

    public:
        const_iterator begin() const noexcept { return {data_, indexes_, 0U}; }
        const_iterator end() const noexcept
        {
            return {data_, indexes_, indexes_->size()};
        }
    };

public:
    template <typename Comp>
    explicit multi_iter_vec(std::vector<T>&& data, Comp comp) noexcept
    : data_{std::move(data)}
    {
        indexes_.resize(data_.size());
        std::iota(indexes_.begin(), indexes_.end(), 0U);
        std::sort(indexes_.begin(), indexes_.end(),
                  [this, comp](unsigned lhs, unsigned rhs) {
                      return comp(data_[lhs], data_[rhs]);
                  });
    }

    multi_iter_vec() noexcept  = default;
    ~multi_iter_vec() noexcept = default;

    native_view get_native_view() const noexcept { return {&data_}; }
    sorted_view get_sorted_view() const noexcept { return {&data_, &indexes_}; }
};

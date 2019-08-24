#pragma once

namespace xutils
{

// A space optimized dynamically allocated array of strings.
// It's filled upon creation and don't allow changing the strings or its size.
// The same object can be safely shared/copied (cheaply) between multiple
// threads without additional synchronization as long as the logic can
// guarantee that the object is not reassigned from one thread while being
// copied from another one
// (the same thread-safety guarantee as the standard library types).
class string_dynarray
{
    // The buffer used for both indexes and string data.
    // The standard library still doesn't offer shared_ptr<T[]> functionality.
    boost::shared_ptr<char[]> data_;
    // The count of the strings
    size_t cnt_ = 0;

public:
    class iterator
    {
        const string_dynarray* parent_;
        size_t idx_;

    public:
        using value_type        = std::string_view;
        using difference_type   = ptrdiff_t;
        using pointer           = value_type*;
        using reference         = value_type&;
        using iterator_category = std::random_access_iterator_tag;

    private:
        friend class string_dynarray;
        iterator(const string_dynarray* parent, size_t idx) noexcept;

    public:
        value_type operator*() const noexcept;

        iterator& operator++() noexcept;
        iterator& operator--() noexcept;
        iterator operator++(int) noexcept;
        iterator operator--(int) noexcept;

        iterator& operator+=(difference_type v) noexcept;
        iterator& operator-=(difference_type v) noexcept;
        difference_type operator-(iterator rhs) noexcept;

        bool operator==(const iterator& rhs) noexcept;
        bool operator!=(const iterator& rhs) noexcept;
    };

public:
    string_dynarray() noexcept;
    ~string_dynarray() noexcept;

    string_dynarray(const string_dynarray&) noexcept;
    string_dynarray& operator=(const string_dynarray&) noexcept;

    explicit string_dynarray(xlib::span<const std::string> data) noexcept;

    string_dynarray(string_dynarray&& rhs) noexcept;
    string_dynarray& operator=(string_dynarray& rhs) noexcept;

    void swap(string_dynarray& rhs) noexcept;

    iterator begin() const noexcept;
    iterator end() const noexcept;

    std::string_view operator[](size_t idx) const noexcept;
    size_t size() const noexcept;
};

} // namespace xutils

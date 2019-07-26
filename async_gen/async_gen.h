#pragma once

struct arena
{
};

template <typename T>
class async_gen
{
public:
    class promise_type;
    using handle_type = std::experimental::coroutine_handle<promise_type>;

private:
    handle_type h_;

public:
    explicit async_gen(handle_type h) noexcept : h_{std::move(h)}
    {
        ::puts("async_gen::async_gen()");
    }
    async_gen(async_gen&& rhs) noexcept : h_{std::exchange(rhs.h_, nullptr)}
    {
        ::puts("async_gen::async_gen(async_gen&&)");
    }
    async_gen& operator=(async_gen&& rhs) noexcept
    {
        ::puts("async_gen::operator=(async_gen&&)");
        h_ = std::exchange(rhs.h_, nullptr);
        return *this;
    }
    ~async_gen() noexcept
    {
        ::puts("async_gen::~async_gen()");
        if (h_)
            h_.destroy();
    }

    async_gen(const async_gen&) = delete;
    async_gen& operator=(const async_gen&) = delete;

    bool advance() noexcept
    {
        ::puts("async_gen::advance()");
        h_.resume();
        return !h_.done();
    }

    const T& get() noexcept
    {
        ::puts("async_gen::get()");
        return h_.promise().val_;
    }

public:
    class iterator
    {
        async_gen* gen_;

    public:
        explicit iterator(async_gen* gen) noexcept : gen_{gen} {}

        const T& operator*() noexcept { return gen_->get(); }
        iterator& operator++() noexcept
        {
            if (!gen_->advance())
                gen_ = nullptr;
            return *this;
        }

        friend bool operator==(const iterator& lhs,
                               const iterator& rhs) noexcept
        {
            return lhs.gen_ == rhs.gen_; // This is not correct of course
        }
        friend bool operator!=(const iterator& lhs,
                               const iterator& rhs) noexcept
        {
            return !(lhs == rhs);
        }
    };

    auto begin() noexcept
    {
        advance();
        return iterator{this};
    }
    auto end() noexcept { return iterator{nullptr}; }

public:
    class promise_type
    {
        friend class async_gen;
        T val_;

    public:
        promise_type() noexcept : val_{}
        {
            ::puts("promise_type::promise_type()");
        }
        ~promise_type() noexcept { ::puts("promise_type::~promise_type()"); }
        auto get_return_object() noexcept
        {
            ::puts("promise_type::get_return_object");
            return async_gen<T>{handle_type::from_promise(*this)};
        }
        auto yield_value(const T& v) noexcept
        {
            ::puts("promise_type::yield_value");
            val_ = v;
            return std::experimental::suspend_always{};
        }
        /*
        auto return_value(const T& v) noexcept
        {
            ::puts("promise_type::return_value");
            val_ = v;
            return std::experimental::suspend_never{};
        }
        */
        auto return_void() noexcept
        {
            ::puts("promise_type::return_void");
            return std::experimental::suspend_never{};
        }
        auto initial_suspend() noexcept
        {
            ::puts("promise_type::initial_suspend");
            return std::experimental::suspend_always{};
        }
        auto final_suspend() noexcept
        {
            ::puts("promise_type::final_suspend");
            return std::experimental::suspend_always{};
        }
        void unhandled_exception() noexcept
        {
            ::puts("promise_type::unhandled_exception");
            ::exit(1);
        }

        template <typename... Args>
        void* operator new(size_t sz, Args&&...)
        {
            ::puts("operator new(sz)");
            return ::malloc(sz);
        }
        template <typename... Args>
        void* operator new(size_t sz, arena&, Args&&...)
        {
            ::puts("operator new(sz, arena)");
            return ::malloc(sz);
        }
        template <typename This, typename... Args>
        void* operator new(size_t sz, const This&, arena&, Args&&...)
        {
            ::puts("operator new(sz, this, arena)");
            return ::malloc(sz);
        }
        void operator delete(void* p, size_t)
        {

            ::puts("operator delete(void*, size_t)");
            ::free(p);
        }
    };
};

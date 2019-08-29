#pragma once

namespace xutils
{

struct malloc_free_resource final : bcont::pmr::memory_resource
{
    ~malloc_free_resource() noexcept override = default;

    void* do_allocate(size_t size, size_t alignment) noexcept override
    {
        ::printf("do_allocate %zu bytes\n", size);
        return ::aligned_alloc(size, alignment);
    }

    void do_deallocate(void* p,
                       [[maybe_unused]] size_t size,
                       [[maybe_unused]] size_t alignment) noexcept override
    {
        ::printf("do_deallocate %zu bytes\n", size);
        ::free(p);
    }

    bool do_is_equal(const bcont::pmr::memory_resource& oth) const
        noexcept override
    {
        return (this == &oth);
    }
};

} // namespace xutils

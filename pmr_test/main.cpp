#include "precompiled.h"
#include "xutils/malloc_free_resource.h"

static void fun(bcont::pmr::string& str) noexcept
{
}

int main()
{
    std::array<std::byte, 2048> buff;
    xutils::malloc_free_resource upstream;
    bcont::pmr::monotonic_buffer_resource mem_res{buff.data(), buff.size(),
                                                  &upstream};
    bcont::pmr::string str{&mem_res};
    str.reserve(1000);

    fun(str);

    return 0;
}

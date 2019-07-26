#include "precompiled.h"

#include "async_gen.h"

async_gen<int> answer(arena&, int lim)
{
    for (int i = 0; i < lim; ++i)
        co_yield i;
}

int main()
{
    auto answer2 = [](arena&, int lim) -> async_gen<int> {
        for (int i = 0; i < lim; ++i)
            co_yield i;
    };

    ::puts("Starting main");
    arena a;
    for (auto v : answer2(a, 3))
        ::printf("Got value: %d\n", v);
    ::puts("Stopping main");
    return 0;
}

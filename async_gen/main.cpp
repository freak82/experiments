#include "precompiled.h"

#include "async_gen.h"

async_gen<int> answer(int lim)
{
    for (int i = 0; i < lim; ++i)
        co_yield i;
}

int main()
{
    ::puts("Starting main");
    for (auto v : answer(3))
        ::printf("Got value: %d\n", v);
    ::puts("Stopping main");
    return 0;
}

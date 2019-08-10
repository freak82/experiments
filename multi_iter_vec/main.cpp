#include "precompiled.h"

#include <functional>
#include <random>

#include "multi_iter_vec.h"

int main()
{
    std::vector<int> v = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(v.begin(), v.end(), g);

    multi_iter_vec<int> vec{std::move(v), std::greater<void>{}};

    ::printf("Native view: [");
    for (const auto& v : vec.get_native_view())
    {
        ::printf("%d,", v);
    }
    ::printf("]\n");

    ::printf("Sorted view: [");
    for (const auto& v : vec.get_sorted_view())
    {
        ::printf("%d,", v);
    }
    ::printf("]\n");

    return 0;
}

#include "precompiled.h"

#include "zip_iter.h"

int main()
{
    std::array<int, 3> arr = {1, 2, 3};
    std::vector<int> vec   = {42, 42, 42, 43, 44};

    const auto rng = make_zip_range(checked_iter{arr.cbegin(), arr.cend()},
                                    checked_iter{vec.cbegin(), vec.cend()});
    for (const auto [a, v] : rng)
    {
        ::printf("A:%d V:%d\n", a, v);
    }

    return 0;
}

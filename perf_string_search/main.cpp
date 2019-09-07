#include "precompiled.h"

namespace
{
using string_type = std::string;

std::vector<string_type> gen_random_strings(size_t cnt_strings) noexcept
{
    const char chrs[] = "0123456789"
                        "abcdefghijklmnopqrstuvwxyz"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    std::mt19937 rg{std::random_device{}()};
    std::uniform_int_distribution<> pick(0, sizeof(chrs) - 1);
    std::uniform_int_distribution<> str_size(8, 48);

    std::vector<string_type> ret;
    ret.reserve(cnt_strings);

    for (size_t i = 0; i < cnt_strings; ++i)
    {
        const size_t len = str_size(rg);

        string_type s;
        s.resize(len);
        for (size_t j = 0; j < len; ++j)
        {
            s[j] = chrs[pick(rg)];
        }

        ret.push_back(std::move(s));
    }

    return ret;
}

void do_shuffle(auto& cont) noexcept
{
    std::mt19937 rg{std::random_device{}()};
    std::shuffle(cont.begin(), cont.end(), rg);
}

std::vector<string_type> gen_unique_keys(const auto& found_keys,
                                         const auto& not_found_keys) noexcept
{
    std::vector<string_type> ret;
    std::copy(found_keys.cbegin(), found_keys.cend(), std::back_inserter(ret));
    std::copy(not_found_keys.cbegin(), not_found_keys.cend(),
              std::back_inserter(ret));
    do_shuffle(ret);
    return ret;
}

std::vector<string_type> gen_all_keys(const auto& uniq_keys) noexcept
{
    std::vector<string_type> ret;
    std::copy(uniq_keys.cbegin(), uniq_keys.cend(), std::back_inserter(ret));
    std::copy(uniq_keys.cbegin(), uniq_keys.cend(), std::back_inserter(ret));
    std::copy(uniq_keys.cbegin(), uniq_keys.cend(), std::back_inserter(ret));
    std::copy(uniq_keys.cbegin(), uniq_keys.cend(), std::back_inserter(ret));
    std::copy(uniq_keys.cbegin(), uniq_keys.cend(), std::back_inserter(ret));
    do_shuffle(ret);
    return ret;
}

std::vector<uint16_t> gen_all_keys_num(const auto& all_keys,
                                       const auto& uniq_keys) noexcept
{
    std::vector<uint16_t> ret;
    ret.reserve(all_keys.size());
    for (const auto& k : all_keys)
    {
        auto it = std::find(uniq_keys.begin(), uniq_keys.end(), k);
        if (it == uniq_keys.end())
            std::abort();
        ret.push_back(it - uniq_keys.begin());
    }
    return ret;
}

size_t find_string_keys(const auto& all_keys, const auto& curr_keys) noexcept
{
    // This number is returned to prevent optimization and to check
    // the correctness of the algorithm.
    size_t ret = 0;
    for (const auto& k : all_keys)
    {
        auto it = std::find(curr_keys.begin(), curr_keys.end(), k);
        if (it != curr_keys.end())
        {
            ret += (it - curr_keys.begin());
        }
    }
    return ret;
}

size_t find_numeric_keys(const auto& all_keys_num,
                         const auto& uniq_keys,
                         const auto& curr_keys) noexcept
{
    // First create the indexes
    boost::container::small_vector<uint16_t, 32> curr_keys_num;
    for (const auto& k : curr_keys)
    {
        auto it = std::find(uniq_keys.begin(), uniq_keys.end(), k);
        if (it == uniq_keys.end())
            std::abort();
        curr_keys_num.push_back(it - uniq_keys.begin());
    }

    // And now do the search with the indexes
    // This number is returned to prevent optimization and to check
    // the correctness of the algorithm.
    size_t ret = 0;
    for (auto k : all_keys_num)
    {
        auto it = std::find(curr_keys_num.begin(), curr_keys_num.end(), k);
        if (it != curr_keys_num.end())
        {
            ret += (it - curr_keys_num.begin());
        }
    }
    return ret;
}

[[maybe_unused]] void
dump_strings(const std::vector<string_type>& strings) noexcept
{
    std::cout << '[';
    for (const auto& s : strings)
    {
        std::cout << "\n\t" << s;
    }
    std::cout << "\n]\n";
}

} // namespace
////////////////////////////////////////////////////////////////////////////////

int main()
{
    const auto found     = gen_random_strings(24);
    const auto not_found = gen_random_strings(8);

    const auto uniq_keys    = gen_unique_keys(found, not_found);
    const auto all_keys     = gen_all_keys(uniq_keys);
    const auto all_keys_num = gen_all_keys_num(all_keys, uniq_keys);
    const auto curr_keys    = found;

    const auto n0 = find_string_keys(all_keys, curr_keys);
    const auto n1 = find_numeric_keys(all_keys_num, uniq_keys, curr_keys);
    if (n0 != n1)
    {
        std::cout << "Wrong algorithm 3\n";
        return 1;
    }

    std::cout << "Start the experiment\n";

    nonius::configuration cfg;
    cfg.output_file                       = "results.html";
    cfg.reporter                          = "html";
    nonius::benchmark_registry benchmarks = {
        nonius::benchmark("find_string_keys",
                          [&] {
                              const auto v =
                                  find_string_keys(all_keys, curr_keys);
                              X3ME_DO_NOT_OPTIMIZE_AWAY(v);
                              return v;
                          }),
        nonius::benchmark("find_numeric_keys",
                          [&] {
                              const auto v = find_numeric_keys(
                                  all_keys_num, uniq_keys, curr_keys);
                              X3ME_DO_NOT_OPTIMIZE_AWAY(v);
                              return v;
                          }),
    };

    nonius::go(cfg, benchmarks);
}

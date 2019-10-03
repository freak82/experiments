#include "precompiled.h"

#include "xutils/cpuset.h"
#include "xutils/do_throw.h"

namespace xlib
{
using namespace x3me::sys_utils;
} // namespace xlib
////////////////////////////////////////////////////////////////////////////////

static xutils::cpuset get_cpuset(const char* cfg)
{
    // The need of this temporary string is really sad but std::from_chars
    // is still not available on GCC 7.4 and all other functions need NULL
    // terminated string.
    auto stoi = [tmp = std::string{}](std::string_view sv) mutable noexcept
    {
        tmp = sv;
        return std::stoi(tmp);
    };
    struct
    {
        const uint32_t cnt_cpus_ = std::thread::hardware_concurrency();
        bool operator()(int idx) noexcept
        {
            return (idx >= 0) && (idx < static_cast<int>(cnt_cpus_));
        }
        bool operator()(int min_idx, int max_idx) noexcept
        {
            return (min_idx >= 0) && (min_idx <= max_idx) &&
                   (max_idx < static_cast<int>(cnt_cpus_));
        }
    } check_cpu_idx;

    xutils::cpuset ret{check_cpu_idx.cnt_cpus_};
    using namespace ctre::literals;
    for (auto match : ctre::range(cfg, "([^,]+),?"_ctre))
    {
        const auto m = match.get<1>().to_view();
        if (auto r0 = "^\\s*(\\d+)\\s*\\-\\s*(\\d+)\\s*$"_ctre.match(m))
        {
            const int i0 = stoi(r0.get<1>().to_view());
            const int i1 = stoi(r0.get<2>().to_view());
            if (!check_cpu_idx(i0, i1))
            {
                xutils::do_throw<std::runtime_error>(
                    "Invalid range of CPU indexes: {}-{}", i0, i1);
            }
            for (int i = i0; i <= i1; ++i)
            {
                if (ret.is_set_cpu(i))
                {
                    xutils::do_throw<std::runtime_error>(
                        "CPU index {}, coming from '{}', is already present", i,
                        m);
                }
                ::fprintf(stdout, "Set CPU idx: %d\n", i);
                ret.set_cpu(i);
            }
        }
        else if (auto r1 = "^\\s*(\\d+)\\s*$"_ctre.match(m))
        {
            const int i = stoi(r1.get<1>().to_view());
            if (!check_cpu_idx(i))
            {
                xutils::do_throw<std::runtime_error>("Invalid CPU index: {}",
                                                     i);
            }
            if (ret.is_set_cpu(i))
            {
                xutils::do_throw<std::runtime_error>(
                    "CPU index {}, coming from '{}', is already present", i, m);
            }
            ::fprintf(stdout, "Set CPU idx: %d\n", i);
            ret.set_cpu(i);
        }
        else
        {
            xutils::do_throw<std::runtime_error>(
                "Invalid CPU index(es) group '{}'", m);
        }
    }
    return ret;
}

////////////////////////////////////////////////////////////////////////////////

void thread_fun(double* pd) noexcept
{
    for (;;)
    {
        // Just do some work here to simulate CPU usage
        double d = 13413543.23424354364567;
        for (int i = 0; i < 10000; ++i)
        {
            d = (::sin(d) * ::cos(d)) / ::atan(d) / ::tan(d);
        }
        *pd = d;

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(50ms);
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        ::fprintf(stderr,
                  "Usage: bin 0|cpunum-cpunum|cpunum-cpunum,cpunum-cpunum\n");
        return EXIT_FAILURE;
    }

    if (argv[1][0] == '0')
    {
        ::fprintf(stdout, "Won't set affinity\n");
    }
    else
    {
        ::fprintf(stdout, "Setting affinity\n");
        try
        {
            const auto cs = get_cpuset(argv[1]);
            if (::sched_setaffinity(xlib::process_id(), cs.size(), cs.data()) ==
                -1)
            {
                ::perror("sched_setaffinity");
                ::exit(EXIT_FAILURE);
            }
        }
        catch (const std::exception& ex)
        {
            ::fprintf(stderr, "Got error: %s\n", ex.what());
            return EXIT_FAILURE;
        }
    }

    const size_t cnt_threads = 4;
    std::vector<std::thread> threads;
    threads.reserve(cnt_threads);
    std::vector<double> results(cnt_threads);
    for (size_t i = 0; i < cnt_threads; ++i)
    {
        threads.emplace_back(thread_fun, &results[i]);
    }

    for (auto& t : threads)
        t.join();
    // These will be never printed actually
    for (double r : results)
        ::fprintf(stdout, "%f\n", r);

    return EXIT_SUCCESS;
}

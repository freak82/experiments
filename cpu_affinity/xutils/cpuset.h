#pragma once

namespace xutils
{

// Useful for preparing a cpu_set for sched_set/getaffinity
// The cpu_set_t is dynamically allocated so that this functionality is able
// to handle machines with more than 1024 cores. Not that we'll ever use such i
// machines but the overhead is not that much taking into account that we'll
// most likely set the affinity just once for the application lifetime.
class cpuset
{
    cpu_set_t* data_;
    uint32_t size_;
    uint32_t cnt_cpus_;

public:
    explicit cpuset(
        uint32_t cnt_cpus = std::thread::hardware_concurrency()) noexcept
    : data_{CPU_ALLOC(cnt_cpus)}
    , size_{CPU_ALLOC_SIZE(cnt_cpus)}
    , cnt_cpus_{cnt_cpus}
    {
        X3ME_ENFORCE(data_);
        CPU_ZERO_S(size_, data_);
    }
    ~cpuset() noexcept { CPU_FREE(data_); }

    cpuset(cpuset&& rhs) noexcept
    : data_{std::exchange(rhs.data_, nullptr)}
    , size_{std::exchange(rhs.size_, 0)}
    , cnt_cpus_{std::exchange(rhs.cnt_cpus_, 0)}
    {
    }
    cpuset& operator=(cpuset&& rhs) noexcept
    {
        if (X3ME_LIKELY(this != &rhs))
        {
            data_     = std::exchange(rhs.data_, nullptr);
            size_     = std::exchange(rhs.size_, 0);
            cnt_cpus_ = std::exchange(rhs.cnt_cpus_, 0);
        }
        return *this;
    }

    cpuset(const cpuset&) = delete;
    cpuset& operator=(const cpuset&) = delete;

    void set_cpu(uint32_t cpu_idx) noexcept
    {
        X3ME_ASSERT(cpu_idx < cnt_cpus_);
        CPU_SET_S(cpu_idx, size_, data_);
    }
    void clr_cpu(uint32_t cpu_idx) noexcept
    {
        X3ME_ASSERT(cpu_idx < cnt_cpus_);
        CPU_CLR_S(cpu_idx, size_, data_);
    }
    bool is_set_cpu(uint32_t cpu_idx) const noexcept
    {
        return CPU_ISSET_S(cpu_idx, size_, data_);
    }

    cpu_set_t* data() noexcept { return data_; }
    const cpu_set_t* data() const noexcept { return data_; }
    size_t size() const noexcept { return size_; }
};

} // namespace xutils

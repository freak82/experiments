#include "precompiled.h"
#include "xutils/string_dynarray.h"
#include "xutils/string_dynarray.ipp"

static std::random_device s_rd;
static std::mt19937 s_g(s_rd());

class sorted_vector_fixture : public ::hayai::Fixture
{
public:
    std::vector<std::string> headers_;

    std::vector<std::string> sorted_headers_;

public:
    virtual void SetUp() override
    {
        headers_.push_back("Host");
        headers_.push_back("Content-Length");
        headers_.push_back("Content-MD5");
        headers_.push_back("x-content-sha");
        headers_.push_back("ETag");
        headers_.push_back("Last-Modified");
        headers_.push_back("Last-Modified-Time");

        std::sort(headers_.begin(), headers_.end());
        sorted_headers_ = headers_;

        std::shuffle(headers_.begin(), headers_.end(), s_g);
    }

    virtual void TearDown() override {}
};

class string_dynarray_fixture : public ::hayai::Fixture
{
public:
    std::vector<std::string> headers_;

    xutils::string_dynarray sorted_headers_;

public:
    virtual void SetUp() override
    {
        headers_.push_back("Host");
        headers_.push_back("Content-Length");
        headers_.push_back("Content-MD5");
        headers_.push_back("x-content-sha");
        headers_.push_back("ETag");
        headers_.push_back("Last-Modified");
        headers_.push_back("Last-Modified-Time");

        std::sort(headers_.begin(), headers_.end());

        sorted_headers_ = xutils::string_dynarray{headers_};

        std::shuffle(headers_.begin(), headers_.end(), s_g);
    }

    virtual void TearDown() override {}
};

////////////////////////////////////////////////////////////////////////////////

BENCHMARK_F(string_dynarray_fixture, string_dynarray, 50, 1000)
{
    for (const auto& h : headers_)
    {
        auto it =
            std::lower_bound(sorted_headers_.begin(), sorted_headers_.end(), h,
                             [](std::string_view lhs, const std::string& rhs) {
                                 return (lhs < rhs);
                             });
        assert(it != sorted_headers_.end());
    }
}

BENCHMARK_F(sorted_vector_fixture, sorted_vector, 50, 1000)
{
    for (const auto& h : headers_)
    {
        auto it = std::lower_bound(
            sorted_headers_.begin(), sorted_headers_.end(), h,
            [](const std::string& lhs, const std::string& rhs) {
                return (lhs < rhs);
            });
        assert(it != sorted_headers_.end());
    }
}

BENCHMARK_F(string_dynarray_fixture, string_dynarray2, 50, 1000)
{
    for (const auto& h : headers_)
    {
        auto it =
            std::lower_bound(sorted_headers_.begin(), sorted_headers_.end(), h,
                             [](std::string_view lhs, const std::string& rhs) {
                                 return (lhs < rhs);
                             });
        assert(it != sorted_headers_.end());
    }
}

int main()
{
    hayai::ConsoleOutputter consoleOutputter;
    hayai::Benchmarker::AddOutputter(consoleOutputter);
    hayai::Benchmarker::RunAllTests();
    return 0;
}

#include "precompiled.h"

namespace balgo = boost::algorithm;

void parse_ini(std::istream& ini_data)
{
    auto rem_comment = [](std::string & s) noexcept
    {
        const auto pos = s.find('#');
        if (pos != std::string::npos)
            s.resize(pos);
    };

    using namespace ctre::literals;
    for (std::string line; std::getline(ini_data, line);)
    {
        rem_comment(line);

        balgo::trim(line);

        if (line.empty())
            continue;

        if ("^\\[(strong|weak-if|weak)-key\\]$"_ctre.match(line))
        {
            ::printf("Matched group: %s\n", line.c_str());
            continue;
        }

        if ("^(\\$[a-zA-Z0-9]+)\\s*=\\s*(non-empty|allow-empty)$"_ctre.match(
                line))
        {
            ::printf("Matched keyword: %s\n", line.c_str());
            continue;
        }

        if ("^([a-zA-Z0-9\\-]+)\\s*=\\s*(non-empty|allow-empty)$"_ctre.match(
                line))
        {
            ::printf("Matched header: %s\n", line.c_str());
            continue;
        }

        ::printf("Skipped line: %s\n", line.c_str());
    }
}

int main(int argv, char** argc)
{
    try
    {
        if (argv != 2)
            throw std::invalid_argument("Usage: binary <file-path>");

        std::ifstream f{argc[1]};
        if (!f)
        {
            throw std::system_error(errno, std::system_category(),
                                    "Failed to open");
        }
        parse_ini(f);
    }
    catch (const std::exception& ex)
    {
        ::printf("General-error: %s\n", ex.what());
    }

    return 0;
}

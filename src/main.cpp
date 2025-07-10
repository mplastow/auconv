// main.cpp - auconv

#include <cstdio>
#include <string>
#include <vector>

#include <cli.hpp>
#include <snd.hpp>

namespace { // provides internal linkage for `main.cpp`

} // namespace

auto main(int argc, char* argv[]) -> int
{
    using ArgVec = std::vector<std::string>;

    ArgVec arg_vec {};
    for (int i { 0 }; i < argc; ++i) {
        arg_vec.emplace_back(argv[i]);
    }

    auto parsed_args = auconv::handleCLIArguments(arg_vec);

    // TODO(MATT): do something with parsed args
    // auconv::scheduleConversion(parsed_args);

    std::quick_exit(0);
}
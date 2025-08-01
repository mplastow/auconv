// main.cpp - auconv

#include <cstdio>

#include <cli.hpp>
#include <defines.hpp>
#include <format.hpp>
#include <snd.hpp>
#include <types.hpp>

auto main(int argc, char* argv[]) -> int
{
    auconv::ArgArray arg_vec {};
    for (int i {0}; i < argc; ++i) {
        arg_vec.emplace_back(argv[i]);
    }

    auconv::Format wav {1};
    auconv::Format flac {2};

    auconv::convert(wav, flac);

    auto parsed_args = auconv::handleCLIArguments(arg_vec);

    auconv::convertFiles(parsed_args);

    std::quick_exit(0);
}
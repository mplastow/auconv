// main.cpp - auconv

#include <cstdio>

#include <defines.hpp>
#include <cli.hpp>
#include <snd.hpp>
#include <types.hpp>

auto main(int argc, char* argv[]) -> int
{
    auconv::ArgArray arg_vec {};
    for (int i { 0 }; i < argc; ++i) {
        arg_vec.emplace_back(argv[i]);
    }

    auto parsed_args = auconv::handleCLIArguments(arg_vec);

    auconv::handleParsedArgs(parsed_args);

    std::quick_exit(0);
}
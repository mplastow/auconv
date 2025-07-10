// flac.hpp - auconv

#ifndef AUCONV_FLAC_HPP
#define AUCONV_FLAC_HPP

#include <filesystem>
#include <iostream>
#include <string>

#include <sndfile.h>
#include <sndfile.hh>

#include <snd.hpp>

namespace auconv {

void convertWavToFlacInDir(std::string const& starting_path)
{
    for (auto const& dir_entry : std::filesystem::directory_iterator(starting_path)) {
        if (dir_entry.is_regular_file()) {
            if (dir_entry.path().extension() == ".wav") {
                std::cout << "Converting wav file: " << dir_entry.path().filename() << '\n';

                std::string outputFile = dir_entry.path().parent_path();
                outputFile.append("/").append(dir_entry.path().stem()).append(".flac");

                convertAudioFile(
                    dir_entry.path(),
                    outputFile,
                    SF_FORMAT_FLAC | SF_FORMAT_PCM_16);
            }
        } else {
            std::cout << "Directory: " << dir_entry << '\n';
        }
    }
}

void convertWavToFlacInDirTree(std::string const& starting_path)
{
    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator(starting_path)) {
        if (dir_entry.is_regular_file()) {
            if (dir_entry.path().extension() == ".wav") {
                std::cout << "Converting wav file: " << dir_entry.path().filename() << '\n';

                std::string outputFile = dir_entry.path().parent_path();
                outputFile.append("/").append(dir_entry.path().stem()).append(".flac");

                convertAudioFile(
                    dir_entry.path(),
                    outputFile,
                    SF_FORMAT_FLAC | SF_FORMAT_PCM_16);
            }
        } else {
            std::cout << "Directory: " << dir_entry << '\n';
        }
    }
}

} // namespace auconv

#endif // AUCONV_FLAC_HPP
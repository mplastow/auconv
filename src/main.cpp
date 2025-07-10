#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <filesystem>

#include <lame/lame.h>
#include <sndfile.hh>
#include <sndfile.h>

constexpr int BUFFER_SIZE = 1024; // 1kb buffer
constexpr int MP3_BUFFER_SIZE = BUFFER_SIZE;
constexpr int PCM_BUFFER_SIZE = BUFFER_SIZE * 2;

void PrintFileInfo(std::string inputFile)
{
    // Print file properties with libsndfile
    SndfileHandle inputFileHandle = SndfileHandle(inputFile);
    if (inputFileHandle.error()) {
        std::cout << "\tCould not open input file: " << inputFileHandle.strError() << std::endl;
        return;
    } else {
        std::cout << "\tInput file: " << inputFile << std::endl;
        std::cout << "\t\tFormat: 0x" << std::hex << inputFileHandle.format() << std::endl;
        std::cout << "\t\tSample rate: " << std::dec << inputFileHandle.samplerate() << std::endl;
        std::cout << "\t\tChannels: " << inputFileHandle.channels() << std::endl;
        std::cout << "\t\tFrames in file: " << inputFileHandle.frames() << std::endl;
    }
}

lame_global_flags* InitLameWithFlags()
{
    // Initialize LAME
    // Create global flags object
    lame_global_flags* gfp = lame_init();

    // Set global flags
    // Input stream flags
    lame_set_num_samples(gfp, BUFFER_SIZE); /* number of samples in the input stream */
    lame_set_in_samplerate(gfp, 44100);     /* sample rate is 44.1k or 48k */
    lame_set_num_channels(gfp, 2);          /* 1 for mono, 2 for joint/stereo */
    // Output stream flags TODO: find optimal settings
    lame_set_out_samplerate(gfp, 44100); /* sample rate is 44.1k or 48k */
    lame_set_mode(gfp, JOINT_STEREO);    /* mono, stereo, joint stereo */
    lame_set_quality(gfp, 0);            /* 0 = best 2 = high 5 = med 7 = low */
    lame_set_brate(gfp, 320);            /* max bitrate = 320 kbps */
    lame_set_compression_ratio(gfp, 11); /* default compression ratio = 11 */

    // int ret_code = lame_init_params(gfp);
    // Check for problems: ret_code must be >= 0
    std::cout << "LAME init (>= 0 is success): " << lame_init_params(gfp) << std::endl;

    return gfp;
}

void ConvertWavToMp3(const std::string inputFile, const std::string outputFile)
{
    // mp3 conversion with libmp3lame
    // See https://stackoverflow.com/a/2496831/22896065
    lame_global_flags* gfp = InitLameWithFlags();

    // Create pcm read-in buffer
    static short int pcm_buffer[PCM_BUFFER_SIZE]; // double buffer size due to interlaced channels
    // Read from file into buffer
    int read = 0;
    std::cout << "PCM buffer length: " << sizeof(pcm_buffer) << " bytes." << std::endl;

    // Create mp3 buffer: char type used for size
    static unsigned char mp3_buffer[MP3_BUFFER_SIZE];
    std::cout << "MP3 buffer length: " << sizeof(mp3_buffer) << " bytes." << std::endl;

    FILE* pcm_file_ptr = fopen("test-audio/wav/BGE_170_G_Stars_Arp_Pad_16.wav", "rb");
    FILE* mp3_file_ptr = fopen("test-audio/out/out1.mp3", "wb");

    int write = 0;
    // Skip header (44 bytes) TODO: parse header to determine actual number of bytes
    read = fread(pcm_buffer, 44, 1, pcm_file_ptr);
    do {
        // Read interlaced data into buffer
        read = fread(pcm_buffer, (sizeof(short int) * 2), BUFFER_SIZE, pcm_file_ptr);
        if (read == 0) {
            write = lame_encode_flush(gfp, mp3_buffer, MP3_BUFFER_SIZE);
        } else {
            write = lame_encode_buffer_interleaved(gfp, pcm_buffer, read, mp3_buffer, MP3_BUFFER_SIZE);
        }
        fwrite(mp3_buffer, write, 1, mp3_file_ptr);
    } while (read != 0);
    std::cout << "Finished writing mp3 file." << std::endl;

    // Close output file
    fclose(mp3_file_ptr);
    // Close LAME
    lame_close(gfp);
}

void ConvertAudioFile(std::string inputFile, std::string outputFile, int outputFormat)
{
    // Get handle to input file
    SndfileHandle inputFileHandle = SndfileHandle(inputFile);
    if (inputFileHandle.error()) {
        std::cout << "Could not open input file: " << inputFileHandle.strError() << std::endl;
        return;
    } else {
        PrintFileInfo(inputFile);
    }

    // Get handle to output file
    SndfileHandle outputFileHandle = SndfileHandle(
        outputFile,
        SFM_WRITE,
        outputFormat,
        inputFileHandle.channels(),
        inputFileHandle.samplerate());
    // Exit if an error occurred
    if (outputFileHandle.error()) {
        std::cout << "\tCould not open output file: " << outputFileHandle.strError() << std::endl;
        std::cout << "\tCheck against specified format (1 = pass): " << outputFileHandle.formatCheck(outputFormat, inputFileHandle.channels(), inputFileHandle.samplerate()) << std::endl;
        return;
    } else { // similar to PrintFileInfo()
        std::cout << "\tOutput file: " << outputFile << std::endl;
        std::cout << "\t\tFormat: 0x" << std::hex << outputFileHandle.format() << std::endl;
        std::cout << "\t\tSample rate: " << std::dec << outputFileHandle.samplerate() << std::endl;
        std::cout << "\t\tChannels: " << outputFileHandle.channels() << std::endl;
    }

    // Initialize conversion buffer
    static short buffer[BUFFER_SIZE];
    // std::cout << "Read/write buffer size: " << BUFFER_SIZE << std::endl;

    // Initialize read and write objects
    sf_count_t read = 1;
    sf_count_t write = 1;

    // Read through file to convert from input format to output format
    // and write to output file
    while (read != 0) {
        read = inputFileHandle.read(buffer, BUFFER_SIZE);
        // std::cout << "Read: " << read << std::endl;

        if (read != 0) { // write to out if bytes were read
            write = outputFileHandle.write(buffer, read);
            // std::cout << "Write: " << write << std::endl;
        }
    }

    std::cout << "\tFinished file conversion." << std::endl;
    // std::cout << " Reached end of input file. Last read: " << read << std::endl;
    if (write == 0) {
        //     std::cout << "Warning: Last write was 0 bytes! Last write: " << write << "\n" << std::endl;
    } else {
        //     std::cout << "Last write: " << write << "\n" << std::endl;
    }

    return;
}

void ConvertWavToFlacInDir(std::string starting_path)
{
    for (auto const& dir_entry : std::filesystem::directory_iterator(starting_path)) {
        if (dir_entry.is_regular_file()) {
            if (dir_entry.path().extension() == ".wav") {
                std::cout << "Converting wav file: " << dir_entry.path().filename() << std::endl;

                std::string outputFile = dir_entry.path().parent_path();
                outputFile.append("/").append(dir_entry.path().stem()).append(".flac");

                ConvertAudioFile(
                    dir_entry.path(),
                    outputFile,
                    SF_FORMAT_FLAC | SF_FORMAT_PCM_16);
            }
        } else {
            std::cout << "Directory: " << dir_entry << '\n';
        }
    }
}

void ConvertWavToFlacInDirTree(std::string starting_path)
{
    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator(starting_path)) {
        if (dir_entry.is_regular_file()) {
            if (dir_entry.path().extension() == ".wav") {
                std::cout << "Converting wav file: " << dir_entry.path().filename() << std::endl;

                std::string outputFile = dir_entry.path().parent_path();
                outputFile.append("/").append(dir_entry.path().stem()).append(".flac");

                ConvertAudioFile(
                    dir_entry.path(),
                    outputFile,
                    SF_FORMAT_FLAC | SF_FORMAT_PCM_16);
            }
        } else {
            std::cout << "Directory: " << dir_entry << '\n';
        }
    }
}

int main(int argc, char** argv)
{

    if (argc == 2) {
        std::string help_flag = argv[1];
        if (help_flag == "--help") {
            std::cout << "auconv v0.01: a basic WAV to FLAC converter" << std::endl;
            std::cout << "Usage: auconv [mode] [path]" << std::endl;
            std::cout << "Modes:" << std::endl;
            std::cout << "\t-s: convert single file. [path] is path to file." << std::endl;
            std::cout << "\t-d: convert all files in a single directory. [path] is path to directory." << std::endl;
            std::cout << "\t-t: convert all files in a directory and all its subdirectories. [path] is path to top directory." << std::endl;
            std::cout << "Path: must be absolute path, e.g. /home/user/audio/wav/, or '.' for current directory\n"
                      << std::endl;

            return 1;
        } else {
            std::cout << "Incorrect number of arguments. Use auconv --help" << std::endl;
            return 1;
        }
    }

    if (argc != 3) {
        std::cout << "Incorrect number of arguments. Use auconv --help" << std::endl;
        return 1;
    }

    std::string mode_flag = argv[1];
    std::string user_path = argv[2];

    auto starting_path = std::filesystem::current_path();
    if (user_path != ".") {
        starting_path = std::filesystem::path(user_path);
    }

    if (!std::filesystem::exists(starting_path)) {
        std::cout << "Path does not exist." << std::endl;
        return 1;
    }

    // Single file
    if (mode_flag == "-s") {
        if (!starting_path.has_filename()) {
            std::cout << "Path must be a file, not a directory." << std::endl;
            return 1;
        }

        if (starting_path.extension() == ".wav") {
            std::cout << "Converting wav file: " << starting_path.filename() << std::endl;

            std::string outputFile = starting_path.parent_path();
            outputFile.append("/").append(starting_path.stem()).append(".flac");

            ConvertAudioFile(
                starting_path,
                outputFile,
                SF_FORMAT_FLAC | SF_FORMAT_PCM_16);
        }
    }
    // Directory, all items
    else if (mode_flag == "-d") {
        // Check if path is to a file
        if (starting_path.has_filename()) {
            std::cout << "Path must be a directory, not a file, when using '-d'" << std::endl;
            return 1;
        }

        ConvertWavToFlacInDir(starting_path);
    }
    // Directory tree, all items
    else if (mode_flag == "-t") {
        // Check if path is to a file
        if (starting_path.has_filename()) {
            std::cout << "Path must be a directory, not a file, when using '-t'" << std::endl;
            return 1;
        }

        ConvertWavToFlacInDirTree(starting_path);
    } else {
        std::cout << "Unspecified conversion mode. Use auconv --help" << std::endl;
        return 1;
    }

    std::cout << "Converted all audio files." << std::endl;

    return 0;
}
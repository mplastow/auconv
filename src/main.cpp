#include <iostream>
#include <stdio.h>
#include <string>
#include "../include/lame.h"
#include "sndfile.hh"

constexpr int BUFFER_SIZE = 8192; // 8kb buffer
constexpr int MP3_BUFFER_SIZE = 8192; // 8kb buffer
constexpr int PCM_BUFFER_SIZE = 16384; // 16kb buffer

void PrintFileInfo(std::string filename) {
    // Print file properties with libsndfile
    // See example on github (https://github.com/libsndfile/libsndfile/blob/master/examples/sndfilehandle.cc)
    SndfileHandle pcm_file = SndfileHandle(filename);

    std::cout << "Opened file " << filename << std::endl;
    std::cout << "Format: 0x" << std::hex << pcm_file.format() << std::endl;
    std::cout << "Sample rate: " << std::dec << pcm_file.samplerate() << std::endl;
    std::cout << "Channels: " << pcm_file.channels() << std::endl;
    std::cout << "Frames in file: " << pcm_file.frames() << std::endl;
}

lame_global_flags* InitLameWithFlags() {
    // Initialize LAME
    // Create global flags object
    lame_global_flags* gfp = lame_init();

    // Set global flags
    // Input stream flags
    lame_set_num_samples(gfp, BUFFER_SIZE); /* number of samples in the input stream */
    lame_set_in_samplerate(gfp, 44100);     /* sample rate is 44.1k or 48k */
    lame_set_num_channels(gfp, 2);          /* 1 for mono, 2 for joint/stereo */
    // Output stream flags TODO: find optimal settings
    lame_set_out_samplerate(gfp, 44100);    /* sample rate is 44.1k or 48k */
    lame_set_mode(gfp, JOINT_STEREO);       /* mono, stereo, joint stereo */
    lame_set_quality(gfp, 0);               /* 0 = best 2 = high 5 = med 7 = low */
    lame_set_brate(gfp, 320);               /* max bitrate = 320 kbps */
    lame_set_compression_ratio(gfp, 11);    /* default compression ratio = 11 */

    // int ret_code = lame_init_params(gfp);
    // Check for problems: ret_code must be >= 0
    std::cout << "LAME init (>= 0 is success): " << lame_init_params(gfp) << std::endl;

    return gfp;
}

void ConvertWavToMp3(const std::string inputFile, const std::string outputFile) {
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

int main() {

    /* Attempt 1: read the API docs
    SF_INFO soundfile_info = {0, 0, 0, 0, 0, 0};
    std::cout << "Format check: " << sf_format_check(&soundfile_info) << std::endl;

    SNDFILE* soundfile = nullptr;

    if (sf_format_check(&soundfile_info)) {
        soundfile = sf_open(
            "test-audio/wav/BGE_170_G_Stars_Arp_Pad.wav",
            SFM_READ,
            &soundfile_info
        );

        if (!soundfile) {
            std::string error = sf_strerror(soundfile);
            std::cout << "libsndfile error: " << error << std::endl;
        }
        
        std::cout << "Sound file memory address: " << soundfile << std::endl;

        sf_close(soundfile);
    } else {
        std::cout << "Soundfile info not valid." << std::endl;
    } */

    /* Attempt 2: */

    // WAV converted from 24 to 16 bits per sample TODO: detect bitrate
    std::string inputFile = "test-audio/wav/BGE_170_G_Stars_Arp_Pad_16.wav";
    std::string outputFile = "test-audio/out/out1.mp3";

    PrintFileInfo(inputFile);
    
    ConvertWavToMp3(inputFile, outputFile);

    return 0;
}
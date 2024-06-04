#include <iostream>
#include <string>
#include "lame.h"
#include "sndfile.hh"

#define BUFFER_LENGTH 1024

SndfileHandle read_file (std::string filename) {
    // Create buffer
    static short buffer[BUFFER_LENGTH]; // would prefer to use u_int16_t

    // Get file handle
    SndfileHandle file = SndfileHandle(filename);

    // Read from file into buffer
    file.read(buffer, BUFFER_LENGTH);

    return file;
}

int main() {
    //
    // Open and close a wav file using libsndfile
    //

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

    /* Attempt 2: follow example on github (https://github.com/libsndfile/libsndfile/blob/master/examples/sndfilehandle.cc)*/
    std::string filename = "test-audio/wav/BGE_170_G_Stars_Arp_Pad.wav";
    SndfileHandle file = read_file(filename);

    std::cout << "Opened file " << filename << std::endl;
    std::cout << "Format: 0x" << std::hex << file.format() << std::endl;
    std::cout << "Sample rate: " << std::dec << file.samplerate() << std::endl;
    std::cout << "Channels: " << file.channels() << std::endl;
    // std::cout << "Format check: (Signed 24-bit PCM WAV, 2 channels, 44100Hz samplerate): " << file.formatCheck(0x010003, 2, 44100) << std::endl;    

    //
    // mp3 conversion with LAME
    //

    // Initialize LAME
    // Create global flags object
    lame_global_flags *gfp = lame_init();

    // Set global flags
    lame_set_num_channels(gfp, 2);      /* 1 for mono, 2 for joint/stereo */
    lame_set_in_samplerate(gfp, 44100); /* sample rate is 44.1k, 48k, 96k */
    lame_set_brate(gfp, 320);           /* max bitrate = 320 kbps */
    lame_set_mode(gfp, JOINT_STEREO);   /* mono, stereo, joint stereo */
    lame_set_quality(gfp, 0);           /* 0 = best 2 = high 5 = med 7 = low */

    std::cout << "LAME init global flags memory address: " << gfp << std::endl;

    int ret_code = lame_init_params(gfp);

    // Check for problems: ret_code must be >= 0
    std::cout << "LAME init return code: " << ret_code << std::endl;

    // Close LAME
    lame_close(gfp);

    return 0;
}
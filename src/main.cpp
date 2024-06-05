#include <iostream>
#include <stdio.h>
#include <string>
#include "../include/lame.h"
#include "sndfile.hh"

constexpr size_t BUFFER_SIZE = 8192; // 8kb buffer

void ConvertWavToMp3() {
    int read, write;

    FILE *pcm = fopen("file.pcm", "rb");
    FILE *mp3 = fopen("file.mp3", "wb");

    const int PCM_SIZE = 8192;
    const int MP3_SIZE = 8192;

    short int pcm_buffer[PCM_SIZE*2];
    unsigned char mp3_buffer[MP3_SIZE];

    lame_t lame = lame_init();
    lame_set_in_samplerate(lame, 44100);
    lame_set_VBR(lame, vbr_default);
    lame_init_params(lame);

    do {
        read = fread(pcm_buffer, 2*sizeof(short int), PCM_SIZE, pcm);
        if (read == 0)
            write = lame_encode_flush(lame, mp3_buffer, MP3_SIZE);
        else
            write = lame_encode_buffer_interleaved(lame, pcm_buffer, read, mp3_buffer, MP3_SIZE);
        fwrite(mp3_buffer, write, 1, mp3);
    } while (read != 0);

    lame_close(lame);
    fclose(mp3);
    fclose(pcm);
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
    SndfileHandle pcm_file = SndfileHandle(filename);

    std::cout << "Opened file " << filename << std::endl;
    std::cout << "Format: 0x" << std::hex << pcm_file.format() << std::endl;
    std::cout << "Sample rate: " << std::dec << pcm_file.samplerate() << std::endl;
    std::cout << "Channels: " << pcm_file.channels() << std::endl;
    std::cout << "file.frames(): " << pcm_file.frames() << std::endl;
    // std::cout << "Format check: (Signed 24-bit PCM WAV, 2 channels, 44100Hz samplerate): "
    //              << file.formatCheck(0x010003, 2, 44100) << std::endl;
    // std::cout << "Last sample in buffer: " << buffer[BUFFER_SIZE - 1] << std::endl;

/*     for (size_t i = 0; i < BUFFER_SIZE; ++i) {
        std::cout << buffer[i] << " ";
    }
    std::cout << "END" << std::endl; */

    //
    // mp3 conversion with LAME
    // Follows instructions in the "Lame API" section of lame.h
    //

    // Initialize LAME
    // Create global flags object
    lame_global_flags *gfp = lame_init();

    // Set global flags
    // Input stream flags
    lame_set_num_samples(gfp, BUFFER_SIZE); /* number of samples in the input stream */
    lame_set_in_samplerate(gfp, 44100);     /* sample rate is 44.1k or 48k */
    lame_set_num_channels(gfp, 2);          /* 1 for mono, 2 for joint/stereo */

    // Processing flags
    lame_set_scale(gfp, 1.0f);              /* default = 1.0f */
    lame_set_analysis(gfp, 0);              /* collect mp3 frame analysis data */

    // Output flags
    lame_set_out_samplerate(gfp, 44100);    /* sample rate is 44.1k or 48k */
    lame_set_mode(gfp, JOINT_STEREO);       /* mono, stereo, joint stereo */
    lame_set_quality(gfp, 0);               /* 0 = best 2 = high 5 = med 7 = low */
    lame_set_brate(gfp, 320);               /* max bitrate = 320 kbps */
    lame_set_compression_ratio(gfp, 11);    /* default compression ratio = 11 */

    std::cout << "LAME init global flags memory address: " << gfp << std::endl;

    int ret_code = lame_init_params(gfp);

    // Check for problems: ret_code must be >= 0
    std::cout << "LAME init return code (>= 0 is success): " << ret_code << std::endl;

    // Create pcm read-in buffer
    static int pcm_buffer[BUFFER_SIZE * 2]; // double buffer size for
    // Read from file into buffer
    int read = pcm_file.read(pcm_buffer, BUFFER_SIZE * 2);
    std::cout << "PCM buffer length: " << sizeof(pcm_buffer) << " bytes." << std::endl;

    // Create mp3 post-convert buffer
    static unsigned char mp3_buffer[BUFFER_SIZE];
    std::cout << "MP3 buffer length: " << sizeof(mp3_buffer) << " bytes." << std::endl;

    FILE* mp3_file = fopen("test-audio/out/out.mp3", "wb");

    int write = 0;
    while (read != 0) {
        // Write 
        write = lame_encode_buffer_interleaved_int(gfp, pcm_buffer, read, mp3_buffer, BUFFER_SIZE);
        std::cout << "mp3_buffer[0]: " << mp3_buffer[0] << std::endl;
        fwrite(mp3_buffer, write, 1, mp3_file);
        read = pcm_file.read(pcm_buffer, BUFFER_SIZE * 2);
    }
    // Flush buffer to output file
    write = lame_encode_flush(gfp, mp3_buffer, BUFFER_SIZE);
    std::cout << "Success? " << write << std::endl;

    // Close output file
    fclose(mp3_file);

    // Close LAME
    lame_close(gfp);

    return 0;
}
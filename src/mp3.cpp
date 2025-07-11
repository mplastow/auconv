// mp3.cpp - auconv

#include <mp3.hpp>

#include <iostream>

#include <defines.hpp>
#include <types.hpp>

namespace auconv {

lame_global_flags* initLameWithFlags()
{
    // Initialize LAME
    // Create global flags object
    lame_global_flags* gfp = lame_init();

    // Set global flags
    // Input stream flags
    lame_set_num_samples(gfp, BUFFER_SIZE); /* number of samples in the input stream */
    lame_set_in_samplerate(gfp, HZ_44100);  /* sample rate is 44.1k or 48k */
    lame_set_num_channels(gfp, 2);          /* 1 for mono, 2 for joint/stereo */
    // Output stream flags TODO: find optimal settings
    lame_set_out_samplerate(gfp, HZ_44100);                  /* sample rate is 44.1k or 48k */
    lame_set_mode(gfp, JOINT_STEREO);                        /* mono, stereo, joint stereo */
    lame_set_quality(gfp, 0);                                /* 0 = best 2 = high 5 = med 7 = low */
    lame_set_brate(gfp, BITRATE_320);                        /* max bitrate = 320 kbps */
    lame_set_compression_ratio(gfp, MP3_COMP_RATIO_DEFAULT); /* default compression ratio = 11 */

    // int ret_code = lame_init_params(gfp);
    // Check for problems: ret_code must be >= 0
    std::cout << "LAME init (>= 0 is success): " << lame_init_params(gfp) << '\n';

    return gfp;
}

void convertWavToMp3(Path const& inputFile, Path const& outputFile)
{
    // mp3 conversion with libmp3lame
    // See https://stackoverflow.com/a/2496831/22896065
    lame_global_flags* gfp = initLameWithFlags();

    // Create pcm read-in buffer
    static short int pcm_buffer[PCM_BUFFER_SIZE]; // double buffer size due to interlaced channels
    // Read from file into buffer
    int read = 0;
    std::cout << "PCM buffer length: " << sizeof(pcm_buffer) << " bytes." << '\n';

    // Create mp3 buffer: char type used for size
    static unsigned char mp3_buffer[MP3_BUFFER_SIZE];
    std::cout << "MP3 buffer length: " << sizeof(mp3_buffer) << " bytes." << '\n';

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

} // namespace auconv
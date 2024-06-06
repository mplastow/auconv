#include <iostream>
#include <stdio.h>
#include <string>

#include <lame.h>
#include <sndfile.hh>
#include <sndfile.h>

constexpr int BUFFER_SIZE = 8192; // 8kb buffer
constexpr int MP3_BUFFER_SIZE = 8192; // 8kb buffer
constexpr int PCM_BUFFER_SIZE = 16384; // 16kb buffer

void PrintFileInfo(std::string inputFile) {
    // Print file properties with libsndfile
    SndfileHandle inputFileHandle = SndfileHandle(inputFile);
    if (inputFileHandle.error()) {
        std::cout << "Could not open input file: " << inputFileHandle.strError() << std::endl;
        return;
    } else {
        std::cout << "Opened input file: " << inputFile << std::endl;
        std::cout << "Format: 0x" << std::hex << inputFileHandle.format() << std::endl;
        std::cout << "Sample rate: " << std::dec << inputFileHandle.samplerate() << std::endl;
        std::cout << "Channels: " << inputFileHandle.channels() << std::endl;
        std::cout << "Frames in file: " << inputFileHandle.frames() << std::endl;
        std::cout << "Check against format: " << inputFileHandle.formatCheck(inputFileHandle.format(), inputFileHandle.channels(), inputFileHandle.samplerate()) << std::endl;
        std::cout << "" << std::endl;
    }

    return;
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

void ConvertAudioFile(std::string inputFile, std::string outputFile, int outputFormat) {
    SndfileHandle inputFileHandle = SndfileHandle(inputFile);
    if (inputFileHandle.error()) {
        std::cout << "Could not open input file: " << inputFileHandle.strError() << std::endl;
        return;
    } else {
        std::cout << "Opened input file: " << inputFile << std::endl;
        std::cout << "Format: 0x" << std::hex << inputFileHandle.format() << std::endl;
        std::cout << "Sample rate: " << std::dec << inputFileHandle.samplerate() << std::endl;
        std::cout << "Channels: " << inputFileHandle.channels() << std::endl;
        std::cout << "Frames in file: " << inputFileHandle.frames() << std::endl;
        std::cout << "" << std::endl;
    }

    SndfileHandle outputFileHandle = SndfileHandle(
        outputFile,
        SFM_WRITE,
        outputFormat,
        inputFileHandle.channels(),
        inputFileHandle.samplerate()
    );
    if (outputFileHandle.error()) {
        std::cout << "Could not open output file: " << outputFileHandle.strError() << std::endl;
        std::cout << "Check against specified format (1 = pass): " << outputFileHandle.formatCheck(outputFormat, inputFileHandle.channels(), inputFileHandle.samplerate()) << std::endl;
        return;
    } else {
        std::cout << "Opened output file: " << outputFile << std::endl;
        std::cout << "Format: 0x" << std::hex << outputFileHandle.format() << std::endl;
        std::cout << "Sample rate: " << std::dec << outputFileHandle.samplerate() << std::endl;
        std::cout << "Channels: " << outputFileHandle.channels() << std::endl;
        std::cout << "" << std::endl;
    }

    static short buffer[BUFFER_SIZE];

    sf_count_t read = 1;
    sf_count_t write = 1;

    while (read != 0) {
        read = inputFileHandle.read(buffer, BUFFER_SIZE);
        // std::cout << "Read: " << read << std::endl;
        
        if (read != 0) {
            write = outputFileHandle.write(buffer, read);
            // std::cout << "Write: " << write << std::endl;
        }
    }

    std::cout << "\nFinished file conversion.\nReached end of input file. Last read: " << read << std::endl;
    if (write == 0) {
        std::cout << "Warning: Last write was 0 bytes! Last write: " << write << "\n" << std::endl;
    } else {
        std::cout << "Last write: " << write << "\n\n" << std::endl;
    }

    return; 
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
    std::cout << "\n----------------\n" << std::endl;

    // Attempt to read from an mp3 file (non-working)
    PrintFileInfo("test-audio/mp3/1-01 Wishful Thinking.mp3");
    std::cout << "\n----------------\n" << std::endl;

    /*  
    FLAC example: SF_FORMAT_FLAC | SF_FORMAT_PCM_16
    MP3 example (non-working): SF_FORMAT_MPEG | SF_FORMAT_MPEG_LAYER_III
    */
    ConvertAudioFile(
        "test-audio/wav/perc/BGE_170_Rising_Tops_2.wav",
        "test-audio/out/BGE_170_Rising_Tops_2.flac",
        SF_FORMAT_FLAC | SF_FORMAT_PCM_16
    );

    return 0;
}
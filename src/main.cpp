#include <iostream>
#include "lame.h"
#include "sndfile.hh"

int main() {

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

    std::cout << gfp << std::endl;

    int ret_code = lame_init_params(gfp);

    // Check for problems: ret_code must be >= 0
    std::cout << ret_code << std::endl;

    return 0;
}
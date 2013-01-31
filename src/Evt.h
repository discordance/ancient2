#ifndef EVT
#define EVT

// evt
struct Evt
{
    int track; // origin track
    int status; // note on or off
    int channel; // channel
    int pitch; // pitch mapped
    int vel; // velocity
};

#endif
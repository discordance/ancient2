//
//  ConfTrack.h
//  ancient2
//
//  Created by nunja on 11/22/12.
//
//

#ifndef CONF_TRACK
#define CONF_TRACK

#include <map>

using namespace std;

struct ConfTrack {
    // PARAMETTERS
    int track_id;
    int track_size; // conf: disk + iface
    int track_onsets; // conf: disk + iface
    float track_rotation; // conf: disk + iface
    float track_evenness;
    unsigned int velocity_mode; // conf: disk + iface
    int velocity_min;
    int velocity_max;
    
    // euclidean properties
    // shadow beat incorrectness
    float euclid_bias; // conf: disk + iface
    // mutations
    float euclid_density; // conf: disk + iface
    float euclid_permutation; // conf: disk + iface
    float euclid_evolution_rate; // conf: disk + iface
    float euclid_permutation_rate; // conf: disk + iface
};

#endif

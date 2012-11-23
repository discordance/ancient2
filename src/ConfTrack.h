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
    int m_track_size; // conf: disk + iface
    int m_track_onsets; // conf: disk + iface
    float m_track_rotation; // conf: disk + iface
    unsigned int m_velocity_mode; // conf: disk + iface
    
    // euclidean properties
    // shadow beat incorrectness
    float m_euclid_bias; // conf: disk + iface
    // mutations
    float m_euclid_density; // conf: disk + iface
    float m_euclid_permutation; // conf: disk + iface
    float m_euclid_evolution_rate; // conf: disk + iface
    float m_euclid_permutation_rate; // conf: disk + iface
};

#endif

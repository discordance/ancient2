//
//  DTrack.h
//  ancient2
//
//  Created by nunja on 11/20/12.
//
//

#ifndef DTRACK
#define DTRACK

#include <vector>
#include "Step.h"
#include "Euclid.h"

using namespace std;

// DTrack stands for Dynamic Track
class DTrack {
    
public:
    DTrack(int track_id, int track_pitch, int track_size = 16);
    ~DTrack();
    
    void generate();
    
protected:
    
    // general properties
    int m_track_id;
    int m_track_pitch;
    int m_track_size;
    int m_track_onsets;
    float m_track_rotation;
    vector<float> m_track_groove;
    vector<Step> m_track_current; // current form of the track after transforms
    unsigned int m_velocity_mode;
    vector<int> m_intervals; // stores the intervals between onsets
    
    // euclidean properties
    bool m_is_euclidean; // constrained to be euclidean.
    float m_euclid_bias; // shadow beat incorrectness
    // mutations
    float m_euclid_density;
    float m_euclid_permutation;
    float m_euclid_evolution_rate;
    float m_euclid_permutation_rate;
};

#endif

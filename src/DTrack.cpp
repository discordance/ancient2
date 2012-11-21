//
//  DTrack.cpp
//  ancient2
//
//  Created by nunja on 11/20/12.
//
//

#include "DTrack.h"


DTrack::DTrack(int track_id, int track_pitch, int track_size)
{
    m_track_id = track_id;
    m_track_pitch = track_pitch;
    m_track_size = track_size;
    
    if(track_size < 4 || track_size > 64)
    {
        m_track_size = 16;
    }
    
    m_track_onsets = 0;
    m_track_rotation = 0.;
    m_track_groove = vector<float>(0.,m_track_size);
    //vector<Step> m_track_current; // current form of the track after transforms
    m_velocity_mode = Euclid::VEL_LINE;
    
    m_is_euclidean = false;
    m_euclid_bias = 0.; // shadow beat incorrectness

    m_euclid_density = 0.;
    m_euclid_permutation = 0.;
    m_euclid_evolution_rate = 1.;
    m_euclid_permutation_rate = 1.;
}

void DTrack::generate()
{
    
}
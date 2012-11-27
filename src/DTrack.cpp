//
//  DTrack.cpp
//  ancient2
//
//  Created by nunja on 11/20/12.
//
//

#include "DTrack.h"

DTrack::DTrack(int track_id, int track_size)
{
    m_track_id = track_id;
    m_track_size = track_size;
    
    if(track_size < 4 || track_size > 64)
    {
        m_track_size = 16;
    }
    
    m_track_onsets = 0;
    m_track_rotation = 0.;
    m_track_groove = vector<float>(m_track_size,0.);
    //vector<Step> m_track_current; // current form of the track after transforms
    m_velocity_mode = Euclid::VEL_STATIC;
    m_velocity_min = 0;
    m_velocity_max = 13;

    m_euclid_bias = 0.; // shadow beat incorrectness

    m_euclid_density = 0.;
    m_euclid_permutation = 0.;
    m_euclid_evolution_rate = 1.;
    m_euclid_permutation_rate = 1.;
    
    // current default
    m_track_current = generate_empty_phr(m_track_size);
    
    m_vanilla_beat = vector<bool>(m_track_size,false); // conf: disk
    m_shadow_beat = vector<bool>(m_track_size,false);; // conf: disk
    m_alternation_beat = vector<bool>(m_track_size,false); // conf: disk
    m_velocities = vector<int>(m_track_size,0);// conf: disk
    
}

vector<Step> * DTrack::get_current()
{
    return &m_track_current;
}

void DTrack::generate(ConfTrack conf)
{
    
}

void DTrack::set_swing(float swing)
{
    if(swing >= 1){ swing = 0.99; }
    if(swing <= -1){ swing = -0.99; }
    vector<float> groove;
    for(int i = 0; i < 4 ; ++i)
    {
        if(i % 2 != 0)
        {
            groove.push_back(swing);
        }
        else
        {
            groove.push_back(0);
        }
    }
    m_track_groove = groove;
    update_groove();
}

void DTrack::set_groove(vector<float> groove)
{
    m_track_groove = groove;
    update_groove();
}

void DTrack::set_xor_variation(float thres, bool mode)
{

}

void DTrack::set_jaccard_variation(float thres, bool mode)
{

}

void update_groove()
{

}

vector<Step> DTrack::generate_empty_phr(int size)
{
    vector<Step> res;
    for(int i=0; i < size; ++i)
    {
        Step st;
        st.vel = 0;
        st.dur = 1;
        st.lock = FALSE;
        st.ctrl = map<int,float>();
        st.drift = 0.0;
        st.chance = 1.;
        res.push_back(st);
    }
    return res;
}
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
    m_track_evenness = 1.;
    m_track_groove = vector<float>(4,0.); // @TODO find an extension algo
    //vector<Step> m_track_current; // current form of the track after transforms
    m_velocity_mode = Euclid::VEL_STATIC;
    m_velocity_min = 0;
    m_velocity_max = 13;

    m_euclid_bias = 0.; // shadow beat incorrectness

    m_euclid_density = 0.5;
    m_euclid_permutation = 0.;
    m_euclid_evolution_rate = 0.2;
    m_euclid_permutation_rate = 1.;
    
    // current default
    m_track_current = generate_empty_phr(m_track_size);
    
    m_vanilla_beat = vector<bool>(m_track_size,false); // conf: disk
    m_shadow_beat = vector<bool>(m_track_size,false);; // conf: disk
    m_alternation_beat = vector<bool>(m_track_size,false); // conf: disk
    m_permutation_places = vector<bool>(m_track_size,false); // conf: disk
    m_velocities = vector<int>(0,0);// conf: disk
    
}

void DTrack::set_conf(ConfTrack conf)
{
    m_track_id = conf.track_id;
    m_track_onsets = conf.track_onsets;
    m_track_rotation = conf.track_rotation;
    m_track_size = conf.track_size;
    m_track_evenness = conf.track_evenness;
    m_velocity_mode = conf.velocity_mode;
    m_velocity_max = conf.velocity_max;
    m_velocity_min = conf.velocity_min;
    m_euclid_bias = conf.euclid_bias;
    m_euclid_density = conf.euclid_density;
    m_euclid_permutation = conf.euclid_permutation;
    m_euclid_evolution_rate = conf.euclid_evolution_rate;
    m_euclid_permutation_rate = conf.euclid_permutation_rate;
}

ConfTrack DTrack::get_conf()
{
    ConfTrack conf = ConfTrack();
    conf.track_id = m_track_id;
    conf.track_onsets = m_track_onsets;
    conf.track_rotation = m_track_rotation;
    conf.track_size = m_track_size;
    conf.track_evenness = m_track_evenness;
    conf.velocity_mode = m_velocity_mode;
    conf.velocity_max = m_velocity_max;
    conf.velocity_min = m_velocity_min;
    conf.euclid_bias = m_euclid_bias;
    conf.euclid_density = m_euclid_density;
    conf.euclid_permutation = m_euclid_permutation;
    conf.euclid_evolution_rate = m_euclid_evolution_rate;
    conf.euclid_permutation_rate = m_euclid_permutation_rate;
    return conf;
}

vector<int>  DTrack::get_velocities()
{
    vector<int> res;
    vector<Step>::iterator step;
    for(step = m_track_current.begin(); step != m_track_current.end(); ++step)
    {
        res.push_back(step->vel);
    }
    return res;
}

vector<Step> * DTrack::get_current()
{
    return &m_track_current;
}

void DTrack::generate(ConfTrack conf)
{
    set_conf(conf);
    
    if(!conf.track_onsets) // no need if no onsets haha
    {
        return;
    }
    // get the beat motherfucker
    if(conf.track_evenness >= 1. )
    {
        m_vanilla_beat = Euclid::gen_bjorklund(m_track_size, m_track_onsets);
    }
    else
    {
        m_vanilla_beat = Euclid::gen_permuted_intervals(m_track_size, m_track_onsets);
    }
    
    Euclid::rotate_beat(m_vanilla_beat, m_track_rotation);

    // shadow beat and alternation
    m_shadow_beat = Euclid::shadow(m_vanilla_beat, m_euclid_bias);
    m_alternation_beat = Euclid::alternation(m_vanilla_beat, 1, 2, m_euclid_bias);
    m_permutation_places = m_shadow_beat;
    
    // prune alternation et shadow
    Euclid::prune(m_shadow_beat, 1-m_euclid_evolution_rate);
    Euclid::prune(m_alternation_beat, 1-m_euclid_evolution_rate);

    switch (m_velocity_mode) {
        
        case Euclid::VEL_STATIC:
            m_velocities = vector<int>(m_track_onsets,m_velocity_max);
            break;
            
        case Euclid::VEL_LINE:
            m_velocities = Euclid::discrete_line(m_track_onsets, m_velocity_max, m_velocity_min);
            break;
            
        case Euclid::VEL_SINE:
            m_velocities = Euclid::discrete_sine(m_track_onsets, m_velocity_max, m_velocity_min);
            break;
            
        case Euclid::VEL_RAMP:
            m_velocities = Euclid::discrete_ramp(m_track_onsets, m_velocity_max, m_velocity_min);
            break;
            
        case Euclid::VEL_RAND:
            m_velocities = Euclid::discrete_random(m_track_onsets, m_velocity_max, m_velocity_min);
            break;
            
        default:
            break;
    }
    // assemble vels and beat
    // @ TODO permutation rate and evolution rate
    vector<int> vels = Euclid::assemble(m_vanilla_beat, m_velocities);
    m_track_current = generate_phr(vels, m_track_groove);
    evolve(m_euclid_density, m_euclid_permutation);
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



void DTrack::evolve(float level, float permute)
{
    m_euclid_density = level;
    m_euclid_permutation = permute;
    
    vector<bool> permuted = m_vanilla_beat;
    vector<bool> shad_permuted = m_shadow_beat;
    vector<bool> alter_permuted = m_alternation_beat;
    
    Euclid::permute(permuted,  Euclid::shadow(m_vanilla_beat,m_euclid_bias), permute);
    Euclid::permute(shad_permuted, Euclid::shadow(m_shadow_beat,m_euclid_bias), permute);
    Euclid::permute(alter_permuted, Euclid::shadow(m_alternation_beat,m_euclid_bias), permute);
    
    vector<int> vels = Euclid::assemble(permuted, m_velocities);
    
    float lrate = 0.;
    if(level < 0.5)
    {
        lrate = ofMap(level,0.5,0.,0.,1.);
        lrate = Sine::easeIn(lrate, 0., lrate, 1.);
        lrate = 1- lrate;
        // alternation
        for(vector<bool>::iterator alt = alter_permuted.begin(); alt != alter_permuted.end(); ++alt)
        {
            int ct = alt - alter_permuted.begin();
            if (*alt)
            {
                vels.at(ct) = vels.at(ct) * lrate;
            }
        }
    }
    if(level > 0.5)
    {
        lrate = ofMap(level,0.5,1.,0.,1.);
        
        lrate = Sine::easeIn(lrate, 0., lrate, 1.);
        
        for(vector<bool>::iterator shad = shad_permuted.begin(); shad != shad_permuted.end(); ++shad)
        {
            int ct = shad - shad_permuted.begin();
            if(*shad)
            {
                vels.at(ct) = m_velocity_max * lrate * 0.9; // attenuate
            }
        }
    }
    
    m_track_current = generate_phr(vels, m_track_groove);
     
}

void DTrack::update_groove()
{
    vector<Step>::iterator step;
    for(step = m_track_current.begin(); step != m_track_current.end(); ++step)
    {
        step->drift = m_track_groove.at((step - m_track_current.begin())%4);
    }
}

vector<Step> DTrack::generate_phr(vector<int> & vels, vector<float> & groove)
{
    vector<Step> res;
    vector<int>::iterator vel;
    for(vel = vels.begin(); vel != vels.end(); ++vel)
    {
        // create the step
        Step st;
        st.vel = *vel;
        st.dur = 1;
        st.lock = FALSE;
        st.drift = groove.at((vel - vels.begin())%4);
        st.ctrl = map<int,float>();
        st.chance = 1.;
        
        res.push_back(st);
    }
    return res;
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
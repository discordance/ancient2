//
//  Ancient.cpp
//  ancientSequencer
//
//  Created by Nunja on 8/12/12.
//  Copyright 2012 ISM. All rights reserved.
//

#include "Ancient.h"

Ancient::Ancient()
{
    m_hold_variation = false;
    m_on_variation = false;
    m_swing = 0.;
    m_xor_variation = 0.;
    m_jacc_variation = 0;
    m_level = 2;
    m_variat = 0;
    
    // init eight tracks
    for(int i = 0; i < 8 ; i++)
    {
        DTrack tr(i,16);
        m_tracks.push_back(tr);
    }
    
    m_groove = vector<float>(Seq::SEQ_LOOP_SIZE,0.);
    
}

vector<float> Ancient::get_groove()
{
    return m_groove;
}

vector<DTrack>* Ancient::get_tracks()
{
    return &m_tracks;
}

ConfTrack Ancient::get_track_conf(int idx)
{
    return m_tracks.at(idx).get_conf();
}
/*
vector<int> Ancient::get_track_velocities(int idx)
{
    vector<int> vels;
    if(lock())
    {
        vels = m_tracks.at(idx).get_velocities();
        unlock();
    }
    return vels;
}
 */

void Ancient::notify(int quav)
{
    m_quav = quav;
    m_bar = floor(quav/16);
    m_beat = floor(quav/4);
    
    if(m_beat % 8 == 0)
    {
        if(m_hold_variation && !m_on_variation)
        {
            if(m_xor_variation > 0)
            { 
                set_xor_variation(m_xor_variation);
            }
            if(m_jacc_variation > 0)
            {
                set_jaccard_variation(m_jacc_variation);
            }
            m_on_variation = !m_on_variation;
        }
    }
    else
    {
        if(m_hold_variation && m_on_variation)
        {
            m_on_variation = !m_on_variation;
            m_tasks.push_back("reset_variation");
        }
    }
}

void Ancient::init()
{
    startThread();
}

void Ancient::generate(ConfTrack conf)
{
    m_generations.push_back(conf);
}

void Ancient::load_preset(ofxXmlSettings * settings)
{
    m_preset = settings;
    m_tasks.push_back("load");
}

void Ancient::set_jaccard_variation(float thres)
{
    m_jacc_variation = thres;
    m_xor_variation = 0.; // not two at the same time
    m_tasks.push_back("jacc_var");
    //startThread();
} 

void Ancient::set_xor_variation(float ratio)
{
    m_xor_variation = ratio;
    m_jacc_variation = 0.; // not two at the same time
    m_tasks.push_back("xor_var");
}

void Ancient::set_swing(float swg)
{
    m_swing = swg;
    m_tasks.push_back("swing");
}

void Ancient::generate_pink(float corr)
{
    vector<float> master = RandUtils::get_pink_serie(64,0.2);
    for(int i = 0; i < m_tracks.size(); ++i)
    {
        vector<float> res;
        RandUtils::correlateRnd(corr, 0.6, master, res);
        m_track_pink.push_back(res);
    }   
    m_tasks.push_back("pink");
}

void Ancient::set_evolution(float level, float variat)
{
    m_level = level;
    m_variat = variat;
    m_tasks.push_back("evolve");
}

void Ancient::set_hold_variation(bool hold)
{
    m_hold_variation = hold;
    if(!m_hold_variation)
    {
        m_on_variation = m_hold_variation;
        m_jacc_variation = 0;
        m_xor_variation = 0;
        set_xor_variation(m_xor_variation);
        set_jaccard_variation(m_jacc_variation);
    }
}

void Ancient::set_seq(Seq *seq)
{
    m_seq = seq;
    m_seq->set_ancient(this);
    //m_seq->update_drum_tracks(&m_tracks);
}
//--------------------------
void Ancient::threadedFunction()
{
    while( isThreadRunning() != 0 )
    {
        bool update_needed = false;
        vector< vector<int> > vels;
        std::vector<DTrack>::iterator track;
        if( lock())
        {
            // refresh velocities
            for(track = m_tracks.begin(); track != m_tracks.end(); ++track)
            {
                vels.push_back(track->get_velocities());
            }
            
            if(m_tasks.size())
            {
                string task = m_tasks.at(0);
                m_tasks.erase(m_tasks.begin());
                
                vector< vector<float> > tr_pink;

                // all tracks
                for(track = m_tracks.begin(); track != m_tracks.end(); ++track) 
                {
                    if(task == "pink")
                    {
                        if(m_track_pink.size())
                        {
                            track->set_groove(m_track_pink.at(track-m_tracks.begin()));
                        }
                    }
                    
                    if(task == "jacc_var")
                    {    
                        track->set_jaccard_variation(m_jacc_variation);
                    }
                    else if(task == "xor_var")
                    {
                        track->set_xor_variation(m_xor_variation);
                    }
                    else if(task == "swing")
                    {
                        m_groove = Seq::classic_swing(m_swing);
                    }
                    else if(task == "evolve")
                    {
                        track->evolve(m_level, m_variat);
                    }
                    else if (task == "reset_variation")
                    {
                        track->set_xor_variation(0);
                        track->set_jaccard_variation(0);
                    }
                    else if (task == "load")
                    {
                        track->load_preset(m_preset);
                    }
                }
                if (task == "load")
                {
                    m_preset->popTag();
                    m_preset->pushTag("global");
                    m_preset->pushTag("seq_groove");
                    int ct = m_preset->getNumTags("i");
                    vector<float> groove;
                    for(int i = 0; i < ct; ++i)
                    {
                        groove.push_back(m_preset->getValue("i", 0., i));
                    }
                    m_groove = groove;
                    
                    ofSendMessage("preset_loaded");
                }
                update_needed = true;
            }
            
            // generate
            if(m_generations.size())
            {
                ConfTrack conf = m_generations.at(0);
                m_generations.erase(m_generations.begin());
                m_tracks.at(conf.track_id).generate(conf);
            }
            update_needed = true;
        }
        
        static RefreshEvent refresh_e;
        refresh_e.message = "refresh";
        refresh_e.velocities = vels;
        // refresh vels;
        ofNotifyEvent(RefreshEvent::events, refresh_e);
        
        map< string, vector<int> > pitches;
        int max_ticks;
        if(m_seq->lock())
        {
            pitches = m_seq->get_pitches();
            max_ticks = m_seq->get_max_ticks();
            m_seq->unlock();
        }
        if(update_needed)
        {
            vector< vector<Evt> > result = Seq::generate_events(&m_tracks, m_groove, pitches, max_ticks);
            m_seq->update(result);
            update_needed = false;
        }
        
        notify(m_seq->get_quav());
        unlock();
        ofSleepMillis(5);
    } 
}
    

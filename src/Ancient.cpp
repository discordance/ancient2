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
    m_processing = false;
    m_level = 2;
    m_variat = 0;
    
    // init eight tracks
    for(int i = 0; i < 8 ; i++)
    {
        DTrack tr(i,16);
        m_tracks.push_back(tr);
    }
    
    // pitch map stuff
    /*
    static const int parr[] = {
                              36,// kick
                              38,// snare1
                              40,// snare2
                              42,// chh
                              46,// ohh
                              47,// perc3   
                              48,// perc2
                              49 // os
                              };
    
    vector<int> pitchmap (parr, parr + sizeof(parr) / sizeof(parr[0]) );
    assign_pitchmap(pitchmap);
    */
    
    /*
    // type map stuff
    static const int tarr[] = {
        Gaia::MODE_LOW_PERC,// kick
        Gaia::MODE_SNARE,// snare1
        Gaia::MODE_SNARE,// snare2
        Gaia::MODE_HITHAT,// chh
        Gaia::MODE_OVERHEAD,// ohh
        Gaia::MODE_PERC,// perc3
        Gaia::MODE_PERC,// perc2
        Gaia::MODE_ONE_SHOT // os
    };
    vector<int> typemap (tarr, tarr + sizeof(tarr) / sizeof(tarr[0]) );
    assign_typemap(typemap);
     */
}

vector<DTrack>* Ancient::get_tracks()
{
    return &m_tracks;
}

ConfTrack Ancient::get_track_conf(int idx)
{
    return m_tracks.at(idx).get_conf();
}

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

bool Ancient::is_processing()
{
    return m_processing;
}

void Ancient::notify(int quav)
{
    m_quav = quav;
    m_bar = floor(quav/16);
    m_beat = floor(quav/4);
    
    if(m_beat % 2 != 0)
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

void Ancient::update()
{
   // cout << m_tasks.size() << " " << m_generations.size() << endl;
    if(m_tasks.size() || m_generations.size())
    {
        startThread();
    }
}

void Ancient::generate(ConfTrack conf)
{
    m_generations.push_back(conf);
    //startThread();
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
void Ancient::set_groove(vector<float> groove)
{
    m_groove = groove;
    m_tasks.push_back("groove");
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
    m_seq->update_drum_tracks(&m_tracks);
}

// protected --------------------------------------
/*
void Ancient::assign_pitchmap(vector<int> pitchmap)
{
    if(m_tracks.size())
    {
        for(vector<Trak>::iterator track = m_tracks.begin(); track != m_tracks.end(); ++track)
        {
            int ct = track - m_tracks.begin();
            if(pitchmap.size()-1 >= ct)
            {
                track->set_pitch(pitchmap[ct]);
            }
        }
    }   
}
 */

/*
void Ancient::assign_typemap(vector<int> typemap)
{
    if(m_tracks.size())
    {
        for(vector<Trak>::iterator track = m_tracks.begin(); track != m_tracks.end(); ++track)
        {
            int ct = track - m_tracks.begin();
            if(typemap.size()-1 >= ct)
            {
                track->m_mode = typemap[ct];
            }
        }
    }
}
*/
//--------------------------
void Ancient::threadedFunction()
{
    while( isThreadRunning() != 0 )
    {
        m_processing = true;
        vector<float> groove;
        if( lock() )
        {
            std::vector<DTrack>::iterator track;
            if(m_tasks.size())
            {
                string task = m_tasks.at(0);
                m_tasks.erase(m_tasks.begin());

                // update variation for all tracks
                
                for(track = m_tracks.begin(); track != m_tracks.end(); ++track) 
                {
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
                        track->set_swing(m_swing);
                    }
                    else if(task == "groove")
                    {
                        track->set_groove(m_groove);
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
                    for(int i = 0; i < ct; ++i)
                    {
                        groove.push_back(m_preset->getValue("i", 0., i));
                    }
                }
            }
            
            // generate
            if(m_generations.size())
            {
                ConfTrack conf = m_generations.at(0);
                m_generations.erase(m_generations.begin());
                //cout << "st " << ofToString(conf.track_onsets) << " " << ofToString(conf.track_size) << endl;
                //track->generate(conf);
                m_tracks.at(conf.track_id).generate(conf);
               // cout << "end " << ofToString(conf.track_onsets) << " " << ofToString(conf.track_size) << endl;
            }
            
            //unlock();
        }
        if(!groove.size())
        {
            m_seq->update_drum_tracks(&m_tracks);
        }
        else
        {
            m_seq->set_groove(groove);
        }
        ofSendMessage("preset_loaded");
        m_processing = false;
        unlock();
        stopThread();
    } 
}
    

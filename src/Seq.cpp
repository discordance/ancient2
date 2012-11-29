//
//  Seq.cpp
//  ancientSequencer
//
//  Created by Nunja on 7/14/12.
//  Copyright 2012 Nunja. All rights reserved.
//


#include "Seq.h"

//--------------------------------------------------------------
Seq::Seq(){
    
    // value init
    m_started = false;
    m_midi_delay = 0;
    m_ticks = 0;
    m_bpm = 120;
    
    // not synced by default
    m_synced_seq = false;
    m_resolution = SEQ_HI_RES;
    m_reso_multiplier = (double)1.0/m_resolution;
    m_max_ticks = (m_resolution / 4) * SEQ_LOOP_SIZE;
    m_max_steps = SEQ_LOOP_SIZE;
    
    // mach time
    mach_timebase_info_data_t tinfo;
    mach_timebase_info(&tinfo);
    m_mach_multiplier = (double)tinfo.numer / tinfo.denom;
    m_mach_multiplier = 1.0 / m_mach_multiplier; // to nano seconds factor
    
    // init mutes to 0
    for(int i = 0; i < 8; ++i){ m_mutes[i] = false; }
    
    // reset events 
    reset_events();
    
    // midi init
    m_midiIn.openVirtualPort("Ancient2 SYNC IN");
    m_midiIn.ignoreTypes(false, false, false);
    
    // virtual port
    m_virtual_midiOut.openVirtualPort("Ancient2 NOTES OUT");
    
    // sync out;
    m_sync_out.openVirtualPort("Ancient2 SYNC OUT");
	
	// add testApp as a listener
	m_midiIn.addListener(this);
    
    // pitchs
    // ableton pitch map
    static const int abl[] = {
        36,// kick
        38,// snare1
        40,// snare2
        42,// chh
        46,// ohh
        47,// perc3
        48,// perc2
        49 // os
    };
    
    vector<int> ab_pitchmap (abl, abl + sizeof(abl) / sizeof(abl[0]) );
    m_pitches["ableton"] = ab_pitchmap;
    
    ofLog(OF_LOG_NOTICE, "initialized Ancient2 sequencer with a resolution of: "+ofToString(m_max_ticks));
}

void Seq::set_synced(bool status)
{
    m_synced_seq = status;
    if(m_synced_seq)
    {
        m_resolution = SEQ_MIDI_RES;
        m_reso_multiplier = (double)1.0/m_resolution;
        m_max_ticks = (m_resolution / 4) * SEQ_LOOP_SIZE;
        m_max_steps = SEQ_LOOP_SIZE;
    }
    else
    {
        m_resolution = SEQ_HI_RES;
        m_reso_multiplier = (double)1.0/m_resolution;
        m_max_ticks = (m_resolution / 4) * SEQ_LOOP_SIZE;
        m_max_steps = SEQ_LOOP_SIZE;
    }
    
    reset_events();
}

void Seq::set_bpm(int bpm)
{
    m_bpm = (float)bpm;
}

void Seq::set_playing(bool status)
{
    m_started = status;
    m_ticks = 0;
    if(!m_started)
    {
        stopThread();
        kill_events(10);
    }
    else
    {
        startThread();
    }
}

void Seq::toggle_mute(int track, bool status)
{
    m_mutes[track] = status;
    kill_events(10, m_pitches["ableton"].at(track));
}

float Seq::get_bpm()
{
    return m_bpm;
}

int Seq::get_ticks()
{
    return m_ticks;
}

void Seq::set_ancient(Ancient * anc)
{
    m_ancient = anc;
}

void Seq::set_midi_delay(int dly)
{
    m_midi_delay = dly;
}

void Seq::exit()
{
    kill_events(10); // note off on drum events
    m_midiIn.closePort();
    m_midiIn.removeListener(this);
    m_virtual_midiOut.closePort();
}

void Seq::update_drum_tracks(vector<DTrack> *tracks) // v1, replace all
{
    /***********************************************
     * V2
     *
     ***/
    
    if(lock())
    {
        reset_events();
        int mult = m_max_ticks/m_max_steps;
        std::vector<DTrack>::iterator track;
        for(track = tracks->begin(); track != tracks->end(); ++track)
        {
            // track num
            int tr_num = track->get_conf().track_id;
            
            // get current
            vector<Step>* current = track->get_current();
            int ps = current->size(); // phrase size
            vector<Step>::iterator step;
            map<int, vector<int> > evts; // events map for the track to correct it
            
            for(int i = 0; i < m_max_steps; ++i)
            {
                int modi=i%ps;
                Step cstep = current->at(modi);
                
                if(cstep.vel)
                {
                    vector<int> evt;
                    int t_dur = (cstep.dur*mult)-1;
                    int cstick = i * mult; // current start tick
                    int drift = mult*(1+cstep.drift) - mult; // TEST
                    cstick += drift;
                    int cetick = cstick + t_dur;
                    int vel = ofMap(cstep.vel, 0, 15, 0, 127);
                    evt.push_back(cstick);
                    evt.push_back(cetick);
                    evt.push_back(vel);
                    evts[i] = evt;
                }
                else
                {
                    evts[i] = vector<int>(0);
                }
                
            }
            // correct the overlaping events and update
            correct_and_update(evts, tr_num, m_pitches["ableton"].at(tr_num));
        }
        unlock();
    }
}

//--------------------------------------------------------------

void Seq::correct_and_update(map<int, vector<int> >& evt_map, int track, int pitch)
{
    vector<int> *n_p = NULL;
    vector<int> *n_c = NULL;
    map<int, vector<int> >::iterator curr;
    map<int, vector<int> >::iterator last = evt_map.end();
    --last; // get the last

    for(curr = evt_map.begin(); curr != evt_map.end(); curr++)
    {
        n_c = &curr->second;
        // correct under 0
        if(n_c->size() && n_c->at(0) < 0)
        {
            n_c->at(0) = 0;
        }
        int key = curr->first;
        if(curr != evt_map.begin() && (&evt_map.at(key-1))->size())
        {
            n_p = &evt_map.at(key-1);
        };
        
        if(n_p != NULL)
        {
            if(n_c->size())
            {
                if(n_p->at(1) >= n_c->at(0))
                {
                    n_p->at(1) = n_c->at(0)-1;
                }
                add_event(n_p->at(0), n_p->at(1), track, pitch, n_p->at(2));
            }
            if(curr == last)
            {
                add_event(n_p->at(0), n_p->at(1), track, pitch, n_p->at(2));
            }
        }
        else
        {
            if(n_c->size() && curr == last)
            {
                if(n_c->at(1) >= m_max_ticks)
                {
                    n_c->at(1) = m_max_ticks - 1;
                }
                add_event(n_c->at(0), n_c->at(1), track, pitch, n_c->at(2));
            }
        }
    }
}

void Seq::reset_events()
{
        if(m_events.size())
        {
            m_events.clear();
        }
        
        // max ticks at 24 ppqn for 64 squav
        // init the vector event pointers
        for(int i = 0; i < m_max_ticks; ++i)
        {
            m_events.push_back(vector<Evt>(0)); 
        }
}


// thread
void Seq::threadedFunction()
{
    uint64_t diff = 0;
    sendMidiClock(1);
    while( isThreadRunning() != 0 )
    {
        int quav = (m_ticks+m_midi_delay) / (m_resolution/4) ;
        quav = quav % (m_max_ticks/(m_resolution/4));
        m_ancient->notify( quav );
        
        if( lock() )
        {
            int at = (m_ticks+m_midi_delay)%m_max_ticks;
            if(at < 0)
            {
                at = 0;
            }
            vector<Evt> *line = &m_events.at(at);
            // send events
            
            ++m_ticks;
            
            unlock();
            
            // send events
            send_events(line);
            
            // time and wait
            double sec_to_wait = 60/(double)m_bpm;
            sec_to_wait *= m_reso_multiplier;
            uint64_t nanos = sec_to_wait*1000000000;
            uint64_t sleepTimeInTicks = (uint64_t)(nanos * m_mach_multiplier) - diff;
            uint64_t time = mach_absolute_time();
            
            if(m_ticks % (m_resolution/24) == 0 )
            {
                sendMidiClock(0); // yiiiia
            }
            
            mach_wait_until(time + sleepTimeInTicks);
            diff = mach_absolute_time() - (time + sleepTimeInTicks);
        }
    }
    sendMidiClock(2);
}


void Seq::newMidiMessage(ofxMidiMessage& msg)
{
    if(!m_synced_seq){ return; } // not synced
    
    int quav = (m_ticks+m_midi_delay) / (m_resolution/4) ;
    quav = quav % (m_max_ticks/(m_resolution/4));
    m_ancient->notify( quav );
    
    if( lock() )
    {
        // make a copy of the latest message
        m_midiMessage = msg;
        int at = (m_ticks+m_midi_delay)%m_max_ticks;
        if(at < 0)
        {
            at = 0;
        }
        vector<Evt> *line = &m_events.at(at); // here because of the 0 event line must be used
        
        
        // start
        if (msg.status == 250)
        {
            m_started = true;
            m_ticks = 0;
        }
        // stop
        if (msg.status == 252)
        {
            m_started = false;
            m_ticks = 0;
            // send all channels / noteoffs
            kill_events(10);
        }
        // time
        if (msg.status == 248 && m_started)
        {
            
            // send events
            send_events(line);
            
            // update ticks
            m_ticks++;
            
            // compute the bpm
            m_bpm = 60000.0/(ofGetElapsedTimeMillis()-m_clock_past_time)/24;
            m_clock_past_time = ofGetElapsedTimeMillis();
        }
        unlock();
    }  
}

void Seq::kill_events(int chan)
{
    for (int i = 0; i < 128; i++)
    {
        m_virtual_midiOut.sendNoteOff(chan,i,0);
    }
}

void Seq::kill_events(int chan, int pitch)
{
    m_virtual_midiOut.sendNoteOff(chan, pitch ,0);
}

void Seq::add_event(int start, int end, int track, int pitch, int vel)
{
    Evt on;
    on.track = track;
    on.status = 1; // note on on channel 10
    on.channel = 10;
    on.pitch = pitch;
    on.vel = vel;
    
    Evt off;
    off.track = track;
    off.status = 0; // note off on channel 10
    off.channel = 10;
    off.pitch = pitch;
    off.vel = vel;
    
    vector<Evt>* event_line_on = &m_events.at(start);
    event_line_on->push_back(on);
    vector<Evt>* event_line_off = &m_events.at(end);
    event_line_off->push_back(off);
}

void Seq::sendMidiClock(int status)
{

    vector<unsigned char> bytes;
    if(status == 2)
    {
        bytes.push_back(MIDI_STOP);
    }
    if(status == 1)
    {
        bytes.push_back(MIDI_START);
    }
    if(!status)
    {
        bytes.push_back(MIDI_TIME_CLOCK);
    }
    m_sync_out.sendMidiBytes(bytes);
}

void Seq::send_events(vector<Evt>* evts)
{
    vector<Evt>::iterator ev;
    for(ev = evts->begin(); ev != evts->end(); ++ev )
    {
        if(!m_mutes[ev->track])
        {
            if(ev->status)
            {
                m_virtual_midiOut.sendNoteOn(ev->channel, ev->pitch, ev->vel);
            }
            else
            {
                m_virtual_midiOut.sendNoteOff(ev->channel, ev->pitch, ev->vel);
            }
        }
    }
}



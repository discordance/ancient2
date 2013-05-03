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
    m_resolution = SEQ_HI_RES; //SEQ_HI_RES
    m_reso_multiplier = (double)1.0/m_resolution;
    m_max_ticks = (m_resolution / 4) * SEQ_LOOP_SIZE;
    m_max_steps = SEQ_LOOP_SIZE;
    
    // record patterns on a4
    m_a4_send = false;
    
    // mach time
    mach_timebase_info_data_t tinfo;
    mach_timebase_info(&tinfo);
    m_mach_multiplier = ((double)tinfo.numer / (double)tinfo.denom);
    m_mach_multiplier = 1.0 / m_mach_multiplier; // to nano seconds factor
    
    m_gonsets = vector<bool>(m_max_ticks/8, false);

    // init mutes to 0
    for(int i = 0; i < 8; ++i){ m_mutes[i] = false; }
    
    // reset events 
    reset_events();
    
    // detect network
    if(!m_network_out.openPort("Network Session 1"))
    {
        m_network_out.closePort();
    }
    
    // detect MOTU
    if(!m_hard_midiOut.openPort("UltraLite mk3 MIDI Port") &&
       !m_hard_midiOut.openPort("Fireface 400 (7CA) Port 2"))
    {
        m_hard_midiOut.closePort();
    }
    // detect analog four
    if(!m_hard_a4USBIn.openPort("Elektron Analog Four"))
    {
        m_hard_a4USBIn.closePort();
    }
    
    // midi init
    m_virtual_midiIn.openVirtualPort("Ancient2 SYNC IN");
    m_virtual_midiIn.ignoreTypes(false, false, false);
    
    // virtual port
    m_virtual_midiOut.openVirtualPort("Ancient2 NOTES OUT");
    
    // sync out;
    m_sync_out.openVirtualPort("Ancient2 SYNC OUT");
	
	// add testApp as a listener
	m_virtual_midiIn.addListener(this);
    
    // pitchs
    // stdr pitch map
    static const int stdr[] = {
        36,// kick
        41,// drm1
        43,// drm2
        38,// sn
        39,// cl
        42,// HH
        44,// HH2
        46 // HH3
    };
    
    vector<int> stdr_pitchmap (stdr, stdr + sizeof(stdr) / sizeof(stdr[0]) );
    m_pitches["stdr"] = stdr_pitchmap;
    
    ofLog(OF_LOG_NOTICE, "initialized Ancient2 sequencer with a resolution of: "+ofToString(m_max_ticks));
    
    m_poco_thread = &getPocoThread();
    //m_poco_thread->setOSPriority(Poco::ThreadImpl::PRIO_HIGHEST_IMPL);
    //m_poco_thread->setOSPriority(m_poco_thread->getMaxOSPriority());
    //m_poco_thread->get
    //pthread_t threadID = (pthread_t) m_poco_thread->tid();
    
    //cout << "priority " << m_poco_thread->getOSPriority() << endl;
    //cout << "tid " << getPocoThread().tid() << endl;
    
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

int Seq::get_max_ticks()
{
    return m_max_ticks;
}
void Seq::toggle_mute(int track, bool status)
{
    m_mutes[track] = status;
    kill_events(10, m_pitches["stdr"].at(track));
}

float Seq::get_bpm()
{
    return m_bpm;
}

void Seq::update(vector< vector<Evt> > & events)
{
    if(lock())
    {
         m_events = events;
         unlock();
    }
}

map< string, vector<int> > Seq::get_pitches()
{
    return m_pitches;
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

// SWINGGG
vector<float> Seq::classic_swing(float swing)
{
    if(swing >= 1){ swing = 0.99; }
    if(swing <= -1){ swing = -0.99; }
    vector<float> groove;
    float gauss = 0;
    for(int i = 0; i < SEQ_LOOP_SIZE ; ++i)
    {
        if(swing != 0){ gauss = ofxGaussian() * 0.06 +0.02;};
        
        if(i % 2 != 0)
        {
            groove.push_back(swing);
        }
        else
        {
            groove.push_back(0);
        }
    }
    return groove;
}

vector<float> Seq::cycle_swing(float swing)
{
    if(swing >= 1){ swing = 0.99; }
    if(swing <= -1){ swing = -0.99; }
    int modl = 2;
    if(swing < 0)
    {
        modl = 1;
    }
    
    vector<float> groove;
    for(int i = 0; i < SEQ_LOOP_SIZE ; ++i)
    {
        if(i % modl != 0)
        {
            float grv = (i%8)/8.;
            grv *= 0.9;
            grv = exp(grv) - 1;
            grv = ofClamp(grv, 0, 0.9);
            groove.push_back(grv*swing);
        }
        else
        {
            groove.push_back(0);
        }
    }
    return groove;
}


void Seq::exit()
{
    kill_events(10); // note off on drum events
    m_virtual_midiIn.closePort();
    m_virtual_midiIn.removeListener(this);
    m_virtual_midiOut.closePort();
}

vector< vector<Evt> > Seq::generate_events(vector<DTrack> *tracks, vector<float> & groove, map< string, vector<int> > & pitches, int max_ticks)
{
    /***********************************************
     * V3 STATIC !!!
     *
     ***/
    
    vector< vector<Evt> > res_evts;
    for(int i = 0; i < max_ticks; ++i)
    {
        res_evts.push_back(vector<Evt>(0));
    }
    
    int mult = max_ticks/SEQ_LOOP_SIZE;
    std::vector<DTrack>::iterator track;
    for(track = tracks->begin(); track != tracks->end(); ++track)
    {
        // track num
        int tr_num = track->get_conf().track_id;
        
        // get current
        vector<Step>* current = track->get_current();
        int ps = current->size(); // phrase size
        if(!ps)
        {
            continue;
        }
        //vector<Step>::iterator step;
        map<int, vector<int> > evts; // events map for the track to correct it
        for(int i = 0; i < SEQ_LOOP_SIZE; ++i)
        {
            int modi=i%ps; // bug
            Step cstep = current->at(modi);
            
            if(cstep.vel)
            {
                vector<int> evt;
                int t_dur = (cstep.dur*mult)-1;
                int cstick = i * mult; // current start tick
                float drift_val = groove.at(i);
                int drift = mult*(1+drift_val) - mult; // TEST
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
        
        Seq::correct_and_update(res_evts, evts, tr_num, pitches["stdr"].at(tr_num), max_ticks);
    }
       
    return res_evts;
}

//--------------------------------------------------------------

    
void Seq::correct_and_update(vector< vector<Evt> > & evts, map<int, vector<int> > & evt_map, int track, int pitch, int max_ticks)
{
    vector<int> *n_p = NULL;
    vector<int> *n_c = NULL;
    map<int, vector<int> >::iterator curr;
    map<int, vector<int> >::iterator last = evt_map.end();
    int last_add = -1;
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
            }
            
            if(n_p->at(0) != last_add)
            {
                add_event(evts, n_p->at(0), n_p->at(1), track, pitch, n_p->at(2));
                last_add = n_p->at(0);
            }
            
            if(curr == last)
            {
                if(n_c->size())
                {
                    if(n_c->at(1) >= max_ticks)
                    {
                        n_c->at(1) = max_ticks-1;
                    }
                    add_event(evts, n_c->at(0), n_c->at(1), track, pitch, n_c->at(2));
                }
            }
        }
        else
        {
            if(n_c->size() && curr == last)
            {
                            
                if(n_c->at(1) >= max_ticks)
                {
                    n_c->at(1) = max_ticks - 1;
                }
                add_event(evts, n_c->at(0), n_c->at(1), track, pitch, n_c->at(2));
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

int Seq::get_quav()
{
    int quav = (m_ticks+m_midi_delay) / (m_resolution/4) ;
    quav = quav % (m_max_ticks/(m_resolution/4));
    return quav;
}

// thread 
void Seq::threadedFunction()
{
    // set realtime
    uint64_t absolute = (uint64_t)(500000 * m_mach_multiplier); //2902490
    uint64_t diff = 0;
    uint64_t nanos = 0;
    uint64_t sleepTimeInTicks = 0;// - diff;
    uint64_t time = 0;
    uint64_t until = 0;
    vector<Evt> *line;
    double sec_to_wait = 0;
    int at = 0;
    long ss = 0;
    uint64_t start = 0;
    OSSpinLock spin_lock = OS_SPINLOCK_INIT;
    set_realtime(absolute, absolute*0.5, absolute * 0.85);
    
    sendMidiClock(1);
    while( isThreadRunning() != 0 )
    {
        if(OSSpinLockTry(&spin_lock))
        {
            start = mach_absolute_time();
            
            at = (m_ticks+m_midi_delay)%m_max_ticks;
            if(at < 0)
            {
                at = 0;
            }
            
            line = &m_events.at(at);
            
            ++m_ticks;
            
            // send events
            
            if(m_ticks % (m_resolution/SEQ_MIDI_RES) == 0 )
            {
                sendMidiClock(0); // yiiiia
                //cout << (60000000.0/(ofGetElapsedTimeMicros() - ss)/24) << endl;
                ss = ofGetElapsedTimeMicros();
            }
            /*
            if(m_ticks % (m_resolution/(SEQ_MIDI_RES/4)) == 0 )
            {
                sendSpp(m_ticks/(m_resolution/(SEQ_MIDI_RES/4)));
            }
             */
           // cout <<  << endl;
            send_events(line);
            
            // time and wait
            sec_to_wait = 60/(double)m_bpm;
            sec_to_wait *= m_reso_multiplier;
            nanos = sec_to_wait*1000000000;
            time = mach_absolute_time();
            sleepTimeInTicks = (uint64_t)(nanos * m_mach_multiplier) - diff - (time-start);
            
            until = (time + (sleepTimeInTicks));
            mach_wait_until(until);
            diff = mach_absolute_time() - until;

            OSSpinLockUnlock(&spin_lock);
        }
    }
    sendMidiClock(2);
}


void Seq::newMidiMessage(ofxMidiMessage& msg)
{
    if(!m_synced_seq){ return; } // not synced
    
    //int quav = (m_ticks+m_midi_delay) / (m_resolution/4) ;
    //quav = quav % (m_max_ticks/(m_resolution/4));
    //m_ancient->notify( quav );
    
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
            sendMidiClock(1);
        }
        // stop
        if (msg.status == 252)
        {
            m_started = false;
            m_ticks = 0;
            // send all channels / noteoffs
            kill_events(10);
            sendMidiClock(2);
        }
        //pause (reaper)
        if (msg.status == 251)
        {
            m_started = true;
        }
        // time
        if (msg.status == 248 && m_started)
        {
            // update ticks
            m_ticks++;
            
            sendMidiClock(0);
            // send events
            send_events(line);
            // compute the bpm
            //m_bpm = 60000.0/(ofGetElapsedTimeMillis()-m_clock_past_time)/24;
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

void Seq::add_event(vector< vector<Evt> > & evts, int start, int end, int track, int pitch, int vel)
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
    
    vector<Evt>* event_line_on = &evts.at(start%evts.size());
    event_line_on->push_back(on);
    vector<Evt>* event_line_off = &evts.at(end%evts.size());
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
    if(m_hard_midiOut.isOpen())
    {
        m_hard_midiOut.sendMidiBytes(bytes);
    }
    if(m_hard_a4USBIn.isOpen())
    {
        m_hard_a4USBIn.sendMidiBytes(bytes);
    }
    if(m_network_out.isOpen())
    {
        m_network_out.sendMidiBytes(bytes);
    }
}

void Seq::sendSpp(int position)
{
    vector<unsigned char> bytes;
    bytes.push_back(MIDI_SONG_POS_POINTER);
    bytes.push_back(position);
    
    m_sync_out.sendMidiBytes(bytes);
    if(m_hard_midiOut.isOpen())
    {
        m_hard_midiOut.sendMidiBytes(bytes);
    }
    if(m_hard_a4USBIn.isOpen())
    {
        //m_hard_a4USBIn.sendMidiBytes(bytes);
    }
    if(m_network_out.isOpen())
    {
        m_network_out.sendMidiBytes(bytes);
    }
}

void Seq::send_events(vector<Evt>* evts)
{
    //long st = ofGetElapsedTimeMicros();
    vector<Evt>::iterator ev;
    if(!evts->size())
    {
        return;
    }
    for(ev = evts->begin(); ev != evts->end(); ++ev )
    {
        if(!m_mutes[ev->track])
        {
            if(ev->status)
            {
                m_virtual_midiOut.sendNoteOn(ev->channel, ev->pitch, ev->vel);
                if(m_hard_midiOut.isOpen())
                {
                    m_hard_midiOut.sendNoteOn(ev->channel, ev->pitch, ev->vel);
                }
                if(m_network_out.isOpen())
                {
                    m_network_out.sendNoteOn(ev->channel, ev->pitch, ev->vel);
                }
                sendA4NoteOn(ev->pitch,  ev->vel);
            }
            else
            {
                m_virtual_midiOut.sendNoteOff(ev->channel, ev->pitch, ev->vel);
                if(m_hard_midiOut.isOpen())
                {
                    m_hard_midiOut.sendNoteOff(ev->channel, ev->pitch, ev->vel);
                }
                if(m_network_out.isOpen())
                {
                    m_network_out.sendNoteOff(ev->channel, ev->pitch, ev->vel);
                }
                sendA4NoteOff(ev->pitch,  ev->vel);
            }
        }
    }
}

void Seq::sendA4NoteOn(int pitch, int vel)
{
    if(m_a4_send && m_hard_a4USBIn.isOpen())
    {
        switch (pitch) {
            case 36:
                m_hard_a4USBIn.sendNoteOn(1, 64, vel);
                break;
            case 41:
                m_hard_a4USBIn.sendNoteOn(2, 64, vel);
                break;
            case 38:
                m_hard_a4USBIn.sendNoteOn(3, 64, vel);
                break;
            case 42:
                m_hard_a4USBIn.sendNoteOn(4, 64, vel);
                break;
                
            default:
                break;
        }
    }
}

void Seq::sendA4NoteOff(int pitch, int vel)
{
    if(m_a4_send && m_hard_a4USBIn.isOpen())
    {
        switch (pitch) {
            case 36:
                m_hard_a4USBIn.sendNoteOff(1, 64, vel);
                break;
            case 41:
                m_hard_a4USBIn.sendNoteOff(2, 64, vel);
                break;
            case 38:
                m_hard_a4USBIn.sendNoteOff(3, 64, vel);
                break;
            case 42:
                m_hard_a4USBIn.sendNoteOff(4, 64, vel);
                break;
                
            default:
                break;
        }
    }
}

// hardcore
int Seq::set_realtime(int period, int computation, int constraint)
{
    struct thread_time_constraint_policy ttcpolicy;
    int ret;
    
    ttcpolicy.period=period; // HZ/160
    ttcpolicy.computation=computation; // HZ/3300;
    ttcpolicy.constraint=constraint; // HZ/2200;
    ttcpolicy.preemptible=1;
    
    if ((ret=thread_policy_set(mach_thread_self(),
                               THREAD_TIME_CONSTRAINT_POLICY, (thread_policy_t)&ttcpolicy,
                               THREAD_TIME_CONSTRAINT_POLICY_COUNT)) != KERN_SUCCESS) {
        fprintf(stderr, "set_realtime() failed.\n");
        return 0;
    }
    return 1;
}



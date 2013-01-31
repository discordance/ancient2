//
//  Seq.h
//  ancientSequencer
//
//  Created by Nunja on 7/14/12.
//  Copyright 2012 Nunja. All rights reserved.
//
#ifndef SEQ
#define SEQ

#include <vector>
#include <map>
#include <mach/mach_time.h>
#include "PennerEasing/Sine.h"
#include "ofxMidi.h"
#include "utils/ofLog.h"
#include "utils/ofUtils.h"
class Ancient;
#include "Ancient.h"
#include "DTrack.h"
#include "Evt.h"

using namespace std;

class Seq : public ofxMidiListener, public ofThread
{
    public:
    
        static const int SEQ_LOOP_SIZE = 64;
        static const int SEQ_MIDI_RES = 24;
        static const int SEQ_HI_RES = 960;
        
        Seq();
        float get_bpm();
        int   get_ticks();
        int   get_max_ticks();
        map< string, vector<int> > get_pitches();
        void  set_midi_delay(int dly);
        void  exit();
        
        void set_ancient(Ancient * anc);
        void toggle_mute(int track, bool status);
        void set_synced(bool status);
        void set_bpm(int bpm);
    
        void set_playing(bool status);
        void update(vector< vector<Evt> > & events);
        //void set_groove_point();
        //void reset_groove();
        //vector<float> get_groove();
        //void set_groove(vector<float> groove);
        int get_quav();
    
        // SWINGGG
        static vector<float> classic_swing(float swing);
        static vector<float> cycle_swing(float swing);
    
        // static
        static vector< vector<Evt> > generate_events(vector<DTrack> *tracks, vector<float> & groove, map< string, vector<int> > & pitches, int max_ticks);
        static void add_event(vector< vector<Evt> > & evts, int start, int end, int track, int pitch, int vel);
        static void correct_and_update(vector< vector<Evt> > & evts, map<int, vector<int> >& evt_map, int track, int pitch, int max_ticks);
    
    protected:
    
        void newMidiMessage(ofxMidiMessage& eventArgs);
        void reset_events();
        
        void send_events(vector<Evt> *evts);
        void kill_events(int chan);
        void kill_events(int chan, int pitch);
        void do_nudge();
        
        // midi
        ofxMidiIn   m_midiIn;
        ofxMidiOut  m_virtual_midiOut; // for internal routing
        ofxMidiOut  m_sync_out;
        ofxMidiOut  m_hard_midiOut; // for hardware routing
        ofxMidiOut  m_network_out;
        ofxMidiMessage m_midiMessage;
        
        // nudge
        float m_nudge;
    
        // mutes
        map<int, bool> m_mutes;
    
        // pitches
        map< string, vector<int> > m_pitches;
        
        // ancient to notify timing
        Ancient *m_ancient;
        
        // seq mode/ res
        bool  m_synced_seq; // tells if the sequencer is sync to an external clock or not (use an internal clock thread)
        int   m_resolution; // 96 for self clock, synced must be 24
        double m_reso_multiplier;
        int   m_max_ticks; // size of the event grid, depends on the resolution
        int   m_max_steps; // max steps of the step sequencer;
        // seq status
        int   m_midi_delay; // delay of the clock
        bool  m_started;
        int   m_ticks;
        float m_bpm;
        
        // helpers
        int m_clock_past_time;
        
        // seq data
        vector< vector<Evt> > m_events; // max ticks
    
        // swing is now part of the Seq
        //vector<float> m_groove;
        
        // mach time
        double m_mach_multiplier;
    
        // thread
        void threadedFunction();
        void sendMidiClock(int status);
    
        //testing
        vector<bool> m_gonsets;
};


#endif
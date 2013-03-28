//
//  Ancient.h
//  ancientSequencer
//
//  Created by Nunja on 8/12/12.
//

#ifndef ANCIENT
#define ANCIENT

#include <vector>
#include "ofTypes.h"
#include "Gaia.h"
#include "Euclid.h"
#include "DTrack.h"
class Seq; 
#include "Seq.h"
#include "Evt.h"

class Ancient : public ofThread
{
    
    
    public:
        Ancient();
        void init();
        void generate(ConfTrack conf);
        void set_seq(Seq *seq);
        void set_evolution(float level, float variat);
        void set_swing(float swg);
        void set_groove(vector<float> groove);
        void set_xor_variation(float ratio);
        void set_jaccard_variation(float thres);
        void set_hold_variation(bool hold);
        void load_preset(ofxXmlSettings * settings);
    
        void notify(int quav); // be informed that a bar is going on
        vector<DTrack> * get_tracks();
        ConfTrack get_track_conf(int idx);
        vector<int> get_track_velocities(int idx);
        vector<float> get_groove();
        int m_bar, m_beat, m_quav;
    
    protected:
        bool m_hold_variation;
        //bool m_processing;
        vector<DTrack> m_tracks;
        Seq *m_seq;
        float m_swing;
        vector<float> m_groove;
        float m_xor_variation;
        float m_jacc_variation;
        float m_level;
        float m_variat;
        ofxXmlSettings * m_preset;
        
        // states
        bool m_on_variation;
        
        // queue handling
        vector<string> m_tasks;
        vector<ConfTrack> m_generations;
    
        // GA
        //map<int, vector<float> > m_ga_tasks;
        
        // thread
        void threadedFunction();
        // refactoring the seq stuff
        /**
        vector< vector<Evt> > generate_events();
        void add_event(vector< vector<Evt> > & events, int start, int end, int track, int pitch, int vel);
        void correct_and_update(vector< vector<Evt> > & events, map<int, vector<int> > & evt_map, int track, int pitch);
         */
};
#endif
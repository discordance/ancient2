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

class Ancient : public ofThread
{
    
    
    public:
        Ancient();
        void update();
        void generate(ConfTrack conf);
        void set_seq(Seq *seq);
        void set_level_variat(float level, float variat);
        void set_swing(float swg);
        void set_groove(vector<float> groove);
        void set_xor_variation(float ratio);
        void set_jaccard_variation(float thres);
        bool is_processing();
        void notify(int quav); // be informed that a bar is going on
        vector<DTrack> * get_tracks();
        ConfTrack get_track_conf(int idx);
        vector<int> get_track_velocities(int idx);
        bool m_hold_variation;
        int m_bar, m_beat, m_quav;
    
    protected:
        bool m_processing;
        vector<DTrack> m_tracks;
        Seq *m_seq;
        float m_swing;
        vector<float> m_groove;
        float m_xor_variation;
        float m_jacc_variation;
        float m_level;
        float m_variat;
        
        // queue handling
        vector<string> m_tasks;
        vector<ConfTrack> m_generations;
    
        // GA
        //map<int, vector<float> > m_ga_tasks;
        
        // thread
        void threadedFunction();
    
};
#endif
//
//  DTrack.h
//  ancient2
//
//  Created by nunja on 11/20/12.
//
//

#ifndef DTRACK
#define DTRACK

#include <vector>
#include "PennerEasing/Sine.h"
#include "Step.h"
#include "ConfTrack.h"
#include "Euclid.h"

using namespace std;

// DTrack stands for Dynamic Track
class DTrack {
    
    public:
        DTrack(int track_id, int track_size = 16);
       // ~DTrack();
        
        void           generate(ConfTrack conf);
        void           set_conf(ConfTrack conf);
        ConfTrack      get_conf();
        bool           has_events();
        vector<Step> * get_current();
        vector<int>    get_velocities();
        
        void set_swing(float swing);
        void set_groove(vector<float> groove);
        void set_xor_variation(float thres, bool mode = false);
        void set_jaccard_variation(float thres, bool mode = false);
        void evolve(float level, float permute);
        
    protected:
        
        // PROPERTIES STORED ON DISK
        int m_track_id; // conf: disk
        vector<float> m_track_groove; // conf: disk
        // current form of the track after transforms
        vector<Step> m_track_current; // conf: disk
        // stores the intervals between onsets
        vector<bool> m_vanilla_beat; // conf: disk
        vector<bool> m_shadow_beat; // conf: disk
        vector<bool> m_alternation_beat; // conf: disk
        vector<int> m_velocities; // conf: disk
 
        
        // PARAMETTERS
        int m_track_size; // conf: disk + iface
        int m_track_onsets; // conf: disk + iface
        float m_track_rotation; // conf: disk + iface
        unsigned int m_velocity_mode; // conf: disk + iface
        int m_velocity_min; //disk + iface
        int m_velocity_max; //disk + iface
        float m_track_evenness;
        
        // euclidean properties
        // shadow beat incorrectness
        float m_euclid_bias; // conf: disk + iface 
        // mutations
        float m_euclid_density; // conf: disk + iface 
        float m_euclid_permutation; // conf: disk + iface 
        float m_euclid_evolution_rate; // conf: disk + iface 
        float m_euclid_permutation_rate; // conf: disk + iface
        
        // adds the current track groove to the phrase
        void update_groove();
        vector<Step> generate_empty_phr(int size);
        vector<Step> generate_phr(vector<int> & vels, vector<float> & groove);
};

#endif

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
#include "ofxXmlSettings.h"
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
    
        vector<int>  get_velocities();

        void add_to_preset(ofxXmlSettings * settings);
        void load_preset(ofxXmlSettings * settings);
    
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
        vector<Step> m_track_prev_current; // conf: disk
        // stores the intervals between onsets
        vector<bool> m_vanilla_beat; // conf: disk
        vector<bool> m_shadow_beat; // conf: disk
        vector<bool> m_alternation_beat; // conf: disk
        vector<bool> m_permutation_places;
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
        void reset();
        vector<Step> generate_empty_phr(int size);
        vector<Step> generate_phr(vector<int> & vels, vector<float> & groove);
    
        // utils
        static vector<unsigned char> steps_to_bytes(vector<Step> *phr);
        static vector<int> bytes_to_ints(vector<unsigned char> bytes);
        static vector<int> steps_to_vel(vector<Step> *phr);
    
        // fx utils
        vector<int> jaccard_variation(vector<Step> *phr, float thres);
        static void upper_compressor(vector<int> *phr, int max_vel);
        // weigthed jaccard
        static float wjacc(vector<int>& s1, vector<int>& s2);
    
        
    

};

#endif

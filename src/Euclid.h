//
//  Euclid.h
//  ancient2
//
//  Created by nunja on 11/13/12.
//
//

#ifndef EUCLID
#define EUCLID

#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <numeric>
#include "PennerEasing/Expo.h"
#include "utils/ofLog.h"
#include "utils/ofUtils.h"

using namespace std;

class Euclid {

    public:
    
        // modes
        static const unsigned int VEL_STATIC = 0;
        static const unsigned int VEL_LINE = 1;
        static const unsigned int VEL_SINE = 2;
        static const unsigned int VEL_RAMP = 3;
        static const unsigned int VEL_RAND = 4;
    
        // euclid heuristics
        // faster and more robust than Gaia.
        static float weighted_density(vector<int> & target);
        static float weighted_evenness(vector<int> & target);
        static float evenness(vector<int> & ivals, int size);
        
        // euclid
        static vector<bool> gen_bjorklund(int size, int onsets);
        // non-euclid, best match to eveness
        static vector<bool> gen_permuted_intervals(int size, int onsets, float evenness = 0.9);
        
        // rotation
        static void rotate_beat(vector<bool> & beat, float rotation);
    
        // generate discrete sequences from waveforms
        static vector<int> discrete_line(int size, int max, int min, bool asc = true);
        static vector<int> discrete_sine(int size, int max, int min);
        static vector<int> discrete_ramp(int size, int max, int min, bool asc = false);
        static vector<int> discrete_random(int size, int max, int min);
        
        // euclid modifiers // shadowbeat
        static vector<bool> shadow(vector<bool> & beat, float bias = 0., float prune = 1.);
        // euclid modifiers // alternation 
        static vector<bool> alternation(vector<bool> & beat, int first, int order, float bias = 0);
        
        // combinatorics utils
        inline static void desc_partitions(int num, int largest, int max, vector< vector<int> > & result,  vector<int> & prefix);
        
        // assemble
        static vector<int> assemble(vector<bool> beat, vector<int> vels);
        static vector<int> cross_assemble(vector<bool> beat_a, vector<bool> beat_b, vector<int> vels, float crossfade);
    
        // utils
        static void prune(vector<bool> & beat, float rate);
        static void permute(vector<bool> & beat, vector<bool> shadow, float rate);
    
        // debug
        static void dump_beat(vector<bool> & beat, string message);
        static void dump_vels(vector<int> & vels);
        
        /*
        static vector<int> generate_velocities(int num, int max = 15);
        static vector<int> rnd_parts(int num, int max);
        */
    
        // normal gaussian rand
        static float normal(float mean, float stdev);
        
    private:
        // utils
        // standard deviation utils
        static float max_stdev(int size, int onsets);
        static float st_dev(vector<int> & target);
        // bjorklund utils
        static void pattern_build(int level, vector<bool> & pattern, vector<float> & counts, vector<int> & remainders);
        // interval utils
        static vector<int> non_even_ivals(int size, int onsets, float evenness = 1.);
        // utils
        static vector<int>  get_ivals(vector<bool> & target); // get intervals from a beat
        static vector<bool> get_beat(vector<int> & ivals, int size); // get beat from intervals and size
        static vector<int>  get_positions(vector<bool> & target);
        
};


#endif

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
#include <vector>
#include <map>
#include <numeric>
#include "Step.h"
#include "utils/ofLog.h"
#include "utils/ofUtils.h"

using namespace std;

class Euclid {

    public:
    
        // modes
        static const unsigned int VEL_LINE = 0;
        static const unsigned int VEL_SINE = 1;
        static const unsigned int VEL_RAMP = 2;
        static const unsigned int VEL_RAND = 3;
    
        // euclid heuristics
        // faster and more robust than Gaia.
        static float weighted_density(vector<int> & target);
        static float weighted_evenness(vector<int> & target);
        static float evenness(vector<int> & ivals, int size);
        
        // euclid
        static vector<bool> gen_bjorklund(int size, int onsets);
        // non-euclid, best match to eveness
        static vector<bool> gen_permuted_intervals(int size, int onsets, float evenness = 0.9);
        
        // generate discrete sequences from waveforms
        static vector<int> discrete_line(int size, int max, int min, bool asc = true);
        static vector<int> discrete_sine(int size, int max, int min);
        static vector<int> discrete_ramp(int size, int max, int min, bool asc = false);
        static vector<int> discrete_random(int size, int max, int min);
        
        // euclid modifiers // shadowbeat
        static vector<bool> shadow(vector<bool> & beat);
        // euclid modifiers // alternation 
        static vector<bool> pruning(vector<bool> & beat);
        
        // combinatorics utils
        inline static void desc_partitions(int num, int largest, int max, vector< vector<int> > & result,  vector<int> & prefix);
        
        /*
        static vector<int> generate_velocities(int num, int max = 15);
        static vector<int> rnd_parts(int num, int max);
        */ 
        
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
        static vector<int> get_ivals(vector<bool> & target);
        static vector<bool> get_beat(vector<int> & ivals, int size);
        
};


#endif

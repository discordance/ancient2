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
        // euclid heuristics
        // faster and more robust than Gaia.
        static float density(vector<int> & target);
        static float evenness(vector<int> & target);
        
        static vector<bool> bjorklund(int size, int onsets);
        
        static void partitions(int num, int largest, int max, vector< vector<int> > & result,  vector<int> & prefix);
        static vector<int> generate_velocities(int num, int max = 15);
        static vector<int> rnd_parts(int num, int max);
        
    private:
        // utils
        // standard deviation utils
        static float max_stdev(int size, int onsets);
        static float st_dev(vector<int> & target);
        // bjorklund utils
        static void pattern_build(int level, vector<bool> & pattern, vector<float> & counts, vector<int> & remainders);
};


#endif

//
//  RandUtils.h
//  ancient2
//
//  Created by nunja on 7/16/13.
//
//

#ifndef __ancient2__RandUtils__
#define __ancient2__RandUtils__

#include <iostream>
#include <vector>
#include "utils/ofUtils.h"
#include "VRand.h"
#include "ConfTrack.h"

class RandUtils
{
public:
    static void test()
    {

        VRand v = VRand();
        v.seed((unsigned int)RandUtils::rnd(100));
        std::vector<float> a;
        std::vector<float> c;
        
        for (int h = 0; h < 100; ++h) {
            a.push_back((v.pink()));
        }
        RandUtils::correlateRnd(0.5, 1, a, c);
        RandUtils::dump(a);
        //RandUtil::dump(b);
        RandUtils::dump(c);
        
    }
    
    static std::vector<float> get_pink_serie(int size, float att)
    {
        std::vector<float> res;
        VRand v = VRand();
        v.seed((unsigned int)RandUtils::rnd(100));
        for (int h = 0; h < size; ++h) {
            res.push_back((v.pink()*att));
        }
        return res;
    }
    
    static void correlateRnd(float rho, float att, std::vector<float>& a, std::vector<float>& res)
    {
        VRand v = VRand();
        v.seed((unsigned int)RandUtils::rnd(100));
        float r2 = powf(rho, 2.);
        float ve = 1-r2;
        float sd = sqrtf(ve);
        
        for (int i = 0; i < a.size(); ++i) {
            res.push_back(rho*a.at(i)+(sd*v.pink()*att));
        }
    }
    
    static ConfTrack random_track(int track_id, int division, float density)
    {
        srand(ofGetElapsedTimeMicros());
        ConfTrack conf = ConfTrack();
        conf.track_id = track_id;
        
        // rnd sizr according to division
        vector<int> sizes;
        for(int i = 3; i < 64; ++i){ if(i % division == 0){ sizes.push_back(i); } }
        int r = std::rand() % sizes.size();
        int dens_max = floor(sizes.at(r)*density);

        conf.track_onsets = std::rand() % dens_max;
        conf.track_rotation =  RandUtils::rnd(1);
        conf.track_size = sizes.at(r);
        conf.track_evenness = ofClamp(RandUtils::normal(1, 1), 0.01, 1);
        conf.track_seed =  RandUtils::rnd(1) * 127;
        conf.velocity_max = 13;
        conf.velocity_min =  RandUtils::rnd(1) * 12;
        conf.euclid_bias =  RandUtils::rnd(1);
        conf.euclid_density = 0.5;
        conf.euclid_permutation = 0.;
        conf.euclid_evolution_rate = 0.2;
        conf.euclid_permutation_rate = 1.;
        conf.groove_cycle = 2;
        conf.groove_ratio = 0.;
        conf.velocity_mode = RandUtils::rnd(1)*4;
        
        return conf;
    }
    
    // gaussian random
    static float normal(float mean, float stdev)
    {
        float rnd = ( RandUtils::rnd(1) * 2 - 1) + (RandUtils::rnd(1) * 2 - 1) + (RandUtils::rnd(1) * 2 - 1);
        return rnd * stdev + mean;
    }
    
    static float rnd(float max)
    {
        return max * rand() / (RAND_MAX + 1.0f);
    }
    
    static void dump(std::vector<float>& trg)
    {
        for (int h = 0; h < trg.size(); ++h) {
            std::cout << trg.at(h) << std::endl;
        }
        std::cout << "************************************************" << std::endl;
    }
};


#endif /* defined(__ancient2__RandUtils__) */

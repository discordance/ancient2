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
#include "VRand.h"

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

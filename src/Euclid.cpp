//
//  Euclid.cpp
//  ancient2
//
//  Created by nunja on 11/13/12.
//
//

#include "Euclid.h"

/**
 * Generates a vector of onsets with bjorklund algorithm.
 */
vector<bool> Euclid::bjorklund(int size, int onsets)
{
    if(onsets > size)
    {
        onsets = size;
    }
    vector<bool> pattern;
    vector<float> counts;
    vector<int> remainders;
    int divisor = size - onsets;
    remainders.push_back(onsets);
    int level = 0;
    while(true)
    {
        counts.push_back(divisor / (float)remainders.at(level));
        remainders.push_back(divisor % remainders.at(level));
        divisor = remainders.at(level);
        ++level;
        if(remainders.at(level) <= 1)
        {
            break;
        }
    }
    counts.push_back(divisor);
    Euclid::pattern_build(level, pattern, counts, remainders);
    vector<bool>::iterator first = find(pattern.begin(), pattern.end(), true);
    int idx = first - pattern.begin();
    rotate(pattern.begin(), pattern.begin() + idx, pattern.end());
    return pattern;
}
 

/**
 * Return the density of a int vector (velocities).
 */
float Euclid::density(vector<int> & target)
{
    vector<float> densities; // reduce densities to groups
    for(int i = 0; i < 4; ++i)
    {
        int onsets = 0;
        vector<int>::iterator vel;
        for(vel = target.begin(); vel != target.end(); ++vel)
        {
            if(*vel && floor(*vel/4.) == i)
            {
                onsets++;
            }
        }
        densities.push_back( (onsets/(float)target.size()) * (1+i) );
    }
    return accumulate(densities.begin(), densities.end(), 0.)/4.;
}

/**
 * Return the evenness of a int vector (velocities).
 */
float Euclid::evenness(vector<int> & target)
{
    vector<float> deviations;
    for(int i = 0; i < 4; ++i)
    {
        vector<int>::iterator vel;
        vector<int> onsets;
        vector<int> ivals;
        int prev_idx = 0;
        for(vel = target.begin(); vel != target.end(); ++vel)
        {
            int index = vel - target.begin();
            if(*vel && floor(*vel/4.) == i)
            {
                onsets.push_back(index);
            }
        }
        if(!onsets.size()){ deviations.push_back(0.); }
        else if(onsets.size() == 1){ deviations.push_back(0.); }
        else
        {
            for(int j = 0; j < onsets.size(); ++j)
            {
                int ival = 0;
                if(j < onsets.size() - 1)
                {
                    ival = onsets.at(j+1)-onsets.at(j) - 1;
                }
                else
                {
                    ival = onsets.at(0) + (target.size() - onsets.at(j)) - 1;
                }
                ivals.push_back(ival);
            }
            float max_dev = Euclid::max_stdev(target.size(), ivals.size()); // max deviation on the serie
            float dev = Euclid::st_dev(ivals);
            deviations.push_back( (dev / max_dev) * (1+i) );
        }
        
    }
    return 1-accumulate(deviations.begin(), deviations.end(), 0.)/4.;
}


/**
 * Utils
 *********************/

/**
 * Gives the max stdev of a serie by its caracteristics
 */
float Euclid::max_stdev(int size, int onsets)
{
    float max_ival = size - onsets;
    float m = max_ival / onsets;
    float accum = ((0 - m) * (0 - m))*(onsets-1);
    accum += (max_ival - m) * (max_ival - m);
    return sqrt(accum / (onsets-1));
}

/**
 * Gives the stdev of a serie
 */
float Euclid::st_dev(vector<int> & target)
{
    float sum = accumulate(target.begin(), target.end(), 0.0);
    float m = sum / target.size();
    float accum = 0.;
    vector<int>::iterator val;
    for(val = target.begin(); val != target.end(); ++val)
    {
        accum += (*val - m) * (*val - m);
    }
    float stdev = sqrt(accum / (target.size()-1));
    return stdev;
}

/**
 * builds an euclidean pattern
 */
void Euclid::pattern_build(int level, vector<bool> & pattern, vector<float> & counts, vector<int> & remainders)
{
    if (level == -1)
    {
        pattern.push_back(false);
    }
    else if (level == -2)
    {
        pattern.push_back(true);
    }
    else
    {
        int to = floor(counts.at(level));
        for (int i = 0; i < to; ++i)
        {
            Euclid::pattern_build(level - 1, pattern, counts, remainders);
        }
        if(remainders.at(level))
        {
            Euclid::pattern_build(level - 2, pattern, counts, remainders);
        }
    }
}

vector<int> Euclid::rnd_parts(int num, int max)
{
    vector<int> res;// init first
    if(!num)
    {
        return res;
    }
    vector<int> subs;
    int rnd_sub = num-max;
    subs.push_back(rnd_sub);
    subs.push_back(num-rnd_sub);
    for(int i = 0; i < subs.size(); ++i)
    {
        if(subs.at(i) <= max)
        {
            res.push_back(subs.at(i));
        }
        else
        {
            vector<int> subres = rnd_parts(subs.at(i), max);
            res.insert(res.end(), subres.begin(), subres.end());
        }
    }
    sort(res.begin(),res.end());
    return res;
}

vector<int> Euclid::generate_velocities(int num, int max)
{
    vector<int> partition(1,0);
    int idx = 0;
    
    for(int i = 0; i < num; ++i)
    {
        if(partition.at(idx) < max)
        {
            partition.at(idx) += 1;
        }
        else
        {
            ++idx;
            --i;
            if(idx > partition.size()-1)
            {
                partition.push_back(0);
            }
        }
    }
    return partition;
}

void Euclid::partitions(int num, int largest, int max, vector< vector<int> > & result,  vector<int> & prefix)
{
    if (num == 0)
    {
        result.push_back(prefix);
        return;
    }
    for (int i = min(num,largest); i >= 1; i--)
    {
        vector<int> fix = prefix;
        fix.push_back(i);
        if(fix.size() <= max)
        {
            partitions(num-i,i,max,result,fix);
        }
    }
}
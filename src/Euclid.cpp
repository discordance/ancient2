//
//  Euclid.cpp
//  ancient2
//
//  Created by nunja on 11/13/12.
//
//

#include "Euclid.h"

namespace Solution3
{
    template<class T>
    struct CompareDeref
    {
        bool operator()( const T& a, const T& b ) const
        { return *a < *b; }
    };
    
    
    template<class T, class U>
    struct Pair2nd
    {
        const U& operator()( const std::pair<T,U>& a ) const
        { return a.second; }
    };
    
    
    template<class IterIn, class IterOut>
    void sort_idxtbl( IterIn first, IterIn last, IterOut out )
    {
        std::multimap<IterIn, int, CompareDeref<IterIn> > v;
        for( int i=0; first != last; ++i, ++first )
            v.insert( std::make_pair( first, i ) );
        std::transform( v.begin(), v.end(), out,
                       Pair2nd<IterIn const,int>() );
    }
}

/**
 * Generates a vector of onsets with bjorklund algorithm.
 */
vector<bool> Euclid::gen_bjorklund(int size, int onsets)
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

vector<bool> Euclid::gen_permuted_intervals(int size, int onsets, float evenness)
{
    vector<bool> res;
    vector<int> ivals;
    
    if(size > 64){ size = 64; }
    if(onsets > size){ onsets = size; }
    if(size > 32) // we must do two parts
    {
        int sa, sb, oa, ob;
        vector<int> ivala, ivalb;
        sa = size/2;
        sb = size-sa;
        oa = onsets/2;
        ob = onsets-oa;
        ivala = non_even_ivals(sa, oa, evenness);
        ivalb = non_even_ivals(sb, ob, evenness);
        ivals.reserve(ivala.size()+ivalb.size());
        ivals.insert(ivals.end(), ivala.begin(), ivala.end());
        ivals.insert(ivals.end(), ivalb.begin(), ivalb.end());
    }
    else
    {
        ivals = non_even_ivals(size, onsets, evenness);
    }
    
    // shuffle
    random_shuffle(ivals.begin(), ivals.end());
    res = Euclid::get_beat(ivals, size);
    return res;
}

/**
 * Return the density of a int vector (velocities).
 */
float Euclid::weighted_density(vector<int> & target)
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

void Euclid::rotate_beat(vector<bool> & beat, float rotation)
{
    int idx = floor(ofMap(rotation, 0, 1, 0, beat.size()));
    rotate(beat.begin(), beat.begin() + idx, beat.end());
}

vector<bool> Euclid::shadow(vector<bool> & beat, float bias, float prune)
{
    vector<bool> res(beat.size(),false);
    
    if(!accumulate(beat.begin(), beat.end(), 0))
    {
        return res;
    }
    vector<int> ivals = get_ivals(beat);
    int pos = 0;
    int shade = 0;
    vector<bool>::iterator first = find(beat.begin(), beat.end(), true);
    pos += first - beat.begin();
    cout << first - beat.begin() << endl;
    
    vector<int>::iterator ival;
    for(ival = ivals.begin(); ival != ivals.end(); ++ival)
    {
        pos += *ival+1;
        shade = pos-ceil((*ival+1)/2.) + (shade*bias);
        shade %= beat.size();
        if(!beat.at(shade))
        {
            res.at(shade) = true;
        }
    }
     
    return res;
}

vector<bool> Euclid::alternation(vector<bool> & beat, int first, int order, float bias)
{    
    vector<bool> res(beat.size(),false);
    vector<int> poss = Euclid::get_positions(beat);
    vector<int>::iterator pos = poss.begin();
    int k, j, c, a;
    a = 0;
    j = first;
    c = order;
    k = poss.size();
    if(ofRandomf()<bias){a = 1;}
    for(j; j < k; j += c+a)
    {
        res.at(poss.at(j)) = true;
    }
    return res;
}

vector<int> Euclid::discrete_line(int size, int max, int min, bool asc)
{
    if(max<min) max = min;
    vector<int> res;
    for(int i = 0; i < size; ++i)
    {
        float rate = 0.;
        if(asc){ rate = (i+1)/(float)size; } else { rate = 1-(i/(float)size); }
        
        res.push_back(ofMap(rate,0,1,min,max));
    }
    return res;
}
vector<int> Euclid::discrete_sine(int size, int max, int min)
{
    if(max<min) max = min;
    vector<int> res;
    for(int i = 0; i < size; ++i)
    {
        float rate = 0.;
        rate = abs(cos(i*size));
        res.push_back(ofMap(rate,0,1,min,max));
    }
    return res;
}
vector<int> Euclid::discrete_ramp(int size, int max, int min, bool asc)
{
    if(max<min) max = min;
    vector<int> res;
    for(int i = 0; i < size; ++i)
    {
        float rate = 0.;
        float maxrate = (size-1)/(float)size;
        maxrate = (4*maxrate) - floor(4*maxrate);
        if(asc){ rate = i/(float)size; } else { rate = 1-((i+1)/(float)size); }
        rate = (4*rate) - floor(4*rate);
        res.push_back(ofMap(rate,0,1,min,max));
    }
    return res;
}
vector<int> Euclid::discrete_random(int size, int max, int min)
{
    if(max<min) max = min;
    vector<int> res;
    for(int i = 0; i < size; ++i)
    {
        res.push_back(ofRandom(min, max));
    }
    return res;
}

void Euclid::permute(vector<bool> & beat, vector<bool> shadow, float rate)
{
    vector<bool>::reverse_iterator rev;
    vector<bool> tmp = beat; //copy
    int onsets = accumulate(tmp.begin(), tmp.end(), 0);
    onsets = floor((float)onsets*rate);
    int last_swap = -1;
    int last_onset = -1;
    
    for(rev = tmp.rbegin(); rev != tmp.rend() && onsets; ++rev )
    {
        int idx = (tmp.rend() - rev) - 1;
        if(shadow.at(idx))
        {
            last_swap = idx;
            if(last_onset > -1)
            {
                beat.at(last_onset) = false;
                beat.at(last_swap) = true;
                last_onset = last_swap = -1;
            }
        }
        if(*rev)
        {
            last_onset = idx;
            
            if(last_swap > -1)
            {
                beat.at(last_onset) = false;
                beat.at(last_swap) = true;
                last_onset = last_swap = -1;
            }
            --onsets;
        }
    }
}

/**
 * Returns the evenness of a interval vector
 */
float Euclid::evenness(vector<int> & ivals, int size)
{
    if(!ivals.size() || ivals.size() == 1)
    {
        return 1.;
    }
    else
    {
        float max_dev = Euclid::max_stdev(size, ivals.size()); // max deviation on the serie
        float dev = Euclid::st_dev(ivals);
        return 1 - (dev / max_dev);
    }
}

/**
 * Return the evenness of a int vector (velocities).
 */
float Euclid::weighted_evenness(vector<int> & target)
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


void Euclid::prune(vector<bool> & beat, float rate)
{
    if(rate <= 0. )
    {
        return;
    }
    // random pruning
    vector<int> indexes;
    vector<bool>::iterator b;
    for(b = beat.begin(); b != beat.end(); ++b)
    {
        if(*b)
        {
            indexes.push_back(b-beat.begin());
        }
    }
    int prune = floor((indexes.size()-1)*rate);
    int idx = 0;
    while(indexes.size() && prune)
    {
        int rnd = (int)ofRandom(0, indexes.size()-1);
        idx = indexes.at(rnd);
        beat.at(idx) = false;
        indexes.erase(indexes.begin()+rnd);
        
        prune--;
    }
}

void Euclid::dump_beat(vector<bool> & beat, string message)
{
    string res = "[";
    for(vector<bool>::iterator it = beat.begin(); it != beat.end(); it++)
    {
        res += ofToString(*it);
        if(it-beat.begin() < beat.size()-1)
        {
            res += ", ";
        }
    }
    res += "]";
    cout << res << " << " << message << endl;
}

void Euclid::dump_vels(vector<int> & vels)
{
    string res = "[";
    for(vector<int>::iterator it = vels.begin(); it != vels.end(); it++)
    {
        res += ofToString(*it);
        if(it-vels.begin() < vels.size()-1)
        {
            res += ", ";
        }
    }
    res += "]";
    cout << "vels debug: " << res << endl;
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
 * Give non even intervals according to an eveness score
 * 1. gives an euclidean beat intervals (maximally even)
 * @TODO Optimization
 */
vector<int> Euclid::non_even_ivals(int size, int onsets, float evenness)
{
    if(size > 32) { size = 32; } // checks
    if(onsets > size) { onsets = size; } // checks
    
    vector<bool> beat;
    int num = size-onsets;
    int maxval = size-onsets;
    int maxsize = onsets;
    vector<int> prefix;
    vector< vector<int> > res;
    
    Euclid::desc_partitions(num, maxval, maxsize, res, prefix);
    
    vector< vector<int> >::iterator ivals;
    vector< vector<int> >::reverse_iterator rivals;
    float dist = 1.;
    vector<int> win;
    map< float, vector<int> > sorted_res;
    
    if(evenness >= 0.5) // one direction
    {
        for(rivals = res.rbegin(); rivals != res.rend(); ++rivals)
        {
            if(abs(Euclid::evenness(*rivals, size) - evenness) < dist)
            {
                dist = abs(Euclid::evenness(*rivals, size) - evenness);
                win = *rivals;
            }
        }
    }
    else // other direction
    {
        for(ivals = res.begin(); ivals != res.end(); ++ivals)
        {
            if(abs(Euclid::evenness(*ivals, size) - evenness) < dist)
            {
                dist = abs(Euclid::evenness(*ivals, size) - evenness);
                win = *ivals;
            }
        }
    }
    return win;
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
/*
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
*/
void Euclid::desc_partitions(int num, int largest, int maxsize, vector< vector<int> > & result,  vector<int> & prefix)
{
    if (num == 0)
    {
        vector<int> final = prefix; // copy

        while (final.size() < maxsize) // resize to match max
        {
            final.push_back(0);
        }

        result.push_back(final);
        return;
    }
    for (int i = min(num,largest); i >= 1; i--)
    {
        vector<int> fix = prefix;
        fix.push_back(i);
        if(fix.size() <= maxsize)
        {
            desc_partitions(num-i,i,maxsize,result,fix);
        }
    }
}

/**
 * Construct a beat given the intervals and the size
 */
vector<bool> Euclid::get_beat(vector<int> & ivals, int size)
{
    vector<bool> res;//(size,false);
    if(!accumulate(ivals.begin(), ivals.end(), 0))
    {
        res = vector<bool>(size,true);
        return res;
    }
    res = vector<bool>(size,false);
    res.at(0) = true;
    vector<int>::iterator ival;
    int pos = 0;
    for(ival = ivals.begin(); ival != ivals.end()-1; ++ival)
    {
        pos += (*ival)+1;
        res.at(pos) = true;
    }
    return res;
}

/**
 * Get intervals on a sequence of beat
 * @TODO too long !!!!
 */
vector<int> Euclid::get_ivals(vector<bool> & target)
{
    vector<bool>::iterator onset;
    vector<int> onsets;
    vector<int> ivals;
    int prev_idx = 0;
    for(onset = target.begin(); onset != target.end(); ++onset)
    {
        int index = onset - target.begin();
        if(*onset)
        {
            onsets.push_back(index);
        }
    }
    if(!onsets.size())
    {
        ivals.push_back(0);
    }
    else if(onsets.size() == 1)
    {
        ivals.push_back(target.size()-1);
    }
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
    }
    return ivals;
}

vector<int> Euclid::get_positions(vector<bool> & target)
{
    vector<int> res;
    vector<bool>::iterator pos;
    for(pos = target.begin(); pos != target.end(); ++pos)
    {
        if(*pos)
        {
            res.push_back(pos-target.begin());
        }
    }
    return res;
}

vector<int> Euclid::cross_assemble(vector<bool> beat_a, vector<bool> beat_b, vector<int> vels, float crossfade)
{

    // yeah do magick
    
    vector<int> a_vels = Euclid::assemble(beat_a, vels);
    vector<int> b_vels = Euclid::assemble(beat_b, vels);
    vector<int> res(a_vels.size(), 0);
    for(vector<int>::iterator it = a_vels.begin(); it != a_vels.end(); ++it)
    {
        int ct = it - a_vels.begin();
        float val = 0.;
        float vel = 0.;
        float norma, normb;
        norma = (float)a_vels.at(ct)/15;
        normb = (float)b_vels.at(ct)/15;
        val = Expo::easeInOut(crossfade, 0, crossfade, 1);

        
        if(normb > norma)
        {
            vel = ofMap(val, 0, 1, a_vels.at(ct), b_vels.at(ct));
        }
        else
        {
            vel = ofMap(1-val, 1, 0, a_vels.at(ct), b_vels.at(ct));
        }
        res.at(ct) = vel;
    }
    Euclid::dump_vels(res);
    return res;
     
}

/**
 * Assemble Beats and velocities
 */
vector<int> Euclid::assemble(vector<bool> beat, vector<int> vels)
{
    vector<int> res;
    vector<bool>::iterator step;
    int vel = 0;
    for (step = beat.begin(); step != beat.end(); ++step)
    {
        if(*step)
        {
            res.push_back(vels.at(vel));
            ++vel;
        }
        else
        {
            res.push_back(0);
        }
    }
    return res;
}

// gaussian random
float Euclid::normal(float mean, float stdev)
{
    float rnd = (ofRandom(1) * 2 - 1) + (ofRandom(1) * 2 - 1) + (ofRandom(1) * 2 - 1);
    return rnd * stdev + mean;
}

//
//  DTrack.cpp
//  ancient2
//
//  Created by nunja on 11/20/12.
//
//

#include "DTrack.h"

DTrack::DTrack(int track_id, int track_size)
{
    m_track_id = track_id;
    m_track_size = track_size;
    
    if(track_size < 4 || track_size > 64)
    {
        m_track_size = 16;
    }
    
    m_track_onsets = 0;
    m_track_rotation = 0.;
    m_track_evenness = 1.;
    m_track_groove = vector<float>(4,0.); // @TODO find an extension algo
    //vector<Step> m_track_current; // current form of the track after transforms
    m_velocity_mode = Euclid::VEL_STATIC;
    m_velocity_min = 0;
    m_velocity_max = 13;

    m_euclid_bias = 0.; // shadow beat incorrectness

    m_euclid_density = 0.5;
    m_euclid_permutation = 0.;
    m_euclid_evolution_rate = 0.2;
    m_euclid_permutation_rate = 1.;
    
    // current default
    m_track_current = generate_empty_phr(m_track_size);
    
    m_vanilla_beat = vector<bool>(m_track_size,false); // conf: disk
    m_shadow_beat = vector<bool>(m_track_size,false);; // conf: disk
    m_alternation_beat = vector<bool>(m_track_size,false); // conf: disk
    m_permutation_places = vector<bool>(m_track_size,false); // conf: disk
    m_velocities = vector<int>(0,0);// conf: disk
    
}

void DTrack::set_conf(ConfTrack conf)
{
    m_track_id = conf.track_id;
    m_track_onsets = conf.track_onsets;
    m_track_rotation = conf.track_rotation;
    m_track_size = conf.track_size;
    m_track_evenness = conf.track_evenness;
    m_velocity_mode = conf.velocity_mode;
    m_velocity_max = conf.velocity_max;
    m_velocity_min = conf.velocity_min;
    m_euclid_bias = conf.euclid_bias;
    m_euclid_density = conf.euclid_density;
    m_euclid_permutation = conf.euclid_permutation;
    m_euclid_evolution_rate = conf.euclid_evolution_rate;
    m_euclid_permutation_rate = conf.euclid_permutation_rate;
}

ConfTrack DTrack::get_conf()
{
    ConfTrack conf = ConfTrack();
    conf.track_id = m_track_id;
    conf.track_onsets = m_track_onsets;
    conf.track_rotation = m_track_rotation;
    conf.track_size = m_track_size;
    conf.track_evenness = m_track_evenness;
    conf.velocity_mode = m_velocity_mode;
    conf.velocity_max = m_velocity_max;
    conf.velocity_min = m_velocity_min;
    conf.euclid_bias = m_euclid_bias;
    conf.euclid_density = m_euclid_density;
    conf.euclid_permutation = m_euclid_permutation;
    conf.euclid_evolution_rate = m_euclid_evolution_rate;
    conf.euclid_permutation_rate = m_euclid_permutation_rate;
    return conf;
}

vector<int>  DTrack::get_velocities()
{
    vector<int> res;
    vector<Step>::iterator step;
    for(step = m_track_current.begin(); step != m_track_current.end(); ++step)
    {
        res.push_back(step->vel);
    }
    return res;
}

vector<Step> * DTrack::get_current()
{
    return &m_track_current;
}

void DTrack::load_preset(ofxXmlSettings * settings)
{
    
    settings->pushTag("track",m_track_id);
    ConfTrack conf;
    conf.track_id = settings->getValue("id",0);
    conf.track_size = settings->getValue("size",0);
    conf.track_onsets = settings->getValue("onsets",0);
    conf.track_rotation = settings->getValue("rotation",0);
    conf.track_evenness = settings->getValue("evenness",0);
    conf.velocity_mode = settings->getValue("velocity_mode",0);
    conf.velocity_max = settings->getValue("velocity_max",0);
    conf.velocity_min = settings->getValue("velocity_min",0);
    conf.euclid_bias = settings->getValue("euclid_bias",0);
    conf.euclid_density = settings->getValue("euclid_density",0);
    conf.euclid_permutation = settings->getValue("euclid_permutation",0);
    conf.euclid_evolution_rate = settings->getValue("euclid_evolution_rate",0);
    conf.euclid_permutation_rate = settings->getValue("euclid_permutation_rate",0);
    set_conf(conf);
    
    settings->pushTag("velocities");
    int tt = settings->getNumTags("i");
    vector<int> vels;
    for(int i = 0; i < tt; ++i)
    {
        vels.push_back(settings->getValue("i", 0, i));
    }
    m_velocities = vels;
    settings->popTag();
    
    //beats
    settings->pushTag("vanillia_beat");
    tt = settings->getNumTags("i");
    vector<bool> vanilla;
    for(int i = 0; i < tt; ++i)
    {
        vanilla.push_back((bool)settings->getValue("i", 0, i));
    }
    m_vanilla_beat = vanilla;
    settings->popTag();
    
    settings->pushTag("shadow_beat");
    tt = settings->getNumTags("i");
    vector<bool> shadow;
    for(int i = 0; i < tt; ++i)
    {
        shadow.push_back((bool)settings->getValue("i", 0, i));
    }
    m_shadow_beat = shadow;
    settings->popTag();
    
    settings->pushTag("alternation_beat");
    tt = settings->getNumTags("i");
    vector<bool> alternation;
    for(int i = 0; i < tt; ++i)
    {
        alternation.push_back((bool)settings->getValue("i", 0, i));
    }
    m_alternation_beat = alternation;
    settings->popTag();
    
    settings->pushTag("permutation_places");
    tt = settings->getNumTags("i");
    vector<bool> permutation_places;
    for(int i = 0; i < tt; ++i)
    {
        permutation_places.push_back((bool)settings->getValue("i", 0, i));
    }
    m_permutation_places = permutation_places;
    settings->popTag();
    
    // current
    settings->pushTag("current");
    tt = settings->getNumTags("step");
    vector<Step> curr;
    for(int i = 0; i < tt; ++i)
    {
        settings->pushTag("step",i);
        Step st;
        st.vel = settings->getValue("vel", 0);
        st.dur = settings->getValue("dur", 0);
        st.chance = settings->getValue("chance", 0);
        curr.push_back(st);
        settings->popTag();
    }
    m_track_current = m_track_prev_current = curr;
    settings->popTag();
    
    
    settings->popTag();
}

void DTrack::add_to_preset(ofxXmlSettings * settings)
{
    ConfTrack conf = get_conf();
    settings->addTag("track");
    settings->pushTag("track",conf.track_id);
    settings->setValue("id", conf.track_id);
    settings->setValue("size",conf.track_size);
    settings->setValue("onsets",conf.track_onsets);
    settings->setValue("rotation",conf.track_rotation);
    settings->setValue("evenness",conf.track_evenness);
    settings->setValue("velocity_mode",(int)conf.velocity_mode);
    settings->setValue("velocity_max",conf.velocity_max);
    settings->setValue("velocity_min",conf.velocity_min);
    settings->setValue("euclid_bias",conf.euclid_bias);
    settings->setValue("euclid_density",conf.euclid_density);
    settings->setValue("euclid_permutation",conf.euclid_permutation);
    settings->setValue("euclid_evolution_rate",conf.euclid_evolution_rate);
    settings->setValue("euclid_permutation_rate",conf.euclid_permutation_rate);
    
    vector<int>::iterator int_erator;
    vector<bool>::iterator bool_erator;
    
    settings->addTag("velocities");
    settings->pushTag("velocities");
    for(int_erator = m_velocities.begin(); int_erator != m_velocities.end(); ++int_erator)
    {
        settings->addValue("i", *int_erator);
    }
    settings->popTag();
    
    settings->addTag("vanillia_beat");
    settings->pushTag("vanillia_beat");
    for(bool_erator = m_vanilla_beat.begin(); bool_erator != m_vanilla_beat.end(); ++bool_erator)
    {
        settings->addValue("i", *bool_erator);
    }
    settings->popTag();
    
    settings->addTag("shadow_beat");
    settings->pushTag("shadow_beat");
    for(bool_erator = m_shadow_beat.begin(); bool_erator != m_shadow_beat.end(); ++bool_erator)
    {
        settings->addValue("i", *bool_erator);
    }
    settings->popTag();
    
    settings->addTag("alternation_beat");
    settings->pushTag("alternation_beat");
    for(bool_erator = m_alternation_beat.begin(); bool_erator != m_alternation_beat.end(); ++bool_erator)
    {
        settings->addValue("i", *bool_erator);
    }
    settings->popTag();
    
    settings->addTag("permutation_places");
    settings->pushTag("permutation_places");
    for(bool_erator = m_permutation_places.begin(); bool_erator != m_permutation_places.end(); ++bool_erator)
    {
        settings->addValue("i", *bool_erator);
    }
    settings->popTag();
    
    vector<Step> * curr = &m_track_current;
    vector<Step>::iterator step;
    settings->addTag("current");
    settings->pushTag("current");
    for(step = curr->begin(); step != curr->end(); ++step)
    {
        settings->addTag("step");
        settings->pushTag("step",step-curr->begin());
        settings->setValue("vel", step->vel);
        settings->setValue("dur", step->dur);
        settings->setValue("chance", step->chance);
        settings->popTag();
    }
    settings->popTag();
    settings->popTag();
}

void DTrack::generate(ConfTrack conf)
{
    set_conf(conf);
    
    if(!conf.track_onsets) // no need if no onsets haha
    {
        return;
    }
    // get the beat motherfucker
    if(conf.track_evenness >= 1. )
    {
        m_vanilla_beat = Euclid::gen_bjorklund(m_track_size, m_track_onsets);
    }
    else
    {
        m_vanilla_beat = Euclid::gen_permuted_intervals(m_track_size, m_track_onsets);
    }
    
    Euclid::rotate_beat(m_vanilla_beat, m_track_rotation);

    // shadow beat and alternation
    m_shadow_beat = Euclid::shadow(m_vanilla_beat, m_euclid_bias);
    m_alternation_beat = Euclid::alternation(m_vanilla_beat, 1, 2, m_euclid_bias);
    m_permutation_places = m_shadow_beat;
    
    // prune alternation et shadow
    Euclid::prune(m_shadow_beat, 1-m_euclid_evolution_rate);
    Euclid::prune(m_alternation_beat, 1-m_euclid_evolution_rate);

    switch (m_velocity_mode) {
        
        case Euclid::VEL_STATIC:
            m_velocities = vector<int>(m_track_onsets,m_velocity_max);
            break;
            
        case Euclid::VEL_LINE:
            m_velocities = Euclid::discrete_line(m_track_onsets, m_velocity_max, m_velocity_min);
            break;
            
        case Euclid::VEL_SINE:
            m_velocities = Euclid::discrete_sine(m_track_onsets, m_velocity_max, m_velocity_min);
            break;
            
        case Euclid::VEL_RAMP:
            m_velocities = Euclid::discrete_ramp(m_track_onsets, m_velocity_max, m_velocity_min);
            break;
            
        case Euclid::VEL_RAND:
            m_velocities = Euclid::discrete_random(m_track_onsets, m_velocity_max, m_velocity_min);
            break;
            
        default:
            break;
    }
    // assemble vels and beat
    // @ TODO permutation rate and evolution rate
    vector<int> vels = Euclid::assemble(m_vanilla_beat, m_velocities);
    m_track_current = generate_phr(vels, m_track_groove);
    evolve(m_euclid_density, m_euclid_permutation);
}

void DTrack::set_swing(float swing)
{
    if(swing >= 1){ swing = 0.99; }
    if(swing <= -1){ swing = -0.99; }
    vector<float> groove;
    for(int i = 0; i < 4 ; ++i)
    {
        if(i % 2 != 0)
        {
            groove.push_back(swing);
        }
        else
        {
            groove.push_back(0);
        }
    }
    m_track_groove = groove;
    update_groove();
}

void DTrack::set_groove(vector<float> groove)
{
    m_track_groove = groove;
    update_groove();
}

void DTrack::set_xor_variation(float thres, bool mode)
{
    if(!has_events())
    {
        return;
    }
    if(thres > 0)
    {
        // we take the quarter of the phrase
        int div = 4;
        
        if((m_track_size/2)%div){
            --div;
        }
        int vlengh = (m_track_size/div)/2;
        // we roll a dice to make a start or ending var
        int voffset = 0;
        if(ofRandom(0, 2) > 1.)
        {
            voffset = (m_track_size/2) - vlengh;
        }
        
        unsigned char rate = (unsigned char)ofMap(thres, 0, 1, 0, 255); // ok valid
        vector<unsigned char> cbytes = steps_to_bytes(&m_track_current);
        vector<unsigned char> rbytes;
        vector<unsigned char>::iterator cbyte;
        // iterate
        for(cbyte = cbytes.begin(); cbyte != cbytes.end(); ++cbyte)
        {
            int ct = cbyte - cbytes.begin();
            unsigned char tbyte = *cbyte;
            float modifier = m_euclid_permutation_rate;
            unsigned char varbyte = static_cast<unsigned char> (modifier*ofRandom(rate));
            
            if(mode) // full mode
            {
                tbyte = tbyte ^ varbyte;
            }
            else // partial mode
            {
                if(ct >= voffset && ct <= voffset+vlengh)
                {
                    tbyte = tbyte ^ varbyte;
                }
            }
            
            rbytes.push_back(tbyte);
        }
        
        vector<int> res_vels = bytes_to_ints(rbytes);
        
        vector<int>::iterator vel;
        for(vel = res_vels.begin(); vel != res_vels.end(); ++vel)
        {
            Step *cstep = &m_track_current.at(vel - res_vels.begin());
            if(cstep->vel < *vel)
            {
                cstep->vel = *vel;
            }
        }
    }
    else
    {
        m_track_current = m_track_prev_current;
    }
}



void DTrack::set_jaccard_variation(float thres, bool mode)
{
    if(!has_events())
    {
        return;
    }
    if(thres > 0.)
    {
        vector<int> vari = DTrack::jaccard_variation(&m_track_current, thres);
        vector<int>::iterator vel;
        // iterate
        for(vel = vari.begin(); vel != vari.end(); ++vel)
        {
            Step *step = &m_track_current.at(vel - vari.begin());
            //step->vel = *vel;
            if(step->vel < *vel)
            {
                step->vel = *vel;
            }
        }
    }
    else
    {
        m_track_current = m_track_prev_current;
    }
}

bool DTrack::has_events()
{
    // get the current level / variat
    vector<Step>::iterator step;
    // iterate
    for(step = m_track_current.begin(); step != m_track_current.end(); ++step)
    {
        if(step->vel)
        {
            return true;
        }
    }
    return false;
}


void DTrack::evolve(float level, float permute)
{
    m_euclid_density = level;
    m_euclid_permutation = permute;
    
    vector<bool> permuted = m_vanilla_beat;
    vector<bool> shad_permuted = m_shadow_beat;
    vector<bool> alter_permuted = m_alternation_beat;
    
    Euclid::permute(permuted,  m_permutation_places, permute);
    shad_permuted = Euclid::shadow(permuted,m_euclid_bias);
    alter_permuted = Euclid::alternation(permuted, 1, 2, m_euclid_bias);
    // prune alternation et shadow
    Euclid::prune(shad_permuted, 1-m_euclid_evolution_rate);
    Euclid::prune(alter_permuted, 1-m_euclid_evolution_rate);
    
    vector<int> vels = Euclid::assemble(permuted, m_velocities);
    
    float lrate = 0.;
    if(level < 0.5)
    {
        lrate = ofMap(level,0.5,0.,0.,1.);
        lrate = Sine::easeIn(lrate, 0., lrate, 1.);
        lrate = 1- lrate;
        // alternation
        for(vector<bool>::iterator alt = alter_permuted.begin(); alt != alter_permuted.end(); ++alt)
        {
            int ct = alt - alter_permuted.begin();
            if (*alt)
            {
                vels.at(ct) = vels.at(ct) * lrate;
            }
        }
    }
    if(level > 0.5)
    {
        lrate = ofMap(level,0.5,1.,0.,1.);
        
        lrate = Sine::easeIn(lrate, 0., lrate, 1.);
        
        for(vector<bool>::iterator shad = shad_permuted.begin(); shad != shad_permuted.end(); ++shad)
        {
            int ct = shad - shad_permuted.begin();
            if(*shad)
            {
                vels.at(ct) = m_velocity_max * lrate * 0.9; // attenuate
            }
        }
    }
    
    m_track_current = generate_phr(vels, m_track_groove);
    m_track_prev_current = m_track_current;
     
}

void DTrack::update_groove()
{
    vector<Step>::iterator step;
    for(step = m_track_current.begin(); step != m_track_current.end(); ++step)
    {
        step->drift = m_track_groove.at((step - m_track_current.begin())%4);
    }
}

vector<Step> DTrack::generate_phr(vector<int> & vels, vector<float> & groove)
{
    vector<Step> res;
    vector<int>::iterator vel;
    for(vel = vels.begin(); vel != vels.end(); ++vel)
    {
        // create the step
        Step st;
        st.vel = *vel;
        st.dur = 1;
        st.lock = FALSE;
        st.drift = groove.at((vel - vels.begin())%4);
        st.ctrl = map<int,float>();
        st.chance = 1.;
        
        res.push_back(st);
    }
    return res;
}

vector<Step> DTrack::generate_empty_phr(int size)
{
    vector<Step> res;
    for(int i=0; i < size; ++i)
    {
        Step st;
        st.vel = 0;
        st.dur = 1;
        st.lock = FALSE;
        st.ctrl = map<int,float>();
        st.drift = 0.0;
        st.chance = 1.;
        res.push_back(st);
    }
    return res;
}

// byte array to vector of int
vector<int> DTrack::bytes_to_ints(vector<unsigned char> bytes)
{
    vector<int> res;
    vector<unsigned char>::iterator cbyte;
    for(cbyte = bytes.begin(); cbyte != bytes.end(); ++cbyte)
    {
        int a = (*cbyte & 0x0f);
        int b = *cbyte >> 4;
        res.push_back(a);
        res.push_back(b);
    }
    return res;
}

// vector of steps to byte array
vector<unsigned char> DTrack::steps_to_bytes(vector<Step> *phr)
{
    vector<unsigned char> res;
    int size = phr->size();
    if (size%2 == 1)
    {
        size--;
    }
    for(int i = 0; i < size; i+=2)
    {
        unsigned char cbyte;
        cbyte = (cbyte & 0xF0) | (phr->at(i).vel & 0xF); // write low quartet
        cbyte = (cbyte & 0x0F) | ((phr->at(i+1).vel & 0xF) << 4);
        res.push_back(cbyte);
    }
    return res;
}


// get a variation according to the weighted jaccard method
vector<int> DTrack::jaccard_variation(vector<Step> *phr, float thres)
{
    thres = ofClamp(thres, 0.97, 0.99);
    int inc = 4;
    vector<int> target;
    vector<int> goal;
    target = steps_to_vel(phr);
    for(int i = 0; i < target.size(); i += inc)
    {
        int to = (inc > target.size()-i)? target.size()-i : inc;
        to += i;
        vector<int>::const_iterator beg = target.begin() + i;
        vector<int>::const_iterator end = beg + (to-i);
        vector<int> part(beg,end);
        vector<int> res;
        float score = 1;
        int stedv = 15;
        vector<int> rnd;
        while (score > thres) {
            
            for(int j = i; j < to; ++j)
            {
                rnd.push_back(ofClamp(Euclid::normal(target.at(j),stedv),0,15));
            }
            res = rnd;
            score = DTrack::wjacc(rnd, part);
            rnd.clear();
            stedv -= 0.1;
        }
        goal.insert(goal.end(), res.begin(), res.end());
    }
    DTrack::upper_compressor(&goal,m_velocity_max);
    return goal;
}

vector<int> DTrack::steps_to_vel(vector<Step> *phr)
{
    vector<int> res;
    vector<Step>::iterator step;
    for(step = phr->begin(); step != phr->end(); ++step)
    {
        res.push_back(step->vel);
    }
    return res;
}

// weighted jaccard distance
float DTrack::wjacc(vector<int>& s1, vector<int>& s2)
{
    int l1, l2;
    l1 = s1.size();
    l2 = s2.size();
    int ct = 0;
    float  a = 0, b = 0 , same = 0, diff = 0 ;
    if (l1 == 0 || l1 != l2) // lengh > 0 and same length
    {
        return -1.;
    }
    if(s1 == s2)
    {
        return 0.;
    }
    while (ct != l1)
    {
        if (s1.at(ct) == s2.at(ct)) {
            ++same;
        } else {
            a = pow((float)s1.at(ct),2);
            b = pow((float)s2.at(ct),2);
            diff += abs(a-b);
        }
        ++ct;
    }
    return (1 - (same / (diff + same)));
}
void DTrack::upper_compressor(vector<int> *phr, int max_vel)
{
    vector<int>::iterator vel;
    for(vel = phr->begin(); vel != phr->end(); ++vel)
    {
        if(*vel > max_vel)
        {
            *vel = max_vel;
        }
    }
}

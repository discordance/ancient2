#pragma once
#include "ofMain.h"

class RefreshEvent : public ofEventArgs {
    
public:
    
    std::vector< std::vector<int> > velocities;
    string   message;
    
    RefreshEvent() {

    }
    
    static ofEvent <RefreshEvent> events;
};
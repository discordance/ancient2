#ifndef _H_KEYMODIFIERS
#define _H_KEYMODIFIERS

#include "ofMain.h"

#ifdef __OBJC__
@class AppleKeyModifiers;
#endif

class KeyModifier {
    
public:
    
    KeyModifier();
    ~KeyModifier();
    
    bool getAppleCommandModifier();
    bool getAppleShiftModifier();
    int getKey();
    
private:
    
#ifdef __OBJC__
    AppleKeyModifiers* modifiers; //only obj-c needs to know the type of this protected var
#else
    void* modifiers;
#endif
};


#endif
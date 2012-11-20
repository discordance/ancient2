#ifndef STEP
#define STEP

#include <map>

using namespace std;

struct Step {
    int vel;
    int dur;
    bool lock;
    float drift;
    float chance;
    map<int,float> ctrl;
};

#endif
#ifndef MonteCarlo_Glass_h
#define MonteCarlo_Glass_h
#include "Primitives/Refraction.h"
class Glass : public Refraction {
protected:
    virtual float refracitveIndex(float wavelength) const{
        float a = (wavelength-380.f)/(780.f-380.f);
        return 0.5f*(1.f-a)+1.5f*a;
    }
};

#endif

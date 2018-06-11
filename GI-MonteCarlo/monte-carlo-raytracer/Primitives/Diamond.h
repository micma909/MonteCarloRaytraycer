#ifndef MonteCarlo_Diamond_h
#define MonteCarlo_Diamond_h
#include "Primitives/Refraction.h"
class Diamond : public Refraction {
protected:
    virtual float refracitveIndex(float wavelength) const{
        return std::exp(-0.01*wavelength)*20+2.4;
    }
};

#endif

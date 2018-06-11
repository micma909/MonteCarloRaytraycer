#ifndef MonteCarlo_Refraction_h
#define MonteCarlo_Refraction_h
#include "Util/Ray.h"


class Refraction {
public:
    virtual float refracitveIndex(float wavelength) const = 0;
};

#endif

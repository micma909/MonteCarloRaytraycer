#ifndef MonteCarlo_Cauchy_h
#define MonteCarlo_Cauchy_h
#include "Primitives/Refraction.h"
class Cauchy : public Refraction {
protected:
    float B,C;
public:
    Cauchy(float b,float c):B(b),C(c){
        
    }
    virtual float refracitveIndex(float wavelength) const{
        float l = wavelength/1000.f; //Convert nanometer to micrometer
        return B+C/(l*l);
    }
    
    static Cauchy borosilicateGlassBK7(){
        return Cauchy(1.5046f,0.00420f);
    }
    
    static Cauchy hardCrownGlassK5(){
        return Cauchy(1.5220f,0.00459f);
    }
    
    static Cauchy bariumFlintGlassBaF10(){
        return Cauchy(1.6700f,0.00743f);
    }
    
    static Cauchy denseFlintGlassSF10(){
        return Cauchy(1.7280f, 0.01342f);
    }
    static Cauchy johannesGlass(){
        return Cauchy(1.3332f, 0.1572f);
    }
    
    static Cauchy michoGlass(){
        return Cauchy(1.2520f, 0.09f);
    }
    
    
};

#endif

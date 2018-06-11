#ifndef MonteCarlo_Sellmeier_h
#define MonteCarlo_Sellmeier_h
#include "Primitives/Refraction.h"
class Sellmeier : public Refraction {
protected:
    float B1, B2, B3, C1, C2, C3;
public:
    Sellmeier(float b1,float b2,float b3,float c1,float c2,float c3):B1(b1),B2(b2),B3(b3),C1(c1),C2(c2),C3(c3){
        
    }
    virtual float refracitveIndex(float wavelength) const{
        float l = wavelength/1000.f; //Convert nanometer to micrometer
        float l2 = l*l;
        return std::sqrt(1.f+(B1*l2)/(l2-C1)+(B2*l2)/(l2-C2)+(B3*l2)/(l2-C3));
    }
    
    static Sellmeier borosilicateCrownGlass(){
        return Sellmeier(1.03961212,
                         0.231792344,
                         1.01046945,
                         0.00600069867,
                         0.0200179144,
                         103.560653);
    }
    
    
    static Sellmeier sapphire(){
        return Sellmeier(1.43134930,
                         0.65054713,
                         5.3414021,
                         0.0052799261,
                         0.0142382647,
                         325.017834);
    }
    
    static Sellmeier diamond(){
        /*
         B1 = 4.3356;
         B2 = 0.3306;
         B3 = 0;
         C1 = 0.1060^2;
         C2 = 0.1750^2;
         C3 = 0;
         */
        return Sellmeier(4.3356,
                         0.3306,
                         0,
                         0.1060*0.1060,
                         0.175*0.175,
                         0);
    }
    
};

#endif

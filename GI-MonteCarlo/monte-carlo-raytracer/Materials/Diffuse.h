#ifndef DIFFUSE_H
#define DIFFUSE_H

#include "Util/Vec.h"
#include "Material.h"
#include "Definitions.h"

// NOTE: before reading this, go over LambdaRaytracer.h and LambdaMonteCarlo.h

class Diffuse : public Material{
public:
    
	Diffuse(float rho = 1.0f) : rho_(rho){
        _reflectance = [](const Material &m,const Vec3f& x,float wavelength)->float{ return 1.0f;};
        _emitt = [](const Material &m,const Vec3f &incident, const Vec3f &Normal, float wavelength)->float{ return 0.0;};
	}
    
	virtual float emittance(const Vec3f &incident, const Vec3f &Normal, float wavelength) {
		return _emitt(*this, incident, Normal, wavelength);
	}
	
    virtual void sampleHemisphere(float &phi,
                                  float &theta,
                                  const Vec3f& incident,
                                  const Vec3f &normal,
                                  random_kit_t& random) const {
        phi   = random.zero_to_two_pi(random.generator);
        theta = std::acos(std::sqrt(random.zero_to_one(random.generator)));
    }
    
    virtual float pdf(const Vec3f& incident, const Vec3f& exitant, const Vec3f &Normal,const Vec3f& x, float wavelength){
        float costheta = Normal.dot(incident);
        return costheta/M_PI;
    }
    
	virtual float BRDF(const Vec3f& incident, const Vec3f& exitant, const Vec3f &Normal,const Vec3f& x, float wavelength){
		return (rho_)*_reflectance(*this,x,wavelength)/M_PI;
	}
    
    virtual float importanceSampledRadiance(const Vec3f& incident, const Vec3f& exitant, const Vec3f &Normal,const Vec3f& x, float wavelength){
        return BRDF(incident, exitant, Normal, x, wavelength)*M_PI;
    }
    
    DYN_FNK(reflectance, float, const Material &m,const Vec3f& x,float wavelength);
    DYN_FNK(emitt, float, const Material &m, const Vec3f &incident, const Vec3f &Normal, float wavelength);
private:
	float rho_;
protected:
};

#endif
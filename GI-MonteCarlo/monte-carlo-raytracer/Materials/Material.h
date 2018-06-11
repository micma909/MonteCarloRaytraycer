#ifndef MATERIAL_H
#define MATERIAL_H

#include "Util/Vec.h"
#include "Util/random_kit.h"
#include <assert.h>

// NOTE: before reading this, go over LambdaRaytracer.h and LambdaMonteCarlo.h
class Primitive;
class Material{
public:
    const Primitive *primitive_;
public:
    Material(){
        primitive_ = NULL;
        _reflectance = [](const Material *m,const Vec3f& x,float wavelength)->float{ return 1.0f;};
    }
	virtual float emittance(const Vec3f &incident, const Vec3f &Normal, float wavelength) = 0;
	// SampleHemisphere: Creates exitant ray for point from incident and normal. Exitant lobe (all ray-cone) determined
	// by subclassed BRDF materials. Wavelength in nm [380 - 780].

	// INPUT  : [surface point, incoming vector, surface normal, wavelength, Mersenne twister rand variable] 
	// OUTPUT : [exitant vector of hemisphere]
    virtual Vec3f sampleHemisphere(const Vec3f& incident, const Vec3f &n,random_kit_t& random) const{
        float phi,theta;
        sampleHemisphere(phi, theta, incident, n, random);
        float sin_t = std::sin(theta);
        Vec3f exitant(std::cos(phi)*sin_t,
                      std::cos(theta),
                      std::sin(phi)*sin_t);
        exitant = exitant.rotate(n);
        exitant.normalize();
        angle_90_deg(exitant,n,0.001f);

        return exitant;
    }

	// Reflectance: Returns radiance based on particular wavelength //
	// INPUT  : [incoming vector, exitant vector, surface normal, if sampled (always true for now), Mersenne twister rand]
    virtual float BRDF(const Vec3f& incident, const Vec3f& exitant, const Vec3f &Normal,const Vec3f& x, float wavelength) = 0;
	virtual float pdf(const Vec3f& incident, const Vec3f& exitant, const Vec3f &Normal,const Vec3f& x, float wavelength) = 0;

    virtual float importanceSampledRadiance(const Vec3f& incident, const Vec3f& exitant, const Vec3f &Normal,const Vec3f& x, float wavelength) = 0;


    P_DYN_FNK(reflectance, float, const Material *m,const Vec3f& x,float wavelength);

protected:
    virtual void sampleHemisphere(float &phi,
                                  float &theta,
                                  const Vec3f& incident,
                                  const Vec3f& normal,
                                  random_kit_t& random) const = 0;
};
#endif
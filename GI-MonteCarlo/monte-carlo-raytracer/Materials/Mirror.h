#ifndef MIRROR_H
#define MIRROR_H

#include "Util/Vec.h"
#include "Material.h"
// NOTE: before reading this, go over LambdaRaytracer.h and LambdaMonteCarlo.h

class Mirror : public Material{
public:
	Mirror():Material(){
        _reflectance = [](const Material *m,const Vec3f& x,float wavelength)->float{ return 1.0f;};
        _emitt = [](const Material &m,const Vec3f &incident, const Vec3f &Normal, float wavelength)->float{ return 0.0;};
	}

	virtual float emittance(const Vec3f &incident, const Vec3f &Normal, float wavelength) {
		return _emitt(*this,incident,Normal,wavelength);
	}
    
    virtual void sampleHemisphere(float &phi,
                                  float &theta,
                                  const Vec3f& incident,
                                  const Vec3f &normal,
                                  random_kit_t& random) const {
        assert(false);
        //Disabled.
    }
    virtual Vec3f sampleHemisphere(const Vec3f& incident, const Vec3f &n) const{
        assert(incident.length() > 1.f-EPSILON && incident.length() < 1.f+EPSILON);
        assert(n.length() > 1.f-EPSILON && n.length() < 1.f+EPSILON);
        
        Vec3f ex = (2.f*(n.dot(incident))*n)-incident;
        //ex.normalize();
        /*assert(incident.dot(n) > -EPSILON && incident.dot(n) < 1.f+EPSILON );
         assert(ex.dot(n) > -EPSILON && ex.dot(n) < 1.f+EPSILON );
         
         float costheta = n.dot(incident);
         bool test = (costheta > -0.1f && costheta < 1.1f);
         if(!test){
         n.print();
         std::cout << std::endl;
         incident.print();
         std::cout << "Cos:" << costheta << std::endl;
         }
         //assert(test);
         */
        angle_90_deg(ex,n,0.001f);
        
        return ex;
    }

    virtual Vec3f sampleHemisphere(const Vec3f& incident, const Vec3f &n,random_kit_t& random) const{
        return sampleHemisphere(incident, n);
    }
	
	virtual float BRDF(const Vec3f& incident,
                       const Vec3f& exitant,
                       const Vec3f &Normal,
                       const Vec3f& x,
                       float wavelength){
        float angle = sampleHemisphere(incident, Normal).dot(exitant);
        if(angle < EPSILON+1.f && angle > -EPSILON+1.f){
            return _reflectance(this,x,wavelength);
        }
        return 0.f;
    }
    
    virtual float pdf(const Vec3f& incident, const Vec3f& exitant, const Vec3f &Normal,const Vec3f& x, float wavelength){
        return 1.f;
    }
    
    virtual float importanceSampledRadiance(const Vec3f& incident, const Vec3f& exitant, const Vec3f &Normal,const Vec3f& x, float wavelength){
        float costheta = Normal.dot(exitant);
        return costheta;
    }

    
    DYN_FNK(emitt, float, const Material &m, const Vec3f &incident, const Vec3f &Normal, float wavelength);
    
private:
protected:
};
#endif
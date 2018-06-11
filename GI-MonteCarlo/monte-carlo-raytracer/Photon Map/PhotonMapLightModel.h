#ifndef __MonteCarlo__PhotonMapLightModel__
#define __MonteCarlo__PhotonMapLightModel__

#include <iostream>
#include <memory>
#include <map>
#include "LightModels/LightModel.h"
#include "LightModels/MonteCarlo.h"
#include "Util/KDTree.h"
#include "Photon Map/Photon.h"

class PhotonMapLightModel : public LightModel{
private:
    MonteCarlo monteCarlo;
    const int Ni_;
public:
    //KDTree<float, 3,Photon> *photonMap;
    KDTree<float, 3,Photon> *global_,*caustics_;
    CUDA_CALLABLE_MEMBER PhotonMapLightModel(const std::shared_ptr<Scene> &s, KDTree<float, 3,Photon> *global,KDTree<float, 3,Photon> *caustics,int Ni = 20):LightModel(s),global_(global),caustics_(caustics),monteCarlo(s,10,0),Ni_(Ni){
	}
    CUDA_CALLABLE_MEMBER ~PhotonMapLightModel(){}
    inline float gaussianFilter(float d,float r) const{
        /*
        float a = 0.918;
        float b = 1.953;
        float enumer = 1.f-std::exp(-b*((d*d)/(2*r*r)));
        float denom = 1.f-std::exp(-b);
         return a*(1.f-(enumer/denom));
        */
        float k = 2.f;
        float w = std::max(0.f,1.f - d/(k*r));
        return w/(1.f-2.f/(3.f*k));
    }
    CUDA_CALLABLE_MEMBER float indirectRadiance(const Primitive *p,const Vec3f &x,const Vec3f &indicent,float wavelength) const{
        const float max_distance = 0.10f;
        float radiance = 0;
        Vec3f normal = p->normal(x);
        std::shared_ptr<Material> m = p->getMaterial();
        std::multimap<float, Photon > closest =  global_->search_n(x,30, max_distance);
        float r2 = (closest.rbegin())->first;
        float r = std::sqrt(r2);
        for (auto &it : closest) {
            Photon photon = (it.second);
            float d = (photon.pos()-x).length();
            float wp =  gaussianFilter(d,r);
            float brdf = m->BRDF(indicent, photon.dir(), normal, x,photon.wavelength());
            radiance += brdf*photon.flux()*wp;
        }
        return radiance/(M_PI*r2);
    }
    
    Vec3f sampleHemisphere(const Vec3f& incident, const Vec3f &n,random_kit_t& random) const{
        float r1   = random.zero_to_two_pi(random.generator);
        float r2 = random.zero_to_one(random.generator);
        float phi = r1*M_PI*2.f;
        float theta = r2*M_PI_2;
        float sin_t = std::sin(theta);
        Vec3f exitant(std::cos(phi)*sin_t,
                      std::cos(theta),
                      std::sin(phi)*sin_t);
        exitant = exitant.rotate(n);
        exitant.normalize();
        //angle_90_deg(exitant,n,0.001f);
        
        return exitant;
    }
    
    CUDA_CALLABLE_MEMBER float indirectRadiance(const Primitive *primitive,
                                                const Ray &r,
                                                const Vec3f &x,
                                                float wavelength,
                                                random_kit_t &random) const{
        float rad = 0.f;
        Vec3f N = primitive->normal(x);
        std::shared_ptr<Material> mat = primitive->getMaterial();
        //Use global photon map
        for(int i = 0; i < Ni_; i++){
            Vec3f exitant = sampleHemisphere(-1.f*r.dir(),N,random);
            float cosTheta = N.dot(exitant);
            Ray ray_in(x, exitant);
            Ray ray_out;
            float t;
            Primitive *p = LightModel::scene_->refractive_hit(ray_in, wavelength,ray_out,t,random);
            if (p != NULL) {
                //Where to sample indirect radiance?
                float Lr = indirectRadiance(p, ray_out.pos(), ray_out.dir()*(-1.f), wavelength);
                rad += Lr*mat->BRDF(r.dir()*(-1.f), exitant, N, x, wavelength)*cosTheta;
            }
            
        }
        
		return rad/float(Ni_);
	}
    
    CUDA_CALLABLE_MEMBER virtual float radiance(const Ray &ray_in, float wavelength,random_kit &rnd) const{
        float radiance = 0;
        
        
        std::shared_ptr<Scene> s = LightModel::scene_;
        float t;
        Ray ray;
        Primitive *p = s->refractive_hit(ray_in, wavelength,ray,t,rnd);
        if (p != NULL) {
            //Hit pos
            Vec<float,3> x = ray.pos()+t*ray.dir();
            Vec3f normal = p->normal(x);
            x += normal*(EPSILON);
            //radiance += indirectRadiance(p, x,ray.dir()*(-1.f) ,wavelength);
            
            //Emittance
            radiance += p->getMaterial()->emittance(ray.dir(), normal, wavelength);
            if(radiance == 0){
                //Direct radiance
                //radiance += monteCarlo.directRadiance(p, ray, x, wavelength, rnd);
                
                //Global Photon map
                radiance += indirectRadiance(p, ray, x, wavelength, rnd);
            }
        }
        //std::cout << radiance << std::endl;
        return radiance;
    }
    
    
};
#endif /* defined(__MonteCarlo__PhotonMapRenderer__) */
